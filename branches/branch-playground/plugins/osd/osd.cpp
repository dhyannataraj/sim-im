/**************************************************************************
                          osd.cpp  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

 Check screen saver state from xscreensaver-command, Copyright (c) 1991-1998
	by Jamie Zawinski <jwz@jwz.org>

 Set the LEDS Methods are taken from setleds. 
 CapsLED-Notification, Copyright (c) 2007 by Tobias Franz <noragen@gmx.net>

*/

#include <qtimer.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qregion.h>
#include <qstyle.h>
#include <qregexp.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QMouseEvent>
#include <QDesktopWidget>

#include "fontedit.h"
#include "log.h"
#include "core.h"
#include "icons.h"

#include "osd.h"
#include "osdconfig.h"




#ifndef Q_WS_WIN
	/*#include <stdio.h>
	#include <fcntl.h>
	#include <string.h>
	#include <getopt.h>
	#include <linux/kd.h>
	#include <sys/ioctl.h>

	#include "local.h"
	#include "utils.h"
	#define KD "/dev/console"	*/

	#if !defined(QT_MACOSX_VERSION) && !defined(QT_MAC) && !defined(__OS2__)
		#include <X11/Xlib.h>
		#include <X11/Xutil.h>
		#include <X11/Xatom.h>
	#endif
#endif

using namespace std;
using namespace SIM;

const unsigned SHADOW_DEF	= 2;
const unsigned XOSD_MARGIN	= 5;

Plugin *createOSDPlugin(unsigned base, bool, Buffer*)
{
    Plugin *plugin = new OSDPlugin(base);
    return plugin;
}

static PluginInfo info =
    {
        I18N_NOOP("OSD"),
        I18N_NOOP("Plugin provides on screen notification about messages and contacts status"),
        VERSION,
        createOSDPlugin,
        PLUGIN_DEFAULT
    };

EXPORT_PROC PluginInfo* GetPluginInfo()
{
    return &info;
}

static DataDef osdUserData[] =
    {
        { "EnableMessage", DATA_BOOL, 1, DATA(1) },
        { "EnableMessageShowContent", DATA_BOOL, 1, DATA(0) },
        { "EnableCapsLockFlash", DATA_BOOL, 1, DATA(0) },
        { "ContentTypes", DATA_ULONG, 1, DATA(3) },
        { "EnableAlert", DATA_BOOL, 1, DATA(1) },
        { "EnableAlertOnline", DATA_BOOL, 1, DATA(1) },
        { "EnableAlertAway", DATA_BOOL, 1, DATA(0) },
        { "EnableAlertNA", DATA_BOOL, 1, DATA(0) },
        { "EnableAlertDND", DATA_BOOL, 1, DATA(0) },
        { "EnableAlertOccupied", DATA_BOOL, 1, DATA(0) },
        { "EnableAlertFFC", DATA_BOOL, 1, DATA(0) },
        { "EnableAlertOffline", DATA_BOOL, 1, DATA(0) },
        { "EnableTyping", DATA_BOOL, 1, 0 },
        { "Position", DATA_ULONG, 1, 0 },
        { "Offset", DATA_ULONG, 1, DATA(30) },
        { "Color", DATA_ULONG, 1, DATA(0x00E000) },
        { "Font", DATA_STRING, 1, 0 },
        { "Timeout", DATA_ULONG, 1, DATA(7) },
        { "Shadow", DATA_BOOL, 1, DATA(1) },
		{ "Fading", DATA_BOOL, 1, DATA(1) },
        { "Background", DATA_BOOL, 1, 0 },
        { "BgColor", DATA_ULONG, 1, 0 },
        { "Screen", DATA_ULONG, 1, 0 },
        { NULL, DATA_UNKNOWN, 0, 0 }
    };

static OSDPlugin *osdPlugin = NULL;

static QWidget *getOSDSetup(QWidget *parent, void *data)
{
    return new OSDConfig(parent, data, osdPlugin);
}

OSDPlugin::OSDPlugin(unsigned base) : Plugin(base)
{
    osdPlugin    = this;

    user_data_id = getContacts()->registerUserData(info.title, osdUserData);
    Command cmd;
    cmd->id		 = user_data_id;
    cmd->text	 = I18N_NOOP("&OSD");
    cmd->icon	 = "alert";
    cmd->param	 = (void*)getOSDSetup;
    EventAddPreferences(cmd).process();

    m_request.contact = 0;
    m_request.type    = OSD_NONE;

    m_osd   = NULL;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
	bCapsState = false;

    EventGetPluginInfo ePlugin("_core");
    ePlugin.process();
    const pluginInfo *info = ePlugin.info();
    core = static_cast<CorePlugin*>(info->plugin);
    bHaveUnreadMessages=false;
}

OSDPlugin::~OSDPlugin()
{
    delete m_osd;
    osdPlugin = NULL;
    EventRemovePreferences(user_data_id).process();
    getContacts()->unregisterUserData(user_data_id);
}

QWidget *OSDPlugin::createConfigWindow(QWidget *parent)
{
    return new OSDConfig(parent, getContacts()->getUserData(user_data_id), this);
}

void OSDPlugin::timeout()
{
    m_osd->hide();
    m_timer->stop();
    processQueue();
}

QFont OSDPlugin::getBaseFont(QFont font)
{
    QFont baseFont;

    baseFont = font;
    int size = baseFont.pixelSize();
    if (size <= 0)
    {
        size = baseFont.pointSize();
        baseFont.setPointSize(size * 2);
    }
    else
    {
        baseFont.setPixelSize(size * 2);
    }
    baseFont.setBold(true);

    return baseFont;
}

OSDWidget::OSDWidget(OSDPlugin *plugin)
    : QWidget(NULL, "osd", Qt::Tool |
        Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint )
{
    m_plugin = plugin;
    m_image = NULL;
    baseFont = m_plugin->getBaseFont(font());
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAutoFillBackground(false);
    connect(&m_transTimer, SIGNAL(timeout()), this, SLOT(slotTimerFadeInTimeout()));
    QPalette pal = palette();
    pal.setColor(QPalette::Background,Qt::transparent);
    setPalette(pal);
}

bool OSDWidget::isScreenSaverActive()
{
/*
#ifdef Q_WS_WIN
    BOOL pvParam;
    if (SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &pvParam, 0)){
        if (pvParam)
            return true;
    }
#endif
*/
    return false;
}

static const char * const close_h_xpm[] = {
            "8 8 3 1",
            "	    c None",
            ".	    c #000000",
            "+	    c none",
            ".++++++.",
            "+.++++.+",
            "++.++.++",
            "+++..+++",
            "+++..+++",
            "++.++.++",
            "+.++++.+",
            ".++++++."};

	static int const cFadeTime = 50;

	void OSDWidget::showOSD(const QString &str, OSDUserData *data)
{
    currentData = *data;
	m_bFading = data->Fading.toBool();
    m_sText = str;
    if (isScreenSaverActive())
    {
        hide();
        return;
    }
    m_text_y = 0;
    m_bBackground = data->Background.toBool();
    m_bShadow = data->Shadow.toBool();

    setFont(FontEdit::str2font(data->Font.str(), baseFont));

    //int SHADOW_OFFS = SHADOW_DEF;

    recalcGeometry();
    resize(m_Rect.size());

    if(m_image != NULL )
    {
        delete m_image;
        m_image = NULL;
    }
    m_image = new QImage(size(),QImage::Format_ARGB32);

	if (m_bFading)
		m_image->fill(Qt::transparent);

    QPainter p(m_image);
    draw(p);

	if (m_bFading)
		setMask(QPixmap(m_image->createAlphaMask()));

    transCounter = 0;
    transCounterDelta = 5;

    QWidget::show();
    raise();
    
	if (m_bFading)
		m_transTimer.start(cFadeTime);

}

QRect OSDWidget::recalcGeometry()
{
    int SHADOW_OFFS = SHADOW_DEF;
    unsigned nScreen = currentData.Screen.toULong();
    unsigned nScreens = screens();
    if (nScreen >= nScreens)
        nScreen = 0;
    QRect rcScreen = screenGeometry(nScreen);
    rcScreen = QRect(0, 0,
                     rcScreen.width()  - SHADOW_OFFS - XOSD_MARGIN * 2 - currentData.Offset.toULong(),
                     rcScreen.height() - SHADOW_OFFS - XOSD_MARGIN * 2 - currentData.Offset.toULong());
    QFontMetrics fm(font());
    QRect rc = fm.boundingRect(rcScreen, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_sText);
    if (rc.height() >= rcScreen.height() / 2){
        rcScreen = QRect(0, 0,
                         rcScreen.width() - SHADOW_OFFS - XOSD_MARGIN * 2 - currentData.Offset.toULong(),
                         rcScreen.height() - SHADOW_OFFS - XOSD_MARGIN * 2 - currentData.Offset.toULong());
        rc = fm.boundingRect(rcScreen, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_sText);
    }
    if (currentData.EnableMessageShowContent.toBool() && currentData.ContentLines.toULong())
    {
        QFontMetrics fm(font());
        int maxHeight = fm.height() * (currentData.ContentLines.toULong() + 1);
        if (rc.height() > maxHeight)
            rc.setHeight(maxHeight);
    }
    int x = rcScreen.left();
    int y = rcScreen.top();
    int w = rc.width() + 1;
    int h = rc.height() + 1;
    m_text_y = 0;
    if (m_bBackground)
    {
        w += XOSD_MARGIN * 2;
        h += XOSD_MARGIN * 2;
        if (m_imageButton.isNull())
        {
            m_imageButton = Image("button_cancel");
			if( m_imageButton.isNull() )
				m_imageButton = QPixmap((const char **)close_h_xpm);
        }
        m_rectButton = QRect(QPoint(w - m_imageButton.width() - 3, 3),m_imageButton.size());
        m_text_y = m_imageButton.height() + 4;
        h += m_text_y;
    }
    if (m_bShadow)
    {
        w += SHADOW_OFFS;
        h += SHADOW_OFFS;
    }
    switch (currentData.Position.toULong()){
    case 1:
        move(x + currentData.Offset.toULong(), y + currentData.Offset.toULong());
        break;
    case 2:
        move(x + rcScreen.width() - currentData.Offset.toULong() - w, y + rcScreen.height() - currentData.Offset.toULong() - h);
        break;
    case 3:
        move(x + rcScreen.width() - currentData.Offset.toULong() - w, y + currentData.Offset.toULong());
        break;
    case 4:
        move(x + (rcScreen.width() - w) / 2, y + rcScreen.height() - currentData.Offset.toULong() - h);
        break;
    case 5:
        move(x + (rcScreen.width() - w) / 2, y + currentData.Offset.toULong());
        break;
    case 6:
        move(x + (rcScreen.width() - w) / 2, y + (rcScreen.height() - h) /2);
        break;
    default:
        move(x + currentData.Offset.toULong(), y + rcScreen.height() - currentData.Offset.toULong() - h);
    }

    m_Rect = QRect(x,y,w,h);
	return m_Rect;
}

QSize OSDWidget::sizeHint() const
{
    return m_Rect.size();
}

void OSDWidget::slotTimerFadeInTimeout()
{
    transCounter += transCounterDelta;
    update();
    if (transCounter>100)
    {
        transCounter = 100;
        m_transTimer.stop();
    }
    else if (transCounter<=0)
    {
        transCounter = 0;
        m_transTimer.stop();
        QWidget::hide();
    }
}

void OSDWidget::draw(QPainter &p)
{
    QSize s = size();
    int w = s.width();
    int h = s.height();
    QRect rc = QRect(0, 0, w, h);

    if (m_bBackground)
    {
        if (m_bShadow)
        {
            w -= SHADOW_DEF;
            h -= SHADOW_DEF;
            rc = QRect(0, 0, w, h);
        }
        p.setPen(QPen(QColor(0x00,0x00,0x00)));
        p.setBrush(QBrush(currentData.BgColor.toULong()));
        p.drawRoundedRect(rc,7,7);
        p.drawImage(m_rectButton,m_imageButton);
        rc = QRect(XOSD_MARGIN, XOSD_MARGIN, w - XOSD_MARGIN * 2, h - XOSD_MARGIN * 2);
    }

    rc.translate(0,m_text_y);
    p.setFont(font());

    if( m_bShadow )
    {
        p.setPen(QColor(0x80,0x80,0x80,0x80));
        QRect src(rc);
        src.translate(SHADOW_DEF,SHADOW_DEF);
        p.drawText(src, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_sText);
    }

    p.setPen(currentData.Color.toULong());
    p.drawText(rc, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_sText);
}

void OSDWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if(NULL != m_image)
    {
        QPixmap image(*m_image);
        unsigned char alpha =(unsigned char) QMIN((int)(transCounter * 256 / 100), 255);
        QPixmap alphaChannel = image.alphaChannel();
        alphaChannel.fill(QColor(alpha,alpha,alpha,alpha));
        image.setAlphaChannel(alphaChannel);
        p.drawPixmap(QPoint(0,0),image);
    }
    else
    {
        draw(p);
    }
}

void OSDWidget::mouseDoubleClickEvent(QMouseEvent*)
{
    emit dblClick();
    close();
}

void OSDWidget::mousePressEvent(QMouseEvent *event)
{
    if(m_rectButton.contains(event->pos()))
        emit closeClick();
}

void OSDWidget::slotCloseClick()
{
    emit closeClick();
}

void OSDWidget::hide()
{
	if( m_bFading )
	{
		transCounter = 100;
		transCounterDelta = -5;
		m_transTimer.start(cFadeTime);
	}
	else
	{
		QWidget::hide();
	}
}

#if 0
i18n("male", "%1 is online")
i18n("female", "%1 is online")
i18n("male", "%1 is away")
i18n("female", "%1 is away")
i18n("male", "%1 is not available")
i18n("female", "%1 is not available")
i18n("male", "%1 doesn't want to be disturbed")
i18n("female", "%1 doesn't want to be disturbed")
i18n("male", "%1 is occupied")
i18n("female", "%1 is occupied")
i18n("male", "%1 is free for chat")
i18n("female", "%1 is free for chat")
i18n("male", "%1 is offline")
i18n("female", "%1 is offline")
i18n("male", "%1 is typing")
i18n("female", "%1 is typing")
#endif

typedef map<unsigned, unsigned> TYPE_MAP;

void OSDPlugin::processQueue()
{
    if (m_timer->isActive())
        return;
    while (queue.size()){
        m_request = queue.front();
        queue.erase(queue.begin());
        Contact *contact = getContacts()->contact(m_request.contact);
        if ((contact == NULL) || contact->getIgnore()){
            continue;
        }
        QString text;
        OSDUserData *data = NULL;
        data = (OSDUserData*)contact->getUserData(user_data_id);
		uint ms=core->getManualStatus();
        switch (m_request.type){
        case OSD_ALERTONLINE:
            if (data->EnableAlert.toBool() && data->EnableAlertOnline.toBool()){
                unsigned style = 0;
                QString statusIcon;
                if (contact->contactInfo(style, statusIcon) == STATUS_ONLINE)
                    text = g_i18n("%1 is online", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTAWAY:
            if (data->EnableAlert.toBool() && data->EnableAlertAway.toBool()){
                text = g_i18n("%1 is away", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTNA:
            if (data->EnableAlert.toBool() && data->EnableAlertNA.toBool()){
                text = g_i18n("%1 is not available", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTDND:
            if (data->EnableAlert.toBool() && data->EnableAlertDND.toBool()){
                text = g_i18n("%1 doesn't want to be disturbed", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTOCCUPIED:
            if (data->EnableAlert.toBool() && data->EnableAlertOccupied.toBool()){
                text = g_i18n("%1 is occupied", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTFFC:
            if (data->EnableAlert.toBool() && data->EnableAlertFFC.toBool()){
                text = g_i18n("%1 is free for chat", contact) .arg(contact->getName());
            }
            break;
        case OSD_ALERTOFFLINE:
            if (data->EnableAlert.toBool() && data->EnableAlertOffline.toBool() && (ms-1) ){
                text = g_i18n("%1 is offline", contact) .arg(contact->getName());
            }
            break;
        case OSD_TYPING:
            if (data->EnableTyping.toBool()){
                unsigned style = 0;
                QString wrkIcons;
                QString statusIcon;
                contact->contactInfo(style, statusIcon, &wrkIcons);
                bool bTyping = false;
                while (!wrkIcons.isEmpty()){
                    if (getToken(wrkIcons, ',') == "typing"){
                        bTyping = true;
                        break;
                    }
                }
                if (bTyping)
                    text = g_i18n("%1 is typing", contact) .arg(contact->getName());
            }
            break;
        case OSD_MESSAGE:
           if (data->EnableMessage.toBool() && core){
				list<msg_id>::iterator it;
                TYPE_MAP types;
                TYPE_MAP::iterator itc;
                QString msg_text;
                for (it = core->unread.begin(); it != core->unread.end(); ++it){
                    if ((*it).contact != m_request.contact)
                        continue;
                    unsigned type = (*it).type;
                    itc = types.find(type);
                    if (itc == types.end()){
                        types.insert(TYPE_MAP::value_type(type, 1));
                    }else{
                        (*itc).second++;
                    }
                    if (!data->EnableMessageShowContent.toBool())
                        continue;
                    EventLoadMessage e((*it).id, (*it).client, (*it).contact);
                    e.process();
                    Message *msg = e.message();
                    if (msg == NULL)
                        continue;
                    QString msgText = msg->getPlainText().trimmed();
                    if (msgText.isEmpty())
                        continue;
                    if (!msg_text.isEmpty())
                        msg_text += "\n";
                    msg_text += msgText;
                }
                if (types.empty())
                    break;
                for (itc = types.begin(); itc != types.end(); ++itc){
                    CommandDef *def = core->messageTypes.find((*itc).first);
                    if (def == NULL)
                        continue;
                    MessageDef *mdef = (MessageDef*)(def->param);
                    QString msg = i18n(mdef->singular, mdef->plural, (*itc).second);
                    if ((*itc).second == 1){
                        int pos = msg.indexOf("1 ");
                        if (pos > 0){
                            msg = msg.left(pos);
                        }else if (pos == 0){
                            msg = msg.mid(2);
                        }
                        msg = msg.left(1).toUpper() + msg.mid(1);
                    }
                    if (!text.isEmpty())
                        text += ", ";
                    text += msg;
                }

				
				if ( core->getManualStatus()==STATUS_NA && 
				     data->EnableCapsLockFlash.toBool() && 
				     ! this->running() 
				   )
				   
					this->start(); //Start flashing the CapsLock if enabled
				text = i18n("%1 from %2") .arg(text) .arg(contact->getName());
                if (msg_text.isEmpty())
                    break;
                text += ":\n";
                text += msg_text;
            }
            break;
        default:
            break;
        }
        if (!text.isEmpty()){
            if (m_osd == NULL){
                m_osd = new OSDWidget(this);
                connect(m_osd, SIGNAL(dblClick()), this, SLOT(dblClick()));
                connect(m_osd, SIGNAL(closeClick()), this, SLOT(closeClick()));
            }
            static_cast<OSDWidget*>(m_osd)->showOSD(text, data);
            m_timer->start(data->Timeout.toULong() * 1000);
            return;
        }
    }
    m_timer->stop();
    m_request.contact = 0;
    m_request.type = OSD_NONE;
}

void OSDPlugin::run(){
	while ( bHaveUnreadMessages ) {
		flashCapsLockLED(!bCapsState);

#ifdef WIN32
		sleepTime(200);
#else
		sleepTime(1);
#endif
	}
	if (bCapsState) flashCapsLockLED(!bCapsState); //switch LED off
}

void OSDPlugin::flashCapsLockLED(bool bCapsState){

#ifdef WIN32
	BYTE keyState[256];

    GetKeyboardState((LPBYTE)&keyState);
    if( ( !(keyState[VK_CAPITAL] & 1)))
		//||
        //(!bCapsState && (keyState[VK_CAPITAL] & 1)) )
      
      // Simulate a key press
         keybd_event( VK_CAPITAL,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | 0,
                      0 );

	 // Simulate a key release
         keybd_event( VK_CAPITAL,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                      0);
		
#elif defined(__OS2__)
    // TODO: add OS/2 code to switch leds
#else
    if (bCapsState)
        system("xset led 3");
    else
        system("xset -led 3");
#endif
		this->bCapsState= bCapsState;
}

void OSDPlugin::closeClick()
{
    if (m_request.type == OSD_MESSAGE){
        for (list<msg_id>::iterator it = core->unread.begin(); it != core->unread.end(); ){
            if ((*it).contact != m_request.contact){
                ++it;
                continue;
            }
            EventLoadMessage e((*it).id, (*it).client, (*it).contact);
            e.process();
            Message *msg = e.message();
            core->unread.erase(it);
            if (msg){
                EventMessageRead(msg).process();
                delete msg;
            }
            it = core->unread.begin();
        }
    }
    timeout();
}

void OSDPlugin::dblClick()
{
    EventDefaultAction(m_request.contact).process();
    m_timer->stop();
    m_timer->start(100);
}

bool OSDPlugin::processEvent(Event *e)
{
    OSDRequest osd;
    switch (e->type()){
    case eEventContact: {
        EventContact *ec = static_cast<EventContact*>(e);
        Contact *contact = ec->contact();
        if (contact->getIgnore())
            break;
        switch(ec->action()) {
        case EventContact::eOnline: {
            osd.contact = contact->id();
            osd.type    = OSD_ALERTONLINE;
            queue.push_back(osd);
            processQueue();
            break;
        }
        case EventContact::eStatus: {
            OSDUserData *data = (OSDUserData*)(contact->getUserData(user_data_id));
            if (data){
                unsigned style = 0;
                QString wrkIcons;
                QString statusIcon;
                contact->contactInfo(style, statusIcon, &wrkIcons);
                bool bTyping = false;
                while (!wrkIcons.isEmpty()){
                    if (getToken(wrkIcons, ',') == "typing"){
                        bTyping = true;
                        break;
                    }
                }
                if (bTyping){
                    list<unsigned>::iterator it;
                    for (it = typing.begin(); it != typing.end(); ++it)
                        if ((*it) == contact->id())
                            break;
                    if (it == typing.end()){
                        typing.push_back(contact->id());
                        osd.contact = contact->id();
                        osd.type    = OSD_TYPING;
                        queue.push_back(osd);
                        processQueue();
                    }
                }else{
                    list<unsigned>::iterator it;
                    for (it = typing.begin(); it != typing.end(); ++it)
                        if ((*it) == contact->id())
                            break;
                    if (it != typing.end())
                        typing.erase(it);
                    if ((m_request.type == OSD_TYPING) && (m_request.contact == contact->id())){
                        m_timer->stop();
                        m_timer->start(100);
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case eEventMessageReceived: {
        EventMessage *em = static_cast<EventMessage*>(e);
        Message *msg = em->msg();
        Contact *contact = getContacts()->contact(msg->contact());
        if (contact == NULL)
            break;
        OSDUserData *data = (OSDUserData*)(contact->getUserData(user_data_id));
        if (data == NULL)
            break;
        osd.contact = msg->contact();
	if (! core->unread.empty())
	    bHaveUnreadMessages=true;
        if (msg->type() == MessageStatus) {
            StatusMessage *smsg = (StatusMessage*)msg;
            switch (smsg->getStatus()) {
            case STATUS_AWAY:
                osd.type = OSD_ALERTAWAY;
                break;
            case STATUS_NA:
                osd.type = OSD_ALERTNA;
                break;
            case STATUS_DND:
                osd.type = OSD_ALERTDND;
                break;
            case STATUS_OCCUPIED:    /* STATUS_OCCUPIED, took over from contacts.h! */
                osd.type = OSD_ALERTOCCUPIED;
                break;
            case STATUS_FFC:
                osd.type = OSD_ALERTFFC;
                break;
            case STATUS_OFFLINE:
                osd.type = OSD_ALERTOFFLINE;
                break;
            case STATUS_ONLINE:
                osd.type = OSD_NONE;
                return false;
            default:
                log(L_DEBUG,"OSD: Unknown status %ld",smsg->getStatus());
                osd.type = OSD_NONE;
                return false;
            }
            queue.push_back(osd);
            processQueue();
        }else{
            osd.type    = OSD_MESSAGE;
            if ((m_request.type == OSD_MESSAGE) && (m_request.contact == msg->contact())){
                queue.push_front(osd);
                m_timer->stop();
                m_timer->start(100);
            }else{
                queue.push_back(osd);
                processQueue();
            }
        }
        break;
    }
    case eEventMessageDeleted:
    case eEventMessageRead: {
        EventMessage *em = static_cast<EventMessage*>(e);
        Message *msg = em->msg();
        Contact *contact = getContacts()->contact(msg->contact());
        if (contact == NULL)
            break;
        OSDUserData *data = (OSDUserData*)(contact->getUserData(user_data_id));
        if (data == NULL)
            break;
        osd.contact = msg->contact();
	if (core->unread.empty())
	    bHaveUnreadMessages=false;
        if (msg->type() == MessageStatus) {
            StatusMessage *smsg = (StatusMessage*)msg;
            switch (smsg->getStatus()) {
            case STATUS_AWAY:
                osd.type = OSD_ALERTAWAY;
                break;
            case STATUS_NA:
                osd.type = OSD_ALERTNA;
                break;
            case STATUS_DND:
                osd.type = OSD_ALERTDND;
                break;
            case STATUS_OCCUPIED:    /* STATUS_OCCUPIED, took over from contacts.h! */
                osd.type = OSD_ALERTOCCUPIED;
                break;
            case STATUS_FFC:
                osd.type = OSD_ALERTFFC;
                break;
            case STATUS_OFFLINE:
                osd.type = OSD_ALERTOFFLINE;
                break;
            case STATUS_ONLINE:
                osd.type = OSD_NONE;
                return false;
            default:
                log(L_DEBUG,"OSD: Unknown status %ld",smsg->getStatus());
                osd.type = OSD_NONE;
                return false;
            }
            queue.push_back(osd);
            processQueue();
        }else{
            osd.type    = OSD_MESSAGE;
            if ((m_request.type == OSD_MESSAGE) && (m_request.contact == msg->contact())){
                queue.push_front(osd);
                m_timer->stop();
                m_timer->start(100);
            }else{
                queue.push_back(osd);
                processQueue();
            }
        }
        break;
    }
    default:
        break;
    }
    return false;
}


/***************************************************************************
                          msgview.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
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
 ***************************************************************************/

#include "msgview.h"
#include "mainwin.h"
#include "client.h"
#include "cuser.h"
#include "history.h"
#include "log.h"
#include "icons.h"
#include "transparent.h"
#include "toolbtn.h"
#include "ui/ballonmsg.h"

#ifdef USE_KDE
#include <keditcl.h>
#include <kstdaccel.h>
#include <kglobal.h>
#include <kfiledialog.h>
#define QFileDialog	KFileDialog
#ifdef HAVE_KROOTPIXMAP_H
#include <krootpixmap.h>
#endif
#else
#include <qfiledialog.h>
#endif

#include <qdatetime.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qobjectlist.h>
#include <qvaluelist.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qtextcodec.h>
#include <qtoolbar.h>
#include <qlineedit.h>
#include <qaccel.h>
#include <qdragobject.h>

#ifdef WIN32
#if _MSC_VER > 1020
#pragma warning(disable:4786)
#endif
#endif

#define MAX_HISTORY	100

TextShow::TextShow(QWidget *p, const char *name)
        : QTextBrowser(p, name)
{
    m_nUin = 0;
    codec = QTextCodec::codecForLocale();
    bg = new TransparentBg(this);
    baseBG = colorGroup().color(QColorGroup::Base);
    baseFG = colorGroup().color(QColorGroup::Text);
    setTextFormat(RichText);
    connect(pClient, SIGNAL(encodingChanged(unsigned long)), this, SLOT(encodingChanged(unsigned long)));
}

void TextShow::setUin(unsigned long uin)
{
    codec = pClient->codecForUser(uin);
    m_nUin = uin;
}

void TextShow::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e);
    scrollToAnchor(curAnchor);
}

void TextShow::resetColors()
{
    setBackground(baseBG);
    setForeground(baseFG);
}

void TextShow::setBackground(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QColorGroup::Base, c);
    pal.setColor(QPalette::Inactive, QColorGroup::Base, c);
    pal.setColor(QPalette::Disabled, QColorGroup::Base, c);
    setPalette(pal);
}

void TextShow::setForeground(const QColor& c)
{
    QPalette pal = palette();

    pal.setColor(QPalette::Active, QColorGroup::Text, c);
    pal.setColor(QPalette::Inactive, QColorGroup::Text, c);

    setPalette(pal);
}

void TextShow::setSource(const QString &href)
{
    if (href.left(6) == "msg://"){
        QString id = href.mid(6);
        int p = id.find('/');
        if (p >= 0) id = id.left(p);
        QStringList s = QStringList::split('.', id);
        if (s.count() >= 2)
            emit goMessage(s[0].toULong(), s[1].toULong());
        return;
    }
    if (href.left(6) == "mailto:"){
        pMain->sendMail(href.mid(6));
        return;
    }
    pMain->goURL(href);
    return;
}

QString TextShow::makeMessageText(ICQMessage *msg, bool bIgnore)
{
    QString s;
    const char *encoding = msg->Charset.c_str();
    if (*encoding == 0)
        encoding = codec->name();
    string msg_text;
    switch (msg->Type()){
    case ICQ_MSGxMSG:
        msg_text = (static_cast<ICQMsg*>(msg))->Message.c_str();
        SIMClient::toUTF(msg_text, encoding);
        s += MainWindow::ParseText(msg_text, bIgnore);
        break;
    case ICQ_MSGxURL:{
            ICQUrl *url = static_cast<ICQUrl*>(msg);
            s += "<a href=\"";
            s += url->URL.c_str();
            s += "\">";
            s += quoteText(url->URL.c_str(), msg->Charset.c_str());
            s += "</a>";
            if (*url->Message.c_str()){
                msg_text = url->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += "<br>";
                s += MainWindow::ParseText(msg_text, true);
            }
            break;
        }
    case ICQ_MSGxAUTHxREQUEST:{
            ICQAuthRequest *req = static_cast<ICQAuthRequest*>(msg);
            s += i18n("Authorization request");
            if (*req->Message.c_str()){
                msg_text = req->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += "<br>";
                s += MainWindow::ParseText(msg_text, true);
            }
            break;
        }
    case ICQ_MSGxAUTHxREFUSED:{
            ICQAuthRefused *req = static_cast<ICQAuthRefused*>(msg);
            s += i18n("Authorization refused");
            if (*req->Message.c_str()){
                msg_text = req->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += "<br>";
                s += MainWindow::ParseText(msg_text, true);
            }
            break;
        }
    case ICQ_MSGxAUTHxGRANTED:
        s += i18n("Authorization granted");
        break;
    case ICQ_MSGxADDEDxTOxLIST:
        s += i18n("Added to contact list");
        break;
    case ICQ_MSGxCONTACTxREQUEST:{
            ICQContactRequest *req = static_cast<ICQContactRequest*>(msg);
            s += i18n("Contact request");
            if (*req->Message.c_str()){
                msg_text = req->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += "<br>";
                s += MainWindow::ParseText(msg_text, true);
            }
            break;
        }
    case ICQ_MSGxFILE:{
            ICQFile *file = static_cast<ICQFile*>(msg);
            QString name = QString::fromLocal8Bit(file->Name.c_str());
            if (name.find(QRegExp("^[0-9]+ Files$")) >= 0){
                s += i18n("File", "%n files", atol(name.latin1()));
            }else{
                s += i18n("File");
                s += ": ";
                s += name;
            }
            s += " (";
            s += QString::number(file->Size);
            s += " ";
            s += i18n("bytes");
            s += ")";
            s += "<br>";
            msg_text = file->Description.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += MainWindow::ParseText(msg_text, true);
            break;
        }
    case ICQ_MSGxCHAT:{
            ICQChat *chat = static_cast<ICQChat*>(msg);
            msg_text = chat->Reason.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += MainWindow::ParseText(msg_text, true);
            break;
        }
    case ICQ_MSGxCONTACTxLIST:{
            ICQContacts *contacts = static_cast<ICQContacts*>(msg);
            s += msg->Received ? i18n("Contact list received") : i18n("Contact list sent");
            s += "<br>";
            s += "<table>";
            for (ContactList::iterator it = contacts->Contacts.begin(); it != contacts->Contacts.end(); it++){
                Contact *contact = static_cast<Contact*>(*it);
                s += "<tr><td align=right>" + QString::number(contact->Uin) + "</td><td>";
                s += quoteText(contact->Alias.c_str(), msg->Charset.c_str());
                s += "</td></tr>";
            }
            s += "</table>";
            break;
        }
    case ICQ_MSGxSMS:{
            ICQSMS *sms = static_cast<ICQSMS*>(msg);
            msg_text = sms->Message.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += MainWindow::ParseText(msg_text, true);
            if (*sms->Phone.c_str()){
                s += "<br>";
                s += quoteText(sms->Phone.c_str(), msg->Charset.c_str());
                if (*sms->Network.c_str())
                    s += " (" + quoteText(sms->Network.c_str(), msg->Charset.c_str()) + ")";
            }
            break;
        }
    case ICQ_MSGxWEBxPANEL:{
            ICQWebPanel *m = static_cast<ICQWebPanel*>(msg);
            msg_text = m->Message.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += MainWindow::ParseText(msg_text, true);
            break;
        }
    case ICQ_MSGxEMAILxPAGER:{
            ICQEmailPager *m = static_cast<ICQEmailPager*>(msg);
            msg_text = m->Message.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += MainWindow::ParseText(msg_text, true);
            break;
        }
    default:
        log(L_WARN, "Unknown message type %u", msg->Type());
        s += "???";
    }
    return s;
}

void TextShow::keyPressEvent(QKeyEvent *e)
{
    if (((e->state() == ControlButton) && (e->key() == Key_C)) ||
            ((e->state() == ShiftButton) && (e->key() == Key_Insert))){
        copy();
        return;
    }
    QTextBrowser::keyPressEvent(e);
}

QString TextShow::quoteText(const char *text, const char *charset)
{
    string msg = ICQClient::quoteText(text);
    QString s = SIMClient::from8Bit(codec, msg, charset);
    return s;
}
static const char *smiles[] =
    {
        ":-)",
        ":-O",
        ":-|",
        ":-/",
        ":-(",
        ":-{}",
        ":*)",
        ":'-(",
        ";-)",
        ":-@",
        ":-\")",
        ":-X",
        ":-P",
        "8-)",
        "O-)",
        ":-D"
    };

QString TextShow::unquoteString(const QString &s, int from, int to) const
{
    if (from < 0) from = 0;
    if (to < 0) to = s.length();
    QString res;
    int n = 0;
    for (unsigned i = 0; i < s.length(); i++){
        if (s[(int)i] == '&'){
            int m = i;
            for (; i < s.length(); i++)
                if (s[(int)i] == ';') break;
            QString ch = s.mid(m, i - m + 1);
            QString r;
            if (ch == "&amp;"){
                r = "&";
            }else if (ch == "&gt;"){
                r = ">";
            }else if (ch == "&lt;"){
                r = "<";
            }else if (ch == "&quot;"){
                r = "\"";
            }
            if (!r.isEmpty()){
                if (n >= to) break;
                if (n >= from) res += r;
                n++;
                continue;
            }
            i = m;
        }
        if (s[(int)i] == '<'){
            unsigned m = i;
            for (; i < s.length(); i++)
                if (s[(int)i] == '>') break;
            QString t = s.mid(m, i - m + 1);
            for (m = 1; m < t.length(); m++)
                if ((t[(int)m] == ' ') || (t[(int)m] == '>')) break;
            QString tag = t.mid(1, m - 1);
            if (tag == "img"){
                int p = t.find("icon:smile");
                if (p >= 0){
                    unsigned nSmile = atol(t.mid(p + 10).latin1());
                    if (nSmile < sizeof(smiles)){
                        if (n >= to) break;
                        if (n >= from) res += smiles[nSmile];
                    }
                }
                n++;
            }else if (tag == "br"){
                if (n >= to) break;
                if (n >= from) res += "\n";
                n++;
            }
            continue;
        }
        if (n >= to) break;
        if (n >= from) res += s[(int)i];
        n++;
    }
    return res;
}

QString TextShow::selectedText() const
{
    QString res;
    int paraFrom, paraTo, indexFrom, indexTo;
    getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
    if ((paraFrom > paraTo) || ((paraFrom == paraTo) && (indexFrom >= indexTo)))
        return res;
    for (int i = paraFrom; i <= paraTo; i++){
        res += unquoteString(text(i), (i == paraFrom) ? indexFrom : 0, (i == paraTo) ? indexTo : -1);
        if ((i < paraTo) && (i < paragraphs()))
            res += "\n";
    }
    return res;
}

void TextShow::copy()
{
    QApplication::clipboard()->setText(selectedText());
}

void TextShow::startDrag()
{
    QDragObject *drag = new QTextDrag(selectedText(), viewport());
    if ( isReadOnly() ) {
        drag->dragCopy();
    } else {
        if ( drag->drag() && QDragObject::target() != this && QDragObject::target() != viewport() )
            removeSelectedText();
    }
}

void TextShow::encodingChanged(unsigned long _uin)
{
    if (_uin && (m_nUin != _uin)) return;
    QTextCodec *newCodec = pClient->codecForUser(m_nUin);
    string s = SIMClient::to8Bit(codec, text());
    codec = newCodec;
    setText(SIMClient::from8Bit(codec, s));
    scrollToBottom();
}

MsgView::MsgView(QWidget *p)
        : TextShow(p)
{
    bBack = false;
    connect(pClient, SIGNAL(messageRead(ICQMessage*)), this, SLOT(messageRead(ICQMessage*)));
    connect(pMain, SIGNAL(colorsChanged()), this, SLOT(colorsChanged()));
    connect(pMain, SIGNAL(ownColorsChanged()), this, SLOT(ownColorsChanged()));
    oldSendColor = pMain->getColorSend();
    oldReceiveColor = pMain->getColorReceive();
}

void MsgView::setUin(unsigned long uin)
{
    bool bAdd = (m_nUin == 0);
    TextShow::setUin(uin);
    if (bAdd) addUnread(uin);
}

static char FONT_FORMAT[] = "<font color=\"#%06X\">";

void MsgView::colorsChanged()
{
    int x = contentsX();
    int y = contentsY();
    char FONT_SEND[] = "<##FontSend##>";
    char FONT_RECEIVE[] = "<##FontReceive##>";
    QString t = text();
    QString c;
    c.sprintf(FONT_FORMAT, oldSendColor);
    t.replace(QRegExp(c), FONT_SEND);
    c.sprintf(FONT_FORMAT, oldReceiveColor);
    t.replace(QRegExp(c), FONT_RECEIVE);
    c.sprintf(FONT_FORMAT, pMain->getColorSend());
    t.replace(QRegExp(FONT_SEND), c);
    c.sprintf(FONT_FORMAT, pMain->getColorReceive());
    t.replace(QRegExp(FONT_RECEIVE), c);
    setText(t);
    setContentsPos(x, y);
}

void MsgView::messageRead(ICQMessage *msg)
{
    QString pat;
    pat.sprintf("<a href=\"msg://%lu.%lu", msg->getUin(), msg->Id);
    for (int i = 0; i < paragraphs();){
        if (text(i).find(pat) < 0){
            for (i++; i < paragraphs(); i++)
                if (text(i).find("<a href=\"msg://") >= 0) break;
            continue;
        }
        int paraFrom, indexFrom;
        int paraTo, indexTo;
        getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
        setSelection(i, 0, i, 0xFFFF);
        setBold(false);
        if ((paraFrom == -1) && (paraTo == -1)){
            removeSelection();
            scrollToBottom();
        }else{
            setSelection(paraFrom, indexFrom, paraTo, indexTo);
        }
        break;
    }
}

void MsgView::deleteUser(unsigned long uin)
{
    QString pat;
    pat.sprintf("<a href=\"msg://%lu.", uin);
    for (int i = 0; i < paragraphs();){
        if (text(i).find(pat) < 0){
            for (i++; i < paragraphs(); i++)
                if (text(i).find("<a href=\"msg://") >= 0) break;
            continue;
        }
        removeParagraph(i);
        for (; i < paragraphs(); ){
            if (text(i).find("<a href=\"msg://") >= 0) break;
            int n = paragraphs();
            removeParagraph(i);
            if (n == paragraphs()){
                i++;
                break;
            }
        }
    }
}

void MsgView::setMessage(unsigned long uin, unsigned long msgId)
{
    QString pat;
    pat.sprintf("<p><a name=\"%lu.%lu\"></a>", uin, msgId);
    if (text().find(pat) < 0) return;
    curAnchor = QString::number(uin) + "." + QString::number(msgId);
    scrollToAnchor(curAnchor);
}

QString MsgView::makeMessage(ICQMessage *msg, bool bUnread)
{
    const char *icon;
    if (msg->Type() == ICQ_MSGxSTATUS){
        ICQStatus *m = static_cast<ICQStatus*>(msg);
        icon = SIMClient::getStatusIcon(m->status);
    }else{
        icon = SIMClient::getMessageIcon(msg->Type());
    }
    QString s;
    s.sprintf("<p><nobr><a name=\"%lu.%lu\"></a>"
              "<a href=\"msg://%lu.%lu\"><img src=\"icon:%s\"></a>&nbsp;",
              msg->getUin(), msg->Id, msg->getUin(), msg->Id, icon);
    if (bUnread) s += "<b>";
    QString color;
    color.sprintf(FONT_FORMAT, msg->Received ? pMain->getColorReceive() : pMain->getColorSend());
    s += color;
    if (msg->Received){
        CUser u(msg->getUin());
        s += u.name(true);
    }else{
        CUser u(pClient->owner);
        s += u.name(true);
    }
    s += "</font>&nbsp;&nbsp;";
    if (msg->Type() == ICQ_MSGxSTATUS){
        ICQStatus *m = static_cast<ICQStatus*>(msg);
        s += SIMClient::getStatusText(m->status);
        s += "&nbsp;&nbsp;";
    }
    QDateTime time;
    time.setTime_t(msg->Time);
    s += "<font size=-1>";
#ifdef USE_KDE
    s += KGlobal::locale()->formatDateTime(time);
#else
    s += time.toString();
#endif
    s += "</font>";
    if (bUnread) s += "</b>";
    s += "</nobr></p>";
    unsigned long foreColor = 0;
    unsigned long backColor = 0;
    if (!pMain->isUseOwnColors() && (msg->Type() == ICQ_MSGxMSG)){
        ICQMsg *m = static_cast<ICQMsg*>(msg);
        foreColor = m->ForeColor;
        backColor = m->BackColor;
    }
    if (msg->Type() != ICQ_MSGxSTATUS){
        s += "<p>";
        if (foreColor != backColor){
            QString fg;
            fg.sprintf("<font color=#%06lX>", foreColor);
            s += fg;
        }
        s += makeMessageText(msg, pMain->isUseOwnColors());
        if (foreColor != backColor) s += "</font>";
        s += "</p>";
    }
    return s;
}

void MsgView::addMessage(ICQMessage *msg, bool bUnread, bool bSet)
{
    if (msg->Id >= MSG_PROCESS_ID) return;
    int startP = 0;
    int endP = 0;
    int startI = 0;
    int endI = 0;
    getSelection(&startP, &startI, &endP, &endI);
    int x = contentsX();
    int y = contentsY();
    QString s(makeMessage(msg, bUnread));
    if (bSet) curAnchor = QString::number(msg->getUin()) + "." + QString::number(msg->Id);
    unsigned long foreColor = 0;
    unsigned long backColor = 0;
    if (!pMain->isUseOwnColors() && (msg->Type() == ICQ_MSGxMSG)){
        ICQMsg *m = static_cast<ICQMsg*>(msg);
        foreColor = m->ForeColor;
        backColor = m->BackColor;
    }
    if (bBack){
        setText(s + text(), curAnchor);
        if (foreColor != backColor)
            setMsgBgColor(msg->getUin(), msg->Id, backColor, 0);
    }else{
        int n = paragraphs();
        if (n > 0) n--;
        append(s);
        clearParagraphBackground(n+1);
        if (foreColor != backColor)
            setMsgBgColor(msg->getUin(), msg->Id, backColor, n);
    }
    if (bSet){
        scrollToBottom();
    }else{
        setContentsPos(x, y);
    }
    sync();
    int startP1 = 0;
    int endP1 = 0;
    int startI1 = 0;
    int endI1 = 0;
    getSelection(&startP1, &startI1, &endP1, &endI1);
    if ((startP != startP1) || (endP != endP1) || (startI != startI1) || (endI != endI1))
        setSelection(startP, startI, endP, endI);
}

int MsgView::setMsgBgColor(unsigned long uin, unsigned long id, unsigned long rgb, int start)
{
    QString pat;
    pat.sprintf("<a href=\"msg://%lu.%lu", uin, id);
    for (int n = start; n < paragraphs(); n++){
        QString t = text(n);
        if (text(n).find(pat) < 0) continue;
        pat = "<a href=\"msg://";
        for (n++; n < paragraphs(); n++){
            if (text(n).isEmpty()) break;
            if (text(n).find(pat) >= 0) break;
            setParagraphBackgroundColor(n, QColor(rgb));
        }
        return n;
    }
    log(L_WARN, "Message bg color not found");
    return paragraphs();
}

void MsgView::addUnread(unsigned long uin)
{
    ICQUser *u = pClient->getUser(uin);
    if (u == NULL) return;
    History h(uin);
    for (list<unsigned long>::iterator it = u->unreadMsgs.begin(); it != u->unreadMsgs.end(); it++){
        ICQMessage *msg = h.getMessage(*it);
        if (msg == NULL) continue;
        addMessage(msg, true, false);
    }
}

void MsgView::ownColorsChanged()
{
    setText("");
}

unsigned long MsgView::msgId(int parag)
{
    QString pat = "<a href=\"msg://";
    for (; parag >= 0; parag--){
        QString t = text(parag);
        int pos = t.find(pat);
        if (pos >= 0){
            t = t.mid(pos + pat.length());
            QCString s = t.latin1();
            unsigned i;
            for (i = 0; i < s.length(); i++)
                if (s[(int)i] == '.') break;
            return atol((const char*)s + i + 1);
        }
    }
    return 0;
}

int MsgView::findMsg(unsigned long id, int parag)
{
    QString pat = "<a href=\"msg://";
    for (; parag < paragraphs(); parag++){
        QString t = text(parag);
        int pos = t.find(pat);
        if (pos >= 0){
            t = t.mid(pos + pat.length());
            QCString s = t.latin1();
            unsigned i;
            for (i = 0; i < s.length(); i++)
                if (s[(int)i] == '.') break;
            if ((unsigned long)(atol((const char*)s + i + 1)) == id)
                return parag;
        }
    }
    return -1;
}

MsgViewConv::MsgViewConv(QWidget *p)
        : MsgView(p)
{
}

void MsgViewConv::setUin(unsigned long uin)
{
    TextShow::setUin(uin);
    setText("");
    if (pMain->getCopyMessages()){
        ICQUser *u = pClient->getUser(m_nUin);
        if (u == NULL) return;
        unsigned long unreadId = 0xFFFFFFFF;
        list<unsigned long>::iterator unreadIt;
        for (unreadIt = u->unreadMsgs.begin(); unreadIt != u->unreadMsgs.end(); unreadIt++){
            unsigned long id = *unreadIt;
            if ((id < MSG_PROCESS_ID) && (id < unreadId)) unreadId = id;
        }

        list<unsigned long> unreadMsg;
        History h(uin);
        History::iterator &it = h.messages();
        for (unsigned n = 0; ++it; n++){
            unsigned long id = (*it)->Id;
            if ((n >= pMain->getCopyMessages()) && (id < unreadId))
                break;
            unreadMsg.push_front(id);
        }
        for (list<unsigned long>::iterator itMsg = unreadMsg.begin(); itMsg != unreadMsg.end(); ++itMsg){
            ICQMessage *msg = h.getMessage(*itMsg);
            if (msg == NULL) continue;
            for (unreadIt = u->unreadMsgs.begin(); unreadIt != u->unreadMsgs.end(); unreadIt++)
                if ((*unreadIt) == (*itMsg)) break;
            addMessage(msg, unreadIt != u->unreadMsgs.end(), true);
        }
        bBack = false;
    }else if (uin){
        addUnread(uin);
    }
    scrollToBottom();
}

HistoryTextView::HistoryTextView(QWidget *p, unsigned long uin)
        : MsgView(p)
{
    setUin(uin);
    bFill = false;
    bBack = true;
    findId = 0;
    h = NULL;
}

HistoryTextView::~HistoryTextView()
{
    if (h) delete h;
}

void HistoryTextView::fill(unsigned long offs, const QString &filter, unsigned long _findId, bool bDirection)
{
    if (bFill){
        bFill = false;
        if (h){
            delete h;
            h = NULL;
        }
    }
    setText("");
    bFill = true;
    u = pClient->getUser(m_nUin);
    if (u == NULL) return;
    h = new History(m_nUin);
    History::iterator &it = h->messages();
    it.setOffs(offs);
    it.setFilter(filter);
    it.setDirection(bDirection);
    showProgress(0);
    nMsg = 0;
    findId = _findId;
    msgs.clear();
    fill();
}

void HistoryTextView::fill()
{
    if ((u == NULL) || (h == NULL)) return;
    History::iterator &it = h->messages();
    list<unsigned long>::iterator unreadIt;
    if ((findId == 0) && msgs.size()){
        unsigned long id = msgs.front();
        ICQMessage *msg = h->getMessage(id);
        if (msg){
            for (unreadIt = u->unreadMsgs.begin(); unreadIt != u->unreadMsgs.end(); unreadIt++)
                if ((*unreadIt) == id) break;
            bBack = false;
            addMessage(msg, unreadIt != u->unreadMsgs.end(), false);
            bBack = true;
            delete msg;
        }
        msgs.remove(id);
        if (msgs.size() == 0)
            emit findDone(id);
        if (msgs.size() || (nMsg < MAX_HISTORY))
            QTimer::singleShot(0, this, SLOT(fill()));
        return;
    }

    if (++it){
        if (findId){
            unsigned long msgId = (*it)->Id;
            msgs.push_back(msgId);
            if (msgId == findId)
                findId = 0;
        }else{
            for (unreadIt = u->unreadMsgs.begin(); unreadIt != u->unreadMsgs.end(); unreadIt++)
                if ((*unreadIt) == (*it)->Id) break;
            bBack = false;
            addMessage(*it, unreadIt != u->unreadMsgs.end(), false);
            bBack = true;
        }
        nMsg++;
        showProgress(nMsg);
        if ((nMsg < MAX_HISTORY) || ((findId == 0) && msgs.size())){
            QTimer::singleShot(0, this, SLOT(fill()));
            return;
        }
        emit fillDone((*it)->Id);
        if (findId) return;
    }

    delete h;
    h = NULL;
    u = NULL;
    showProgress(101);
}

void HistoryTextView::ownColorsChanged()
{
    MsgView::ownColorsChanged();
    bFill = false;
    if (h){
        delete h;
        h = NULL;
    }
    QTimer::singleShot(100, this, SLOT(fill()));
}

const int cmbSearch		= 1;
const int btnSearch		= 2;
const int btnFilter		= 3;
const int btnPrev		= 4;
const int btnNext		= 5;
const int btnDirection	= 6;
const int btnSave		= 7;
const int btnDelete		= 8;

ToolBarDef historyToolBar[] =
    {
        { btnSave, "save_all", NULL, I18N_NOOP("Save"), 0, SLOT(slotSave()), NULL },
        //		{ btnDelete, "remove", NULL, I18N_NOOP("&Delete"), 0, SLOT(slotDelete()), NULL },
        { cmbSearch, "find", NULL, I18N_NOOP("Search"), BTN_COMBO, SLOT(searchTextChanged(const QString&)), NULL },
        { btnSearch, "find", NULL, I18N_NOOP("&Search"), 0, SLOT(slotSearch()), NULL },
        { btnFilter, "filter", NULL, I18N_NOOP("&Filter"), BTN_TOGGLE, SLOT(slotFilter(bool)), NULL },
        SEPARATOR,
        { btnPrev, "1leftarrow", NULL, I18N_NOOP("&Previous page"), 0, SLOT(prevPage()), NULL },
        { btnNext, "1rightarrow", NULL, I18N_NOOP("&Next page"), 0, SLOT(nextPage()), NULL },
        SEPARATOR,
        { btnDirection, "1uparrow", "1downarrow", I18N_NOOP("&Sort order"), BTN_TOGGLE, SLOT(slotDirection(bool)), NULL },
        END_DEF,
        END_DEF
    };

const ToolBarDef *pHistoryToolBar = historyToolBar;

HistoryView::HistoryView(QWidget *p, unsigned long uin)
        : QMainWindow(p, "historyview", 0)
{
    connect(pClient, SIGNAL(messageReceived(ICQMessage*)), this, SLOT(messageReceived(ICQMessage*)));
    connect(pClient, SIGNAL(event(ICQEvent*)), this, SLOT(processEvent(ICQEvent*)));
    connect(pMain, SIGNAL(searchChanged()), this, SLOT(searchChanged()));
    view = new HistoryTextView(this, uin);
    connect(view, SIGNAL(showProgress(int)), this, SLOT(slotShowProgress(int)));
    connect(view, SIGNAL(goMessage(unsigned long, unsigned long)), this, SLOT(slotGoMessage(unsigned long, unsigned long)));
    connect(view, SIGNAL(fillDone(unsigned long)), this, SLOT(fillDone(unsigned long)));
    connect(view, SIGNAL(findDone(unsigned long)), this, SLOT(findDone(unsigned long)));
    setCentralWidget(view);
    toolbar = new CToolBar(historyToolBar, pMain->_ToolBarHistory(), this, this);
    toolbar->setOn(btnDirection, pMain->isHistoryDirection());
    searchTextChanged("");
    setDockEnabled(toolbar, Left, false);
    setDockEnabled(toolbar, Right, false);
    searchParag = 0;
    searchIndex = 0;
    searchChanged();
    QAccel *accel = new QAccel(this);
#ifdef USE_KDE
    accel->connectItem(accel->insertItem(KStdAccel::find()), this, SLOT(slotSearch(int)));
    accel->connectItem(accel->insertItem(KStdAccel::findNext()), this, SLOT(slotSearch(int)));
#else
    accel->connectItem(accel->insertItem(Key_F3), this, SLOT(slotSearch(int)));
    accel->connectItem(accel->insertItem(CTRL + Key_F), this, SLOT(slotSearch(int)));
#endif
    QTimer::singleShot(100, this, SLOT(fill()));
}

#ifdef USE_KDE
#undef QLineEdit
#endif

void HistoryView::fill()
{
    toolbar->setEnabled(btnPrev, false);
    toolbar->setEnabled(btnNext, false);
    viewFill(toolbar->isOn(btnDirection) ? (unsigned long)(-1) : 0, 0);
}

void HistoryView::viewFill(unsigned long offs, unsigned long findId)
{
    view->fill(offs, filter, findId, toolbar->isOn(btnDirection));
}

void HistoryView::fillDone(unsigned long offs)
{
    if (offs){
        toolbar->setEnabled(btnNext, true);
        pages.push(offs);
    }
    if (view->findId) nextPage(view->findId);
}

void HistoryView::searchChanged()
{
    int safeParag = searchParag;
    int safeIndex = searchIndex;
    QComboBox *cmb = static_cast<QComboBox*>(toolbar->getWidget(cmbSearch));
    QLineEdit *edt = cmb->lineEdit();
    QString searchText = edt->text();
    if (searchText.length() == 0) return;
#if QT_VERSION >= 300
    int startPos, endPos;
    edt->getSelection(&startPos, &endPos);
#endif
    int pos = edt->cursorPosition();
    for (int n = cmb->count() - 1; n >= 0; n--){
        cmb->removeItem(n);
    }
    cmb->insertStringList(pMain->searches);
    edt->setText(searchText);
#if QT_VERSION >= 300
    if ((startPos < endPos) && (startPos >= 0) && (endPos >= 0))
        edt->setSelection(startPos, endPos);
#endif
    edt->setCursorPosition(pos);
    searchParag = safeParag;
    searchIndex = safeIndex;
}

void HistoryView::slotSearch(int)
{
    slotSearch();
}

void HistoryView::findDone(unsigned long id)
{
    QComboBox *cmb = static_cast<QComboBox*>(toolbar->getWidget(cmbSearch));
    QString searchText = cmb->lineEdit()->text();
    if (searchText.isEmpty()) return;
    searchParag = view->findMsg(id, searchParag);
    searchIndex = 0;
    if (searchParag == -1) return;
    if (!view->find(searchText, true, false, true, &searchParag, &searchIndex))
        return;
    view->setSelection(searchParag, searchIndex, searchParag, searchIndex + searchText.length(), 1);
    searchIndex += searchText.length();
    view->setCursorPosition(searchParag, searchIndex);
    view->ensureCursorVisible();
}

void HistoryView::slotSearch()
{
    QComboBox *cmb = static_cast<QComboBox*>(toolbar->getWidget(cmbSearch));
    QString searchText = cmb->lineEdit()->text();
    if (searchText.isEmpty()) return;
    pMain->addSearch(searchText);
    unsigned long curId = view->msgId(searchParag);
    int nSearchParag = searchParag;
    int nSearchIndex = searchIndex;
    if (view->find(searchText, true, false, true, &nSearchParag, &nSearchIndex)){
        unsigned long newId = view->msgId(nSearchParag);
        if (newId == curId){
            searchParag = nSearchParag;
            searchIndex = nSearchIndex;
            view->removeSelection();
            view->setSelection(searchParag, searchIndex, searchParag, searchIndex + searchText.length(), 1);
            searchIndex += searchText.length();
            view->setCursorPosition(searchParag, searchIndex);
            view->ensureCursorVisible();
            return;
        }
    }
    History h(view->Uin());
    History::iterator &it = h.messages();
    it.setOffs(curId-1);
    it.setFilter(filter);
    it.setCondition(searchText);
    it.setDirection(toolbar->isOn(btnDirection));
    if (++it){
        searchParag = view->findMsg((*it)->Id, searchParag);
        searchIndex = 0;
        if (searchParag >= 0){
            if (view->find(searchText, true, false, true, &searchParag, &searchIndex)){
                view->removeSelection();
                view->setSelection(searchParag, searchIndex, searchParag, searchIndex + searchText.length(), 1);
                searchIndex += searchText.length();
                view->setCursorPosition(searchParag, searchIndex);
                view->ensureCursorVisible();
            }
            return;
        }
        nextPage((*it)->Id);
        return;
    }
    QApplication::beep();
}

void HistoryView::slotDirection(bool dir)
{
    pMain->setHistoryDirection(dir);
    while (pages.size())
        pages.pop();
    fill();
}

void HistoryView::slotFilter(bool bSet)
{
    searchParag = 0;
    searchIndex = 0;
    QComboBox *cmb = static_cast<QComboBox*>(toolbar->getWidget(cmbSearch));
    filter = bSet ? cmb->lineEdit()->text() : QString("");
    while (pages.size())
        pages.pop();
    fill();
}

void HistoryView::slotShowProgress(int n)
{
    emit showProgress(n);
}

void HistoryView::slotGoMessage(unsigned long uin, unsigned long msgId)
{
    emit goMessage(uin, msgId);
}

void HistoryView::searchTextChanged(const QString &searchText)
{
    toolbar->setEnabled(btnSearch, !searchText.isEmpty());
    toolbar->setEnabled(btnFilter, !searchText.isEmpty());
    searchParag = 0;
    searchIndex = 0;
}

void HistoryView::prevPage()
{
    if (toolbar->isEnabled(btnNext)) pages.pop();
    unsigned long offs = toolbar->isOn(btnDirection) ? (unsigned long)(-1) : 0;
    if (pages.size()) pages.pop();
    if (pages.size()) offs = pages.top();
    toolbar->setEnabled(btnPrev, pages.size() > 0);
    toolbar->setEnabled(btnNext, false);
    viewFill(offs, 0);
}

void HistoryView::nextPage(unsigned long findId)
{
    if (pages.size() == 0) return;
    toolbar->setEnabled(btnPrev, true);
    toolbar->setEnabled(btnNext, false);
    viewFill(pages.top(), findId);
}

void HistoryView::nextPage()
{
    nextPage(0);
}

void HistoryView::processEvent(ICQEvent *e)
{
    if (e->type() != EVENT_DONE) return;
    ICQMessage *msg = e->message();
    if (msg == NULL) return;
    messageReceived(msg);
}

void HistoryView::messageReceived(ICQMessage *msg)
{
    if (msg->getUin() != view->Uin()) return;
    if (msg->Id >= MSG_PROCESS_ID) return;
    toolbar->setEnabled(btnPrev, true);
}

void HistoryView::slotSave()
{
    string name = pMain->getFullPath("");
    fileName = QString::fromLocal8Bit(name.c_str());
    fileName += QString::number(view->Uin()) + ".txt";
    fileName = QFileDialog::getSaveFileName(fileName,
#ifdef USE_KDE
                                            i18n("text/plain|Text file\ntext/html|HTML file"),
#else
                                            i18n("Text file(*.txt);;HTML file(*.htm *.html)"),
#endif
                                            this, i18n("Save history"));
    if (fileName.isEmpty()) return;
    saveMode = TXT;
    int r = fileName.findRev('.');
    if (r >= 0){
        QString ext = fileName.mid(r + 1);
        ext = ext.lower();
        if ((ext == "htm") || (ext == "html"))
            saveMode = HTML;
    }
    QFile f(fileName);
    if (f.exists()){
        QStringList btns;
        btns.append(i18n("&Yes"));
        btns.append(i18n("&No"));
        BalloonMsg *msg = new BalloonMsg(i18n("File %1 exists\nOverwrite?") .arg(fileName),
                                         btns, toolbar->getWidget(btnSave));
        connect(msg, SIGNAL(action(int)), this, SLOT(saveToFile(int)));
        msg->show();
        return;
    }
    saveToFile(0);
}

void HistoryView::saveToFile(int n)
{
    if (n){
        fileName = "";
        return;
    }
    if (fileName.isEmpty())
        return;
    QFile f(fileName);
    if (!f.open(IO_WriteOnly | IO_Truncate | IO_Translate)){
        BalloonMsg::message(i18n("Can't create %1") .arg(fileName), toolbar->getWidget(btnSave));
        fileName = "";
        return;
    }
    showProgress(0);
    if (saveMode == HTML){
        QTextCodec *codec = pClient->codecForUser(0);
        QString charset = codec->name();
        CUser u(view->Uin());
        QString header = QString(
                             "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
                             "<HTML>\n"
                             "<HEADER>\n"
                             "<TITLE>History %1</TITLE>\n"
                             "<META http-equiv=Content-Type content=\"text/html; charset=%2\">\n"
                             "</HEADER>\n"
                             "<BODY>\n")
                         .arg(u.name(true))
                         .arg(codec->name());
        QCString ss = header.local8Bit();
        f.writeBlock(ss, ss.length());
    }
    History h(view->Uin());
    History::iterator &it = h.messages();
    it.setDirection(true);
    QTextCodec *codec = pClient->codecForUser(view->Uin());
    for (;++it;){
        ICQMessage *msg = *it;
        const char *encoding = msg->Charset.c_str();
        if (*encoding == 0)
            encoding = codec->name();
        string msg_text;
        QString s;
        CUser u(msg->Received ? msg->getUin() : pClient->owner->Uin);
        s = u.name();
        s += " ";
        QDateTime time;
        time.setTime_t(msg->Time);
#ifdef USE_KDE
        s += KGlobal::locale()->formatDateTime(time);
#else
        s += time.toString();
#endif
        s += " ";
        switch (msg->Type()){
        case ICQ_MSGxMSG:
            msg_text = (static_cast<ICQMsg*>(msg))->Message.c_str();
            SIMClient::toUTF(msg_text, encoding);
            s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
            break;
        case ICQ_MSGxURL:{
                ICQUrl *url = static_cast<ICQUrl*>(msg);
                s += i18n("URL:");
                s += " ";
                s += url->URL.c_str();
                if (*url->Message.c_str()){
                    msg_text = url->Message.c_str();
                    SIMClient::toUTF(msg_text, encoding);
                    s += "\n";
                    s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                }
                break;
            }
        case ICQ_MSGxAUTHxREQUEST:{
                ICQAuthRequest *req = static_cast<ICQAuthRequest*>(msg);
                s = i18n("Authorization request");
                if (*req->Message.c_str()){
                    msg_text = req->Message.c_str();
                    SIMClient::toUTF(msg_text, encoding);
                    s += ": ";
                    s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                }
                break;
            }
        case ICQ_MSGxAUTHxREFUSED:{
                ICQAuthRefused *req = static_cast<ICQAuthRefused*>(msg);
                s += i18n("Authorization refused");
                if (*req->Message.c_str()){
                    msg_text = req->Message.c_str();
                    SIMClient::toUTF(msg_text, encoding);
                    s += ": ";
                    s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                }
                break;
            }
        case ICQ_MSGxAUTHxGRANTED:
            s += i18n("Authorization granted");
            break;
        case ICQ_MSGxADDEDxTOxLIST:
            s += i18n("Added to contact list");
            break;
        case ICQ_MSGxCONTACTxREQUEST:{
                ICQContactRequest *req = static_cast<ICQContactRequest*>(msg);
                s += i18n("Contact request");
                if (*req->Message.c_str()){
                    msg_text = req->Message.c_str();
                    SIMClient::toUTF(msg_text, encoding);
                    s += ": ";
                    s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                }
                break;
            }
        case ICQ_MSGxFILE:{
                ICQFile *file = static_cast<ICQFile*>(msg);
                QString name = QString::fromLocal8Bit(file->Name.c_str());
                if (name.find(QRegExp("^[0-9]+ Files$")) >= 0){
                    s += i18n("File", "%n files", atol(name.latin1()));
                }else{
                    s += i18n("File");
                    s += ": ";
                    s += name;
                }
                s += " (";
                s += QString::number(file->Size);
                s += " ";
                s += i18n("bytes");
                s += ")";
                msg_text = file->Description.c_str();
                if (*msg_text.c_str()){
                    SIMClient::toUTF(msg_text, encoding);
                    s += "\n";
                    s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                }
                break;
            }
        case ICQ_MSGxCHAT:{
                ICQChat *chat = static_cast<ICQChat*>(msg);
                msg_text = chat->Reason.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                break;
            }
        case ICQ_MSGxCONTACTxLIST:{
                ICQContacts *contacts = static_cast<ICQContacts*>(msg);
                s += msg->Received ? i18n("Contact list received") : i18n("Contact list sent");
                s += "\n";
                for (ContactList::iterator it = contacts->Contacts.begin(); it != contacts->Contacts.end(); it++){
                    Contact *contact = static_cast<Contact*>(*it);
                    s += QString::number(contact->Uin) + "\t";
                    s += QString::fromLocal8Bit(contact->Alias.c_str());
                    s += "\n";
                }
                break;
            }
        case ICQ_MSGxSMS:{
                ICQSMS *sms = static_cast<ICQSMS*>(msg);
                msg_text = sms->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                if (*sms->Phone.c_str()){
                    s += "\n";
                    s += QString::fromLocal8Bit(sms->Phone.c_str());
                    if (*sms->Network.c_str())
                        s += " (" + QString::fromLocal8Bit(sms->Network.c_str()) + ")";
                }
                break;
            }
        case ICQ_MSGxWEBxPANEL:{
                ICQWebPanel *m = static_cast<ICQWebPanel*>(msg);
                msg_text = m->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                break;
            }
        case ICQ_MSGxEMAILxPAGER:{
                ICQEmailPager *m = static_cast<ICQEmailPager*>(msg);
                msg_text = m->Message.c_str();
                SIMClient::toUTF(msg_text, encoding);
                s += QString::fromUtf8(pClient->clearHTML(msg_text).c_str());
                break;
            }
        default:
            log(L_WARN, "Unknown message type %u", msg->Type());
            s += "???";
        }
        if (s.isEmpty()) continue;
        if (s[(int)(s.length()-1)] != '\n') s += "\n";
        QCString ss = s.local8Bit();
        f.writeBlock(ss, ss.length());
        showProgress(it.progress());
    }
    if (saveMode == HTML){
        QString footer =
            "</BODY>\n"
            "</HTML>";
        QCString ss = footer.local8Bit();
        f.writeBlock(ss, ss.length());
    }
    showProgress(101);
}

void HistoryView::slotDelete()
{
}

#ifndef _WINDOWS
#include "msgview.moc"
#endif

/***************************************************************************
                          textshow.cpp  -  description
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

#include "textshow.h"
#include "html.h"
#include "simapi.h"

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
#include <QFileDialog>
#include <QByteArray>
#include <QGridLayout>
#include <QKeyEvent>
#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QContextMenuEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#endif

#ifdef USE_KDE
#include <kcolordialog.h>
#include <kfontdialog.h>
#else
#include <QColorDialog>
#include <QFontDialog>
#endif

#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QRegExp>
#include <QObject>
#include <QList>
#include <QTimer>
#include <QStringList>
#include <QTextCodec>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QToolButton>
#include <QStatusBar>
#include <QToolTip>
#include <QLayout>
#include <QTextCursor>

#define MAX_HISTORY	100

class RichTextDrag : public QMimeData
{
public:
    RichTextDrag(QWidget *dragSource = 0);

    void setRichText(const QString &txt);

    virtual QByteArray encodedData(const char *mime) const;
    virtual const char* format(int i) const;

    static bool decode(QMimeData *e, QString &str, const QByteArray &mimetype, const QByteArray &subtype);
    static bool canDecode( QMimeData* e );
private:
    QString richTxt;
};

RichTextDrag::RichTextDrag( QWidget *dragSource)
        : QMimeData()
{
}

QByteArray RichTextDrag::encodedData( const char *mime ) const
{
    if ( qstrcmp( "application/x-qrichtext", mime ) == 0 ) {
        return richTxt.toUtf8(); // #### perhaps we should use USC2 instead?
    } else
        return QMimeData::data( mime );
}

bool RichTextDrag::decode(QMimeData *e, QString &str, const QByteArray &mimetype, const QByteArray &subtype)
{
    if (mimetype == "application/x-qrichtext"){
        QString mime;
        QStringList formats = e->formats();
	int i;
        for ( i = 0; (mime = formats.value(i)) != formats.last(); ++i ) {
            if (qstrcmp("application/x-qrichtext", mime.toLatin1()) != 0)
                continue;
            str = QString::fromUtf8(e->data(mime));
            return TRUE;
        }
        return FALSE;
    }
    QString subt = subtype;
    return FALSE;
}

bool RichTextDrag::canDecode(QMimeData* e)
{
    if ( e->hasFormat("application/x-qrichtext"))
        return TRUE;
    return e->hasText();
}

const char* RichTextDrag::format(int i) const
{
    return "application/x-qrichtext";
}

void RichTextDrag::setRichText(const QString &txt)
{
    richTxt = txt;
    setText(unquoteText(txt));
}

TextEdit::TextEdit(QWidget *p, const char *name)
        : TextShow(p, name)
{
    m_param = NULL;
    m_bEmpty = true;
    m_bBold  = false;
    m_bItalic = false;
    m_bUnderline = false;
    m_bSelected  = false;
    m_bNoSelected = false;
    m_bInClick = false;
    m_bChanged = false;
    setReadOnly(false);
    curFG = palette().color(QPalette::Text);
    m_bCtrlMode = true;
    setWordWrapMode(QTextOption::WordWrap);
    setAutoFormatting(0);
    connect(this, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(fontChanged(const QFont&)));
    connect(this, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(slotColorChanged(const QColor&)));
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
    connect(this, SIGNAL(clicked(int,int)), this, SLOT(slotClicked(int,int)));
    viewport()->installEventFilter(this);
    fontChanged(font());
}

TextEdit::~TextEdit()
{
    emit finished(this);
}

void TextEdit::setFont(const QFont &f)
{
    TextShow::setFont(f);
    m_bNoSelected = true;
    fontChanged(f);
    m_bNoSelected = false;
    m_bSelected   = false;
}

void TextEdit::slotTextChanged()
{
    bool bEmpty = isEmpty();
    if (m_bEmpty == bEmpty)
        return;
    m_bEmpty = bEmpty;
    emit emptyChanged(m_bEmpty);
}

void TextEdit::slotClicked(int,int)
{
    int paraFrom, paraTo, indexFrom, indexTo;
    m_bInClick = true;
    QContextMenuEvent e(QContextMenuEvent::Other, QPoint(0, 0), QPoint(0, 0));
    contextMenuEvent(&e);
    m_bInClick = false;
}

QMenu *TextEdit::createPopupMenu(const QPoint& pos)
{
    if (m_bInClick)
        return NULL;
    m_popupPos = pos;
    return TextShow::createStandardContextMenu();
}

bool TextEdit::isEmpty()
{
    QString t = toPlainText();
    return t.isEmpty() || (t == " ");
}

void TextEdit::setParam(void *param)
{
    m_param = param;
}

void TextEdit::slotColorChanged(const QColor &c)
{
    if (c == curFG)
        return;
    this->textCursor().select(QTextCursor::WordUnderCursor);
    QString txt = this->textCursor().selectedText();
    if (txt.isEmpty()){
        setTextColor(curFG);
        return;
    }
    if (c != curFG)
        setForeground(c, false);
}

void TextEdit::bgColorChanged(QColor c)
{
    setBackground(c);
    emit colorsChanged();
}

void TextEdit::fgColorChanged(QColor c)
{
    setForeground(c, true);
    curFG = c;
    emit colorsChanged();
}

bool TextEdit::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::FocusOut){
        emit lostFocus();
    }
    return QTextEdit::eventFilter(o, e);
}

void TextEdit::changeText()
{
    emit textChanged();
}

void TextEdit::fontChanged(const QFont &f)
{
    if (m_bSelected){
        if (!m_bNoSelected)
            emit fontSelected(f);
        m_bSelected = false;
    }
    if (m_param == NULL)
        return;
    m_bChanged = true;
    if (f.bold() != m_bBold){
        m_bBold = f.bold();
        Command cmd;
        cmd->id    = CmdBold;
        cmd->flags = m_bBold ? COMMAND_CHECKED : 0;
        cmd->param = m_param;
        Event e(EventCommandChecked, cmd);
        e.process();
    }
    if (f.italic() != m_bItalic){
        m_bItalic = f.italic();
        Command cmd;
        cmd->id    = CmdItalic;
        cmd->flags = m_bItalic ? COMMAND_CHECKED : 0;
        cmd->param = m_param;
        Event e(EventCommandChecked, cmd);
        e.process();
    }
    if (f.underline() != m_bUnderline){
        m_bUnderline = f.underline();
        Command cmd;
        cmd->id    = CmdUnderline;
        cmd->flags = m_bUnderline ? COMMAND_CHECKED : 0;
        cmd->param = m_param;
        Event e(EventCommandChecked, cmd);
        e.process();
    }
    m_bChanged = false;
}

void TextEdit::setCtrlMode(bool mode)
{
    m_bCtrlMode = mode;
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)))
    {
        //   in m_bCtrlMode:    enter      --> newLine
        //                      ctrl+enter --> sendMsg
        //   in !m_bCtrlMode:   enter      --> sendMsg
        //                      ctrl+enter --> newLine
        // the (bool) is required due to the bitmap
        if (m_bCtrlMode == (bool)(e->modifiers() & Qt::ControlModifier)){
            emit ctrlEnterPressed();
            return;
        }
    }
    if (!isReadOnly()){
        if ((e->modifiers() == Qt::ShiftModifier) && (e->key() == Qt::Key_Insert)){
            paste();
            return;
        }
        if ((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_Delete)){
            cut();
            return;
        }
    }
    TextShow::keyPressEvent(e);
}

void *TextEdit::processEvent(Event *e)
{
    if (m_param == NULL)
        return NULL;
    if (e->type() == EventCheckState){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->param != m_param)
            return NULL;
        switch (cmd->id){
        case CmdBgColor:
        case CmdFgColor:
        case CmdBold:
        case CmdItalic:
        case CmdUnderline:
        case CmdFont:
            if (!isReadOnly()){
                cmd->flags &= ~BTN_HIDE;
            }else{
                cmd->flags |= BTN_HIDE;
            }
            return e->param();
        default:
            return NULL;
        }
    }
    if (e->type() == EventCommandExec){
        CommandDef *cmd = (CommandDef*)(e->param());
        if (cmd->param != m_param)
            return NULL;
        switch (cmd->id){
        case CmdBgColor:{
                Event eWidget(EventCommandWidget, cmd);
                QToolButton *btnBg = (QToolButton*)(eWidget.process());
                if (btnBg){
                    ColorPopup *popup = new ColorPopup(this, background());
                    popup->move(btnBg->pos());
                    connect(popup, SIGNAL(colorChanged(QColor)), this, SLOT(bgColorChanged(QColor)));
                    popup->show();
                }
                return e->param();
            }
        case CmdFgColor:{
                Event eWidget(EventCommandWidget, cmd);
                QToolButton *btnFg = (QToolButton*)(eWidget.process());
                if (btnFg){
                    ColorPopup *popup = new ColorPopup(this, foreground());
                    popup->move(btnFg->pos());
                    connect(popup, SIGNAL(colorChanged(QColor)), this, SLOT(fgColorChanged(QColor)));
                    popup->show();
                }
                return e->param();
            }
        case CmdBold:
            if (!m_bChanged){
                m_bSelected = true;
		if ((cmd->flags & COMMAND_CHECKED) != 0)
		{
		    setFontWeight(75);
		}
		else
		{
		    setFontWeight(50);
		}
            }
            return e->param();
        case CmdItalic:
            if (!m_bChanged){
                m_bSelected = true;
                setFontItalic((cmd->flags & COMMAND_CHECKED) != 0);
            }
            return e->param();
        case CmdUnderline:
            if (!m_bChanged){
                m_bSelected = true;
                setFontUnderline((cmd->flags & COMMAND_CHECKED) != 0);
            }
            return e->param();
        case CmdFont:{
#ifdef USE_KDE
                QFont f = font();
                if (KFontDialog::getFont(f, false, topLevelWidget()) != KFontDialog::Accepted)
                    break;
#else
                bool ok = false;
                QFont f = QFontDialog::getFont(&ok, font(), topLevelWidget());
                if (!ok)
                    break;
#endif
                m_bSelected = true;
                setCurrentFont(f);
                break;
            }
        default:
            return NULL;
        }
    }
    return NULL;
}

void TextShow::setBackground(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Base, c);
    pal.setColor(QPalette::Inactive, QPalette::Base, c);
    pal.setColor(QPalette::Disabled, QPalette::Base, c);
    setPalette(pal);
}

void TextShow::setForeground(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Text, c);
    setPalette(pal);
}

void TextEdit::setForeground(const QColor& c, bool bDef)
{
    curFG = c;
    if (bDef)
        defFG = c;
    if (!hasSelectedText)
        setTextColor(c);
    int r = Qt::red;
    if (r){
        r--;
    }else{
        r++;
    }
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Text, QColor(r, Qt::green, Qt::blue));
    setPalette(pal);
}

const QColor &TextShow::background() const
{
    return palette().color(QPalette::Active, QPalette::Base);
}

const QColor &TextShow::foreground() const
{
    return palette().color(QPalette::Active, QPalette::Text);
}

const QColor &TextEdit::foreground() const
{
    return curFG;
}

const QColor &TextEdit::defForeground() const
{
    return defFG;
}

void TextEdit::setTextFormat(Qt::TextFormat format)
{
}

TextShow::TextShow(QWidget *p, const char *name)
        : QTextEdit(p)
{
    setReadOnly(true);
    if (QApplication::clipboard()->supportsSelection())
        connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotResizeTimer()));
    connect(this, SIGNAL(copyAvailable(bool yes)), this, SLOT(slotCopyAvailable(bool)));
    hasSelectedText = false;
}

TextShow::~TextShow()
{
    emit finished();
}

void TextShow::emitLinkClicked(const QString &name)
{
    setSource(name);
}

void TextShow::setSource(const QString &name)
{
    if ( isVisible() )
        qApp->setOverrideCursor( Qt::WaitCursor );
    QString source = name;
    QString mark;
    int hash = name.indexOf('#');
    if ( hash != -1) {
        source = name.left( hash );
        mark = name.mid( hash+1 );
    }
    if ( source.left(5) == "file:" )
        source = source.mid(6);

    QString url = source;
    QString txt;

    if (!mark.isEmpty()) {
        url += "#";
        url += mark;
    }

    QByteArray s = url.toLocal8Bit();
    Event e(EventGoURL, (void *)static_cast<const char *>(url.toLocal8Bit()));
    e.process();

#ifndef QT_NO_CURSOR
    if ( isVisible() )
        qApp->restoreOverrideCursor();
#endif
}

void TextShow::slotResizeTimer()
{
#ifdef WIN32
    if (inResize())
        return;
    m_timer->stop();
    setVScrollBarMode(Auto);
    setHScrollBarMode(Auto);
    QResizeEvent re(QSize(0, 0), size());
    resizeEvent(&re);
#endif
}

void TextShow::slotCopyAvailable(const bool &yes)
{
    hasSelectedText = yes;
}

void TextShow::resizeEvent(QResizeEvent *e)
{
#ifdef WIN32
    if (inResize()){
        if (!m_timer->isActive()){
            setHScrollBarMode(AlwaysOff);
            setVScrollBarMode(AlwaysOff);
            m_timer->start(100);
        }
        return;
    }
#endif
    QPoint p = QPoint(0, height());
    p = mapToGlobal(p);
    p = viewport()->mapFromGlobal(p);
    viewport()->repaint();
}

void TextShow::keyPressEvent(QKeyEvent *e)
{
    if (((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_C)) ||
            ((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_Insert))){
        copy();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void TextShow::copy()
{
    QTextEdit::copy();
}

void TextShow::cut()
{
    if (hasSelectedText) {
        QTextEdit::cut();
    }
}

QMimeData *TextShow::dragObject(QWidget *parent) const
{
    if (!hasSelectedText)
        return NULL;

    RichTextDrag *drag = new RichTextDrag(parent);
    drag->setHtml(QApplication::clipboard()->text(QClipboard::Selection));
    return drag;
}

QString TextShow::quoteText(const char *t, const char *charset)
{
    if (t == NULL)
        t = "";
    QString text;
    QTextCodec *codec = NULL;
    if (charset)
        codec = QTextCodec::codecForName(charset);
    if (codec){
        text = codec->makeDecoder()->toUnicode(t, strlen(t));
    }else{
        text = QString::fromLocal8Bit(t);
    }
    return quoteString(text);
}

void TextShow::setText(const QString &text)
{
    QTextEdit::setPlainText(text);
}

void TextShow::slotSelectionChanged()
{
    disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
    if (QApplication::clipboard()->supportsSelection()){
        QMimeData *drag = dragObject(NULL);
        if ( !drag )
            return;
        QApplication::clipboard()->setMimeData(drag, QClipboard::Selection);
        connect( QApplication::clipboard(), SIGNAL(selectionChanged()), this, SLOT(clipboardChanged()));
    }
}

class BgColorParser : public HTMLParser
{
public:
    BgColorParser(TextEdit *edit);
protected:
    virtual void text(const QString &text);
    virtual void tag_start(const QString &tag, const list<QString> &options);
    virtual void tag_end(const QString &tag);
    TextEdit *m_edit;
};

BgColorParser::BgColorParser(TextEdit *edit)
{
    m_edit = edit;
}

void BgColorParser::text(const QString&)
{
}

void BgColorParser::tag_start(const QString &tag, const list<QString> &options)
{
    if (tag != "body")
        return;
    for (list<QString>::const_iterator it = options.begin(); it != options.end(); ++it){
        QString key = *it;
        ++it;
        QString val = *it;
        if (key == "bgcolor"){
            if (val[0] == '#'){
                bool bOK;
                unsigned rgb = val.mid(1).toUInt(&bOK, 16);
                if (bOK)
                    m_edit->setBackground(QColor(rgb));
            }
        }
    }
}

void BgColorParser::tag_end(const QString&)
{
}

static unsigned colors[16] =
    {
        0x000000,
        0xFF0000,
        0x00FF00,
        0x0000FF,
        0xFFFF00,
        0xFF00FF,
        0x00FFFF,
        0xFFFFFF,
        0x404040,
        0x800000,
        0x008000,
        0x000080,
        0x808000,
        0x800080,
        0x008080,
        0x808080
    };

const int CUSTOM_COLOR	= 100;

ColorPopup::ColorPopup(QWidget *popup, QColor color)
        : QFrame(popup, Qt::Popup | Qt::Tool)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    m_color = color;
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
    QGridLayout *lay = new QGridLayout(this);
    lay->setMargin(4);
    lay->setSpacing(2);
    for (unsigned i = 0; i < 4; i++){
        for (unsigned j = 0; j < 4; j++){
            unsigned n = i*4+j;
            QWidget *w = new ColorLabel(this, QColor(colors[n]), n, "");
            connect(w, SIGNAL(selected(int)), this, SLOT(colorSelected(int)));
            lay->addWidget(w, i, j);
        }
    }
    QWidget *w = new ColorLabel(this, color, CUSTOM_COLOR, i18n("Custom"));
    lay->addWidget(w, 5, 0, 5, 3);
    connect(w, SIGNAL(selected(int)), this, SLOT(colorSelected(int)));
    resize(minimumSizeHint());
}

void ColorPopup::colorSelected(int id)
{
    if (id == CUSTOM_COLOR){
        hide();
        QWidget *top = NULL;
        if (parent())
            top = static_cast<QWidget*>(parent())->topLevelWidget();
#ifdef USE_KDE
        QColor c = m_color;
        if (KColorDialog::getColor(c, top) != KColorDialog::Accepted){
            close();
            return;
        }
#else
        QColor c = QColorDialog::getColor(m_color, top);
        if (!c.isValid()){
            close();
            return;
        }
#endif
        emit colorChanged(c);
        close();
    }else{
        emit colorChanged(QColor(colors[id]));
        close();
    }
}

ColorLabel::ColorLabel(QWidget *parent, QColor c, int id, const QString &text)
        : QLabel(parent)
{
    m_id = id;
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Base, c);
    pal.setColor(QPalette::Inactive, QPalette::Base, c);
    pal.setColor(QPalette::Disabled, QPalette::Base, c);
    setPalette(pal);
    setText(text);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setFrameShape(StyledPanel);
    setFrameShadow(Sunken);
    setLineWidth(2);
}

void ColorLabel::mouseReleaseEvent(QMouseEvent*)
{
    emit selected(m_id);
}

QSize ColorLabel::sizeHint() const
{
    QSize s = QLabel::sizeHint();
    if (s.width() < s.height())
        s.setWidth(s.height());
    return s;
}

QSize ColorLabel::minimumSizeHint() const
{
    QSize s = QLabel::minimumSizeHint();
    if (s.width() < s.height())
        s.setWidth(s.height());
    return s;
}


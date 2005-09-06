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
#include "toolbtn.h"
#include "html.h"
#include "simapi.h"

#ifdef USE_KDE
#include <keditcl.h>
#include <kstdaccel.h>
#include <kglobal.h>
#include <kfiledialog.h>
#define Q3FileDialog	KFileDialog
#ifdef HAVE_KROOTPIXMAP_H
#include <krootpixmap.h>
#endif
#else
#include <q3filedialog.h>
//Added by qt3to4:
#include <Q3CString>
#include <QGridLayout>
#include <QKeyEvent>
#include <QEvent>
#include <Q3Frame>
#include <QLabel>
#include <QContextMenuEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#endif

#ifdef USE_KDE
#include <kcolordialog.h>
#include <kfontdialog.h>
#else
#include <qcolordialog.h>
#include <qfontdialog.h>
#endif

#include <qdatetime.h>
#include <q3popupmenu.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qobject.h>
#include <q3valuelist.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qtextcodec.h>
#include <q3toolbar.h>
#include <qlineedit.h>
#include <q3accel.h>
#include <q3dragobject.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qtooltip.h>
#include <qlayout.h>

#define MAX_HISTORY	100

class RichTextDrag : public Q3TextDrag
{
public:
    RichTextDrag(QWidget *dragSource = 0, const char *name = 0);

    void setRichText(const QString &txt);

    virtual QByteArray encodedData(const char *mime) const;
    virtual const char* format(int i) const;

    static bool decode(QMimeSource *e, QString &str, const Q3CString &mimetype, const Q3CString &subtype);
    static bool canDecode( QMimeSource* e );
private:
    QString richTxt;
};

RichTextDrag::RichTextDrag( QWidget *dragSource, const char *name )
        : Q3TextDrag( dragSource, name )
{
}

QByteArray RichTextDrag::encodedData( const char *mime ) const
{
    if ( qstrcmp( "application/x-qrichtext", mime ) == 0 ) {
        return richTxt.utf8(); // #### perhaps we should use USC2 instead?
    } else
        return Q3TextDrag::encodedData( mime );
}

bool RichTextDrag::decode(QMimeSource *e, QString &str, const Q3CString &mimetype, const Q3CString &subtype)
{
    if (mimetype == "application/x-qrichtext"){
        const char *mime;
        int i;
        for ( i = 0; (mime = e->format(i)) != NULL; ++i ) {
            if (qstrcmp("application/x-qrichtext", mime) != 0)
                continue;
            str = QString::fromUtf8(e->encodedData(mime));
            return TRUE;
        }
        return FALSE;
    }
    Q3CString subt = subtype;
    return Q3TextDrag::decode( e, str, subt );
}

bool RichTextDrag::canDecode(QMimeSource* e)
{
    if ( e->provides("application/x-qrichtext"))
        return TRUE;
    return Q3TextDrag::canDecode(e);
}

const char* RichTextDrag::format(int i) const
{
    if ( Q3TextDrag::format(i))
        return Q3TextDrag::format(i);
    if ( Q3TextDrag::format(i-1))
        return "application/x-qrichtext";
    return 0;
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
    curFG = colorGroup().color(QColorGroup::Text);
    m_bCtrlMode = true;
    setWordWrap(WidgetWidth);
#if COMPAT_QT_VERSION >= 0x030100
    setAutoFormatting(0);
#endif
    connect(this, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(fontChanged(const QFont&)));
    connect(this, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(slotColorChanged(const QColor&)));
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
#if COMPAT_QT_VERSION >= 0x030000
    connect(this, SIGNAL(clicked(int,int)), this, SLOT(slotClicked(int,int)));
#endif
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
#if COMPAT_QT_VERSION >= 0x030000
    int paraFrom, paraTo, indexFrom, indexTo;
    getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
    if ((paraFrom != paraTo) || (indexFrom != indexTo))
        return;
    m_bInClick = true;
    QContextMenuEvent e(QContextMenuEvent::Other, QPoint(0, 0), QPoint(0, 0), 0);
    contentsContextMenuEvent(&e);
    m_bInClick = false;
#endif
}

Q3PopupMenu *TextEdit::createPopupMenu(const QPoint& pos)
{
    if (m_bInClick)
        return NULL;
    m_popupPos = pos;
    return TextShow::createPopupMenu(pos);
}

bool TextEdit::isEmpty()
{
    if (paragraphs() < 2){
        QString t = text(0);
        if (textFormat() == Q3TextEdit::RichText)
            t = unquoteText(t);
        return t.isEmpty() || (t == " ");
    }
    return false;
}

void TextEdit::setParam(void *param)
{
    m_param = param;
}

void TextEdit::slotColorChanged(const QColor &c)
{
    if (c == curFG)
        return;
    int parag;
    int index;
    getCursorPosition(&parag, &index);
    if (Q3TextEdit::text(parag).isEmpty()){
        setColor(curFG);
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
    return Q3TextEdit::eventFilter(o, e);
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
    if (((e->key() == Key_Enter) || (e->key() == Key_Return)))
    {
        //   in m_bCtrlMode:    enter      --> newLine
        //                      ctrl+enter --> sendMsg
        //   in !m_bCtrlMode:   enter      --> sendMsg
        //                      ctrl+enter --> newLine
        // the (bool) is required due to the bitmap
        if (m_bCtrlMode == (bool)(e->state() & ControlButton)){
            emit ctrlEnterPressed();
            return;
        }
    }
    if (!isReadOnly()){
        if ((e->state() == ShiftButton) && (e->key() == Key_Insert)){
            paste();
            return;
        }
        if ((e->state() == ControlButton) && (e->key() == Key_Delete)){
            cut();
            return;
        }
    }
#if (COMPAT_QT_VERSION >= 0x030000) && (COMPAT_QT_VERSION < 0x030100)
    // Workaround about autoformat feature in qt 3.0.x
    if ((e->text()[0] == '-') || (e->text()[0] == '*')){
        if (isOverwriteMode() && !hasSelectedText())
            moveCursor(MoveForward, true);
        insert( e->text(), TRUE, FALSE );
        return;
    }
#endif
    // Note: We no longer translate Enter to Ctrl-Enter since we need
    // to know about paragraph breaks now.
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
            if ((textFormat() == RichText) && !isReadOnly()){
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
                CToolButton *btnBg = (CToolButton*)(eWidget.process());
                if (btnBg){
                    ColorPopup *popup = new ColorPopup(this, background());
                    popup->move(CToolButton::popupPos(btnBg, popup));
                    connect(popup, SIGNAL(colorChanged(QColor)), this, SLOT(bgColorChanged(QColor)));
                    popup->show();
                }
                return e->param();
            }
        case CmdFgColor:{
                Event eWidget(EventCommandWidget, cmd);
                CToolButton *btnFg = (CToolButton*)(eWidget.process());
                if (btnFg){
                    ColorPopup *popup = new ColorPopup(this, foreground());
                    popup->move(CToolButton::popupPos(btnFg, popup));
                    connect(popup, SIGNAL(colorChanged(QColor)), this, SLOT(fgColorChanged(QColor)));
                    popup->show();
                }
                return e->param();
            }
        case CmdBold:
            if (!m_bChanged){
                m_bSelected = true;
                setBold((cmd->flags & COMMAND_CHECKED) != 0);
            }
            return e->param();
        case CmdItalic:
            if (!m_bChanged){
                m_bSelected = true;
                setItalic((cmd->flags & COMMAND_CHECKED) != 0);
            }
            return e->param();
        case CmdUnderline:
            if (!m_bChanged){
                m_bSelected = true;
                setUnderline((cmd->flags & COMMAND_CHECKED) != 0);
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
    pal.setColor(QPalette::Active, QColorGroup::Base, c);
    pal.setColor(QPalette::Inactive, QColorGroup::Base, c);
    pal.setColor(QPalette::Disabled, QColorGroup::Base, c);
    setPalette(pal);
}

void TextShow::setForeground(const QColor& c)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QColorGroup::Text, c);
    setPalette(pal);
}

void TextEdit::setForeground(const QColor& c, bool bDef)
{
    curFG = c;
    if (bDef)
        defFG = c;
    if (!hasSelectedText())
        setColor(c);
    int r = c.Qt::red();
#if COMPAT_QT_VERSION >= 0x030000
    if (r){
        r--;
    }else{
        r++;
    }
#endif
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QColorGroup::Text, QColor(r, c.Qt::green(), c.Qt::blue()));
    setPalette(pal);
}

const QColor &TextShow::background() const
{
    return palette().color(QPalette::Active, QColorGroup::Base);
}

const QColor &TextShow::foreground() const
{
    return palette().color(QPalette::Active, QColorGroup::Text);
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
    if (format == textFormat())
        return;
    if (format == RichText){
        Q3TextEdit::setTextFormat(format);
        return;
    }
    QString t = unquoteText(text());
    Q3TextEdit::setTextFormat(format);
    setText(t);
}

TextShow::TextShow(QWidget *p, const char *name)
        : Q3TextEdit(p, name)
{
    setTextFormat(RichText);
    setReadOnly(true);
#if COMPAT_QT_VERSION >= 0x030100
    if (QApplication::clipboard()->supportsSelection())
        connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
#endif
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotResizeTimer()));
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
#ifndef QT_NO_CURSOR
    if ( isVisible() )
        qApp->setOverrideCursor( waitCursor );
#endif
    QString source = name;
    QString mark;
    int hash = name.find('#');
    if ( hash != -1) {
        source = name.left( hash );
        mark = name.mid( hash+1 );
    }
    if ( source.left(5) == "file:" )
        source = source.mid(6);

    QString url = mimeSourceFactory()->makeAbsolute( source, context() );
    QString txt;

    if (!mark.isEmpty()) {
        url += "#";
        url += mark;
    }

    Q3CString s = url.local8Bit();
    Event e(EventGoURL, (void*)(const char*)url);
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
    int x, y;
    viewportToContents(p.x(), p.y(), x, y);
    int para;
    int pos;
    if (isReadOnly()){
        pos = charAt(QPoint(x, y), &para);
    }else{
        getCursorPosition(&para, &pos);
    }
    Q3TextEdit::resizeEvent(e);
    if (pos == -1){
        scrollToBottom();
    }else{
        setCursorPosition(para, pos);
        ensureCursorVisible();
    }
    sync();
    viewport()->repaint();
}


void TextShow::keyPressEvent(QKeyEvent *e)
{
    if (((e->state() == Qt::ControlButton) && (e->key() == Qt::Key_C)) ||
            ((e->state() == ControlButton) && (e->key() == Key_Insert))){
        copy();
        return;
    }
    Q3TextEdit::keyPressEvent(e);
}

void TextShow::copy()
{
    Q3TextDrag *drag = dragObject(NULL);
    if ( !drag )
        return;
#if COMPAT_QT_VERSION <= 0x030100
    QApplication::clipboard()->setData(drag);
#else
    QApplication::clipboard()->setData(drag, QClipboard::Clipboard);
#endif
}

void TextShow::cut()
{
    if (isReadOnly())
        return;
    if (hasSelectedText()) {
        copy();
        removeSelectedText();
    }
}

Q3TextDrag *TextShow::dragObject(QWidget *parent) const
{
    if (!hasSelectedText())
        return NULL;
#if (COMPAT_QT_VERSION < 0x030000) || (COMPAT_QT_VERSION >= 0x030100)
    if (textFormat() == RichText){
        RichTextDrag *drag = new RichTextDrag(parent);
        drag->setRichText(selectedText());
        return drag;
    }
#endif
    return new Q3TextDrag(selectedText(), parent );
}

void TextShow::startDrag()
{
    Q3DragObject *drag = dragObject(viewport());
    if (drag == NULL)
        return;
    if (isReadOnly()) {
        drag->dragCopy();
    } else {
        if (drag->drag() && Q3DragObject::target() != this && Q3DragObject::target() != viewport() )
            removeSelectedText();
    }
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
    Q3TextEdit::setText(text, "");
}

void TextShow::slotSelectionChanged()
{
#if COMPAT_QT_VERSION >= 0x030100
    disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
    if (QApplication::clipboard()->supportsSelection()){
        Q3TextDrag *drag = dragObject(NULL);
        if ( !drag )
            return;
        QApplication::clipboard()->setData(drag, QClipboard::Selection);
        connect( QApplication::clipboard(), SIGNAL(selectionChanged()), this, SLOT(clipboardChanged()));
    }
#endif
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

RichTextEdit::RichTextEdit(QWidget *parent, const char *name)
        : Q3MainWindow(parent, name, 0)
{
    m_edit = new TextEdit(this);
    m_bar  = NULL;
    setCentralWidget(m_edit);
}

void RichTextEdit::setText(const QString &str)
{
    if (m_edit->textFormat() != Q3TextEdit::RichText)
        m_edit->setText(str);
    BgColorParser p(m_edit);
    p.parse(str);
    m_edit->setText(str);
}

QString RichTextEdit::text()
{
    if (m_edit->textFormat() != Q3TextEdit::RichText)
        return m_edit->text();
    char bg[20];
    sprintf(bg, "%06X", m_edit->background().rgb());
    QString res;
    res = "<BODY BGCOLOR=\"#";
    res += bg;
    res += "\">";
    res += m_edit->text();
    res += "</BODY>";
    return res;
}

void RichTextEdit::setTextFormat(Qt::TextFormat format)
{
    m_edit->setTextFormat(format);
}

Qt::TextFormat RichTextEdit::textFormat()
{
    return m_edit->textFormat();
}

void RichTextEdit::setReadOnly(bool bState)
{
    m_edit->setReadOnly(bState);
}

void RichTextEdit::showBar()
{
    if (m_bar)
        return;
    BarShow b;
    b.bar_id = ToolBarTextEdit;
    b.parent = this;
    Event e(EventShowBar, &b);
    m_bar = (CToolBar*)(e.process());
    m_bar->setParam(this);
    m_edit->setParam(this);
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
        : Q3Frame(popup, "colors", Qt::WType_Popup | Qt::WStyle_Customize | Qt::WStyle_Tool | Qt::WDestructiveClose)
{
    m_color = color;
    setFrameShape(PopupPanel);
    setFrameShadow(Sunken);
    QGridLayout *lay = new QGridLayout(this, 5, 4);
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
    lay->addMultiCellWidget(w, 5, 5, 0, 3);
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
    setText(text);
    setBackgroundColor(c);
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

#ifndef _WINDOWS
#include "textshow.moc"
#endif

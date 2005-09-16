/* This file is part of the KDE libraries
   Copyright (C) 2000 Daniel M. Duley <mosfet@kde.org>
   Copyright (C) 2002 Hamish Rodda <meddie@yoyo.its.monash.edu.au>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "simapi.h"
#include "kpopup.h"

#ifndef USE_KDE

#include <qpainter.h>
#include <qdrawutil.h>
#include <qtimer.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QPixmap>
#include <QKeyEvent>
#include <Q3PopupMenu>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QMenuItem>

KPopupTitle::KPopupTitle(QWidget *parent, const char *name)
        : QWidget(parent, name)
{
    bgColor = colorGroup().mid();
    fgColor = colorGroup().highlightedText();
}

void KPopupTitle::setTitle(const QString &text, const QPixmap *icon)
{
    titleStr = text;
    if(icon){
        miniicon = *icon;
    }
    else
        miniicon.resize(0, 0);

    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void KPopupTitle::setText( const QString &text )
{
    titleStr = text;
    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void KPopupTitle::setIcon( const QPixmap &pix )
{
    miniicon = pix;
    int w = miniicon.width()+fontMetrics().width(titleStr);
    int h = QMAX( fontMetrics().height(), miniicon.height() );
    setMinimumSize( w+16, h+8 );
}

void KPopupTitle::paintEvent(QPaintEvent *)
{
    QRect r(rect());
    QPainter p(this);

    p.fillRect(2, 2, r.width()-4, r.height()-4, QBrush(bgColor));

    if(!miniicon.isNull())
        p.drawPixmap(4, (r.height()-miniicon.height())/2, miniicon);
    if(!titleStr.isNull()){
        p.setPen(fgColor);
        if(!miniicon.isNull())
            p.drawText(miniicon.width()+8, 0, width()-(miniicon.width()+8),
                       height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine,
                       titleStr);
        else
            p.drawText(0, 0, width(), height(),
                       Qt::AlignCenter | Qt::SingleLine, titleStr);
    }
    p.setPen(Qt::black);
    p.drawRect(r);
}

QSize KPopupTitle::sizeHint() const
{
    return(minimumSize());
}

class KPopupMenu::KPopupMenuPrivate
{
public:
    KPopupMenuPrivate ()
: noMatches(false)
    , shortcuts(false)
    , autoExec(false)
    , lastHitIndex(-1)
    {}

    QString m_lastTitle;

    // variables for keyboard navigation
    QTimer clearTimer;

bool noMatches : 1;
bool shortcuts : 1;
bool autoExec : 1;

    QString keySeq;
    QString originalText;

    int lastHitIndex;
};


KPopupMenu::KPopupMenu(QWidget *parent, const char *name)
        : Q3PopupMenu(parent, name)
{
    d = new KPopupMenuPrivate;
    resetKeyboardVars();
    connect(&(d->clearTimer), SIGNAL(timeout()), SLOT(resetKeyboardVars()));
}

KPopupMenu::~KPopupMenu()
{
    delete d;
}

int KPopupMenu::insertTitle(const QString &text, int id, int index)
{
    return(insertItem(text, id, index));
}

int KPopupMenu::insertTitle(const QPixmap &icon, const QString &text, int id,
                            int index)
{
    return(insertItem(icon, text, id, index));
}

void KPopupMenu::changeTitle(int id, const QString &text)
{
    QMenuItem *item = findItem(id);
    if(item){
	item->setText( text);
    }
    else
        qWarning("KPopupMenu: changeTitle() called with invalid id %d.", id);
}

void KPopupMenu::changeTitle(int id, const QPixmap &icon, const QString &text)
{
    QMenuItem *item = findItem(id);
    QIcon newicon;
    newicon.addPixmap( icon, QIcon::Normal, QIcon::Off);
    if(item){
	item->setText(text);
	item->setIcon(newicon);
    }
    else
        qWarning("KPopupMenu: changeTitle() called with invalid id %d.", id);
}

QString KPopupMenu::title(int id) const
{
    if(id == -1) // obsolete
        return(d->m_lastTitle);
    QMenuItem *item = findItem(id);
    if(item){
        return item->text();
    }
    else
        qWarning("KPopupMenu: title() called with invalid id %d.", id);
    return(QString::null);
}

QPixmap KPopupMenu::titlePixmap(int id) const
{
    QMenuItem *item = findItem(id);
    if(item){
        return item->icon().pixmap(32, QIcon::Normal, QIcon::Off);
    }
    else
        qWarning("KPopupMenu: titlePixmap() called with invalid id %d.", id);
    QPixmap tmp;
    return(tmp);
}

/**
 * This is re-implemented for keyboard navigation.
 */
void KPopupMenu::closeEvent(QCloseEvent*)
{
    if (d->shortcuts)
        resetKeyboardVars();
}

void KPopupMenu::keyPressEvent(QKeyEvent* e)
{
    if (!d->shortcuts) {
        // continue event processing by Qpopup
        //e->ignore();
        Q3PopupMenu::keyPressEvent(e);
        return;
    }

    int i = 0;
    bool firstpass = true;
    QString keyString = e->text();

    // check for common commands dealt with by QPopup
    int key = e->key();
    if (key == Qt::Key_Escape || key == Qt::Key_Return || key == Qt::Key_Enter
            || key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left
            || key == Qt::Key_Right || key == Qt::Key_F1) {

        resetKeyboardVars();
        // continue event processing by Qpopup
        //e->ignore();
        Q3PopupMenu::keyPressEvent(e);
        return;
    }

    // check to see if the user wants to remove a key from the sequence (backspace)
    // or clear the sequence (delete)
    if (d->keySeq != QString::null) {

        if (key == Qt::Key_Backspace) {

            if (d->keySeq.length() == 1) {
                resetKeyboardVars();
                return;
            }

            // keep the last sequence in keyString
            keyString = d->keySeq.left(d->keySeq.length() - 1);

            // allow sequence matching to be tried again
            resetKeyboardVars();

        } else if (key == Qt::Key_Delete) {
            resetKeyboardVars();
            return;

        } else if (d->noMatches) {
            // clear if there are no matches
            resetKeyboardVars();

        } else {
            // the key sequence is not a null string
            // therefore the lastHitIndex is valid
            i = d->lastHitIndex;
        }
    } else if (key == Qt::Key_Backspace) {
        hide();
        resetKeyboardVars();
        return;
    }

    d->keySeq += keyString;
    int seqLen = d->keySeq.length();

    for (; i < (int)count(); i++) {
        // compare typed text with text of this entry
        int j = idAt(i);

        // dont search disabled entries
        if (!isItemEnabled(j))
            continue;

        QString thisText;

        // retrieve the right text
        // (the last selected item one may have additional ampersands)
        if (i == d->lastHitIndex)
            thisText = d->originalText;
        else
            thisText = text(j);

        // if there is an accelerator present, remove it
        if ((int)accel(j) != 0)
            thisText = thisText.replace(QRegExp("&"), "");

        // chop text to the search length
        thisText = thisText.left(seqLen);

        // do the search
        if (thisText.find(d->keySeq, 0, false) == 0) {

            if (firstpass) {

                // check to see if were underlining a different item
                if (d->lastHitIndex != i)
                    // yes; revert the underlining
                    changeItem(idAt(d->lastHitIndex), d->originalText);

                // set the original text if its a different item
                if (d->lastHitIndex != i || d->lastHitIndex == -1)
                    d->originalText = text(j);

                // underline the currently selected item
                changeItem(j, underlineText(d->originalText, d->keySeq.length()));

                // remeber whats going on
                d->lastHitIndex = i;

                // start/restart the clear timer
                d->clearTimer.start(5000, true);

                // go around for another try, to see if we can execute
                firstpass = false;
            } else {
                // dont allow execution
                return;
            }
        }

        // fall through to allow execution
    }

    if (!firstpass) {
        if (d->autoExec) {
            // activate anything
            activateItemAt(d->lastHitIndex);
            resetKeyboardVars();

        } else if (findItem(idAt(d->lastHitIndex)))  {
            // only activate sub-menus
            activateItemAt(d->lastHitIndex);
            resetKeyboardVars();
        }

        return;
    }

    // no matches whatsoever, clean up
    resetKeyboardVars(true);
    //e->ignore();
    Q3PopupMenu::keyPressEvent(e);
}

QString KPopupMenu::underlineText(const QString& text, uint length)
{
    QString ret = text;
    for (int i = 0; i < (int)length; i++) {
        if (ret[2*i] != '&')
            ret.insert(2*i, "&");
    }
    return ret;
}

void KPopupMenu::resetKeyboardVars()
{
    resetKeyboardVars(false);
}

void KPopupMenu::resetKeyboardVars(bool noMatches)
{
    // Clean up keyboard variables
    if (d->lastHitIndex != -1) {
        changeItem(idAt(d->lastHitIndex), d->originalText);
        d->lastHitIndex = -1;
    }

    if (!noMatches) {
        d->keySeq = QString::null;
    }

    d->noMatches = noMatches;
}

void KPopupMenu::setKeyboardShortcutsEnabled(bool enable)
{
    d->shortcuts = enable;
}

void KPopupMenu::setKeyboardShortcutsExecute(bool enable)
{
    d->autoExec = enable;
}
/**
 * End keyboard navigation.
 */

// Obsolete
KPopupMenu::KPopupMenu(const QString& title, QWidget *parent, const char *name)
        : Q3PopupMenu(parent, name)
{
    d = new KPopupMenuPrivate;
    setTitle(title);
}

// Obsolete
void KPopupMenu::setTitle(const QString &title)
{
    insertItem(title);
    d->m_lastTitle = title;
}

#ifndef _WINDOWS
#include "kpopup.moc"
#endif

#endif

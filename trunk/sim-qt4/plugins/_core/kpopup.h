/* This file is part of the KDE libraries
   Copyright (C) 2000 Daniel M. Duley <mosfet@kde.org>

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

#ifdef HAVE_CONFIG_H
#include "config.h"
//Added by qt3to4:
#include <QPixmap>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QCloseEvent>
#endif

#ifndef USE_KDE

#ifndef _KPOPUP_H
#define _KPOPUP_H "$Id: kpopup.h,v 1.2 2003/08/02 19:21:24 shutoff Exp $"

#define INCLUDE_MENUITEM_DEF

#include <q3popupmenu.h>

/**
 * Title widget for use in @ref KPopupMenu.
 *
 * You usually don't have to create this manually since
 * @ref KPopupMenu::insertTitle will do it for you, but it is allowed if
 * you wish to customize it's look.
 *
 * @author Daniel M. Duley <mosfet@kde.org>
 * @short KPopupMenu title widget.
 */
class KPopupTitle : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a title widget with the user specified gradient, pixmap,
     * and colors.
     */
    KPopupTitle(QWidget *parent=0, const char *name=0);
    /**
     * Sets the title string and optional icon for the title widget.
     *
     * You will want to call this before inserting into a menu.
     */
    void setTitle(const QString &text, const QPixmap *icon=NULL);
    /**
     * Returns the current title.
     */
    QString title() const { return(titleStr); }
    /**
     * Returns the current icon.
     */
    QPixmap icon() const { return(miniicon); }

    QSize sizeHint() const;

public slots:
    void setText( const QString &text );
    void setIcon( const QPixmap &pix );

protected:
    void paintEvent(QPaintEvent *ev);

    QString titleStr;
    QPixmap miniicon;
    QColor fgColor, bgColor;
};

/**
 * KPopupMenu is a class for menus with standard title items and keyboard
 * accessibility for popups with many options and/or varying options. It acts
 * identically to QPopupMenu, with the addition of insertTitle(),
 * changeTitle(), setKeyboardShortcutsEnabled() and
 * setKeyboardShortcutsExecute() methods.
 *
 * The titles support a text string, an icon, plus user defined gradients,
 * colors, and background pixmaps.
 *
 * The keyboard search algorithm is incremental with additional underlining
 * for user feedback.
 *
 * @short A menu with title items.
 * @author Daniel M. Duley <mosfet@kde.org>
 * @author Hamish Rodda <meddie@yoyo.its.monash.edu.au>
 */
class KPopupMenu : public Q3PopupMenu {
    Q_OBJECT
public:
    /**
     * Constructs a KPopupMenu.
     */
    KPopupMenu(QWidget *parent=0, const char *name=0);

    /**
     * Destructs the object
     */
    ~KPopupMenu();

    /**
     * Inserts a title item with no icon.
     */
    int insertTitle(const QString &text, int id=-1, int index=-1);
    /**
     * Inserts a title item with the given icon and title.
     */
    int insertTitle(const QPixmap &icon, const QString &text, int id=-1,
                    int index=-1);
    /**
     * Changes the title of the item at the specified id. If a icon was
     * previously set it is cleared.
     */
    void changeTitle(int id, const QString &text);
    /**
     * Changes the title and icon of the title item at the specified id.
     */
    void changeTitle(int id, const QPixmap &icon, const QString &text);
    /**
     * Returns the title of the title item at the specified id. The default
     * id of -1 is for backwards compatibility only, you should always specify
     * the id.
     */
    QString title(int id=-1) const;
    /**
     * Returns the icon of the title item at the specified id.
     */
    QPixmap titlePixmap(int id) const;

    /**
     * Enables keyboard navigation by searching for the entered key sequence.
     * Also underlines the currently selected item, providing feedback on the search.
     *
     * Defaults to off.
     *
     * WARNING: calls to text() of currently keyboard-selected items will
     * contain additional ampersand characters.
     *
     * WARNING: though pre-existing keyboard shortcuts will not interfere with the
     * operation of this feature, they may be confusing to the user as the existing
     * shortcuts will not work.
     */
    void setKeyboardShortcutsEnabled(bool enable);

    /**
     * Enables execution of the menu item once it is uniquely specified.
     * Defaults to off.
     */
    void setKeyboardShortcutsExecute(bool enable);

    /**
     * Obsolete method provided for backwards compatibility only. Use the
     * normal constructor and insertTitle instead.
     */
    KPopupMenu(const QString &title, QWidget *parent=0, const char *name=0);
    /**
     * Obsolete method provided for backwards compatibility only. Use
     * insertTitle and changeTitle instead.
     */
    void setTitle(const QString &title);

protected:
    virtual void closeEvent(QCloseEvent *);
    virtual void keyPressEvent(QKeyEvent* e);

protected slots:
    QString underlineText(const QString& text, uint length);
    void resetKeyboardVars(bool noMatches);
    void resetKeyboardVars();

private:
    class KPopupMenuPrivate;
    KPopupMenuPrivate *d;
};

#endif
#endif

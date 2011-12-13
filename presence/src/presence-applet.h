/***************************************************************************
 *   Copyright (C) 2011 by Francesco Nwokeka <francesco.nwokeka@gmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef TELEPATHY_KDE_PRESENCE_APPLET_H
#define TELEPATHY_KDE_PRESENCE_APPLET_H

#include <Plasma/PopupApplet>
#include <Plasma/DeclarativeWidget>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/Presence>

#include <KTelepathy/presence.h>

namespace KTp {
    class GlobalPresence;
}

namespace Tp {
    class PendingOperation;
}

class GlobalPresenceWrapper;
class QAction;

class TelepathyPresenceApplet: public Plasma::PopupApplet
{
    Q_OBJECT

public:
    TelepathyPresenceApplet(QObject *parent, const QVariantList &args);
    ~TelepathyPresenceApplet();

    QList<QAction*>contextualActions();
    void init();
    void paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);

    /// TODO
//     /** overide of config signal */
//     void showConfigurationInterface();

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void onPresenceChanged(KTp::Presence presence);
    void onPresenceActionClicked();
    void startAccountManager() const;
    void startContactList() const;
    void toolTipAboutToShow();
    void toolTipHidden();

private:
    void setupAccountManager();

    /** used only upon creation to setup a list of actions for the context menu */
    void setupContextMenuActions();

    QList<QAction*>m_contextActions;

    Tp::AccountManagerPtr m_accountManager;
    KTp::GlobalPresence *m_globalPresence;
};

#endif  // TELEPATHY_KDE_PRESENCE_APPLET_H

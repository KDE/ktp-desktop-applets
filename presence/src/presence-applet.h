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
class KConfigDialog;
class QAction;
class DBusExporter;

class TelepathyPresenceApplet: public Plasma::PopupApplet
{
    Q_OBJECT

public:
    enum OnClickAction {
        DO_NOTHING,
        SHOW_ACCOUNT_MANAGER,
        SHOW_CONTACTLIST
    };

    TelepathyPresenceApplet(QObject *parent, const QVariantList &args);
    ~TelepathyPresenceApplet();

    QList<QAction*> contextualActions();
    void createConfigurationInterface(KConfigDialog *parentDialog);
    void init();
    void paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);

    /** called when plasmoid is clicked */
    void onActivated();
    void onPresenceChanged(KTp::Presence presence);
    void onPresenceActionClicked();
    void startAccountManager() const;
    void startContactList() const;
    void toolTipAboutToShow();
    void toolTipHidden();
    void updateClickAction(TelepathyPresenceApplet::OnClickAction);

private:
    void setupAccountManager();

    /** used only upon creation to setup a list of actions for the context menu */
    void setupContextMenuActions();

    QList<QAction*>       m_contextActions;

    Tp::AccountManagerPtr m_accountManager;
    KTp::GlobalPresence  *m_globalPresence;
    OnClickAction         m_onClickAction;
    DBusExporter         *m_dbusExporter;
};

#endif  // TELEPATHY_KDE_PRESENCE_APPLET_H

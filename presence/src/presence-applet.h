/*
 * Copyright (C) 2011 by Francesco Nwokeka <francesco.nwokeka@gmail.com>
 * Copyright (C) 2012 by Martin Klapetek <martin.klapetek@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef KTP_PRESENCE_APPLET_H
#define KTP_PRESENCE_APPLET_H

#include <Plasma/Applet>
#include <Plasma/DeclarativeWidget>
#include <Plasma/IconWidget>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/Presence>

#include <KTp/presence.h>

namespace KTp {
    class GlobalPresence;
}

namespace Tp {
    class PendingOperation;
}

class GlobalPresenceWrapper;
class KConfigDialog;
class QAction;

class TelepathyPresenceApplet: public Plasma::Applet
{
    Q_OBJECT

public:
    TelepathyPresenceApplet(QObject *parent, const QVariantList &args);
    ~TelepathyPresenceApplet();

    QList<QAction*> contextualActions();
    void init();

public slots:
     Q_SCRIPTABLE int handleCustomPresenceChange();

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void onAccountsChanged();

    void onPresenceChanged(KTp::Presence presence);
    void onConnectionStatusChanged(Tp::ConnectionStatus connectionStatus);
    void onPresenceActionClicked();
    void onJoinChatRoomRequest();
    void onJoinChatRoomSelected();
    void toolTipAboutToShow();
    void toolTipHidden();

    void startAccountManager();
    void toggleContactList();
    void onAddContactRequest();
    void onMakeCallRequest();
    void onSendFileRequest();

    void contactListServiceRegistered();
    void contactListServiceUnregistered();
    void serviceNameFetchFinished(QDBusPendingCallWatcher *callWatcher);

private:

    /** used to get the correct icon from either the Plasma theme or the Oxygen icon set */
    KIcon getThemedIcon(const QString &iconBaseName) const;

    /** used only upon creation to setup a list of actions for the context menu */
    void setupContextMenuActions();

    /** the number of instances which have tried to register the DBus service*/
    static int s_instanceCount;

    QList<QAction*>       m_contextActions;
    Plasma::IconWidget   *m_icon;

    Tp::AccountManagerPtr m_accountManager;
    KTp::GlobalPresence  *m_globalPresence;

    QDBusServiceWatcher  *m_contactListWatcher;
    bool                  m_contactListRunning;

    void addPresence(const KTp::Presence &presence);
    QList<KTp::Presence> m_presences;
};

#endif  // KTP_PRESENCE_APPLET_H

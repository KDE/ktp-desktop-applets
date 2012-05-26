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
class DBusExporter;

class TelepathyPresenceApplet: public Plasma::Applet
{
    Q_OBJECT

public:
    TelepathyPresenceApplet(QObject *parent, const QVariantList &args);
    ~TelepathyPresenceApplet();

    QList<QAction*> contextualActions();
    void init();

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);

    void onPresenceChanged(KTp::Presence presence);
    void onPresenceActionClicked();
    void toolTipAboutToShow();
    void toolTipHidden();

    void startAccountManager();
    void startContactList();
    void onAddContactRequest();
    void onAddContactRequestFoundContacts(Tp::PendingOperation *operation);

private:
    /** used only upon creation to setup a list of actions for the context menu */
    void setupContextMenuActions();

    QList<QAction*>       m_contextActions;
    Plasma::IconWidget   *m_icon;

    Tp::AccountManagerPtr m_accountManager;
    KTp::GlobalPresence  *m_globalPresence;
    DBusExporter         *m_dbusExporter;
};

//-----------------------------------------------------------------------------------------

class DBusExporter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Telepathy.PresenceApplet")

public:
    DBusExporter(QObject *parent = 0) : QDBusAbstractAdaptor(parent) {}
    ~DBusExporter() {};
};

#endif  // KTP_PRESENCE_APPLET_H

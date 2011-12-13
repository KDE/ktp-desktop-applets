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

#include "presenceapplet.h"

#include <KAction>
#include <KActionMenu>
#include <KToolInvocation>
#include <KUser>

#include <KTelepathy/global-presence.h>
#include <KTelepathy/presence.h>

#include <Plasma/ToolTipManager>

#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingReady>


TelepathyPresenceApplet::TelepathyPresenceApplet(QObject* parent, const QVariantList& args)
    : Plasma::PopupApplet(parent, args)
    , m_globalPresence(new KTp::GlobalPresence(this))
{
    setupContextMenuActions();
    setupAccountManager();

    resize(128, 128);
    setAspectRatioMode(Plasma::KeepAspectRatio);
    setBackgroundHints(NoBackground);

    int iconSize = IconSize(KIconLoader::Small);
    setMinimumSize(QSize(iconSize, iconSize));

    connect(m_globalPresence, SIGNAL(currentPresenceChanged(Tp::Presence)), this, SLOT(onPresenceChanged(Tp::Presence)));

    // register plasmoid for tooltip
    Plasma::ToolTipManager::self()->registerWidget(this);
}

TelepathyPresenceApplet::~TelepathyPresenceApplet()
{
    m_contextActions.clear();
}

QList<QAction*> TelepathyPresenceApplet::contextualActions()
{
    return m_contextActions;
}

void TelepathyPresenceApplet::init()
{
    Plasma::Applet::init();
}

void TelepathyPresenceApplet::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}

void TelepathyPresenceApplet::setupAccountManager()
{
    Tp::registerTypes();

    // setup the telepathy account manager from where I'll retrieve info on accounts and contacts
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureCapabilities
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = Tp::ContactFactory::create(Tp::Features()
    << Tp::Contact::FeatureAlias
    << Tp::Contact::FeatureAvatarData
    << Tp::Contact::FeatureSimplePresence
    << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}


void TelepathyPresenceApplet::setupContextMenuActions()
{
    KActionMenu *moreMenu = new KActionMenu(i18n("More"), this);

    KAction *goOnlineAction = new KAction(KIcon("user-online"), i18n("Online"), this);
    KAction *goBusyAction = new KAction(KIcon("user-busy"), i18n("Busy"), this);
    KAction *goAwayAction = new KAction(KIcon("user-away"), i18n("Away"), this);
    KAction *goExtendedAwayAction = new KAction(KIcon("user-away-extended"), i18n("Not Available"), this);
    KAction *goHiddenAction = new KAction(KIcon("user-invisible"), i18n("Invisible"), this);
    KAction *goOfflineAction = new KAction(KIcon("user-offline"), i18n("Offline"), this);

    // application actions
    KAction *showAccountManagerAction = new KAction(KIcon("telepathy-kde"), i18n("Account Manager"), this);
    KAction *showContactListAction = new KAction(KIcon("meeting-attending"), i18n("Contact List"), this);

    // connect actions
    connect(goOnlineAction, SIGNAL(triggered()), this, SLOT(setPresenceOnline()));
    connect(goBusyAction, SIGNAL(triggered()), this, SLOT(setPresenceBusy()));
    connect(goAwayAction, SIGNAL(triggered()), this, SLOT(setPresenceAway()));
    connect(goExtendedAwayAction, SIGNAL(triggered()), this, SLOT(setPresenceXa()));
    connect(goHiddenAction, SIGNAL(triggered()), this, SLOT(setPresenceHidden()));
    connect(goOfflineAction, SIGNAL(triggered()), this, SLOT(setPresenceOffline()));

    connect(showAccountManagerAction, SIGNAL(triggered()), this, SLOT(startAccountManager()));
    connect(showContactListAction, SIGNAL(triggered()), this, SLOT(startContactList()));

    m_contextActions.append(goOnlineAction);
    m_contextActions.append(goBusyAction);
    m_contextActions.append(goAwayAction);
    m_contextActions.append(goExtendedAwayAction);
    m_contextActions.append(goHiddenAction);
    m_contextActions.append(goOfflineAction);
    m_contextActions.append(moreMenu->addSeparator());

    moreMenu->addAction(showAccountManagerAction);
    moreMenu->addAction(showContactListAction);
    m_contextActions.append(moreMenu);

    m_contextActions.append(moreMenu->addSeparator());
}

void TelepathyPresenceApplet::onAccountManagerReady(Tp::PendingOperation* op)
{
    if (op->isError()) {
        kDebug() << op->errorName();
        kDebug() << op->errorMessage();
    }

    // set the manager to the globalpresence
    m_globalPresence->setAccountManager(m_accountManager);
}

void TelepathyPresenceApplet::onPresenceChanged(Tp::Presence presence)
{
    QString presenceStr = presence.status();

    if (presenceStr == "available") {
        setPopupIcon("user-online");
    } else if (presenceStr == "busy" || presenceStr == "dnd") {
        setPopupIcon("user-busy");
    } else if (presenceStr == "away" || presenceStr == "brb") {
        setPopupIcon("user-away");
    } else if (presenceStr == "xa") {
        setPopupIcon("user-away-extended");
    } else if (presenceStr == "hidden") {
        setPopupIcon("user-invisible");
    } else if (presenceStr == "offline") {
        setPopupIcon("user-offline");
    } else {
        setPopupIcon("user-offline");
    }
}

void TelepathyPresenceApplet::setPresenceAway()
{
    m_globalPresence->setPresence(Tp::Presence::away(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::setPresenceBusy()
{
    m_globalPresence->setPresence(Tp::Presence::busy(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::setPresenceHidden()
{
    m_globalPresence->setPresence(Tp::Presence::hidden(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::setPresenceOffline()
{
    m_globalPresence->setPresence(Tp::Presence::offline(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::setPresenceOnline()
{
    m_globalPresence->setPresence(Tp::Presence::available(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::setPresenceXa()
{
    m_globalPresence->setPresence(Tp::Presence::xa(m_globalPresence->currentPresence().statusMessage()));
}

void TelepathyPresenceApplet::startAccountManager() const
{
    KToolInvocation::startServiceByDesktopName("kcm_telepathy_accounts");
}

void TelepathyPresenceApplet::startContactList() const
{
    KToolInvocation::startServiceByDesktopName("telepathy-kde-contactlist");
}

void TelepathyPresenceApplet::toolTipAboutToShow()
{
    Plasma::ToolTipContent content;
    KUser user;

    QString presenceMsg("\"");
    presenceMsg.append(m_globalPresence->currentPresence().statusMessage());
    presenceMsg.append("\"");

    content.setImage(KIcon("telepathy-kde"));
    content.setMainText(user.loginName());
    content.setSubText(presenceMsg);
    Plasma::ToolTipManager::self()->setContent(this, content);
}

void TelepathyPresenceApplet::toolTipHidden()
{
    Plasma::ToolTipManager::self()->clearContent(this);
}



// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(telepathy-kde-presence-applet, TelepathyPresenceApplet)



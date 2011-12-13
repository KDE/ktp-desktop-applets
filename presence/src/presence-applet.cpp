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

    connect(m_globalPresence, SIGNAL(currentPresenceChanged(KTp::Presence)), this, SLOT(onPresenceChanged(KTp::Presence)));

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
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory);

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

    goOnlineAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::available())));
    goBusyAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::busy())));
    goAwayAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::away())));
    goExtendedAwayAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::xa())));
    goHiddenAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::hidden())));
    goOfflineAction->setData(QVariant::fromValue(KTp::Presence(Tp::Presence::offline())));

    // application actions
    KAction *showAccountManagerAction = new KAction(KIcon("telepathy-kde"), i18n("Account Manager"), this);
    KAction *showContactListAction = new KAction(KIcon("meeting-attending"), i18n("Contact List"), this);

    // connect actions
    connect(goOnlineAction,       SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
    connect(goBusyAction,         SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
    connect(goAwayAction,         SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
    connect(goExtendedAwayAction, SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
    connect(goHiddenAction,       SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
    connect(goOfflineAction,      SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));

    connect(showAccountManagerAction, SIGNAL(triggered()), this, SLOT(startAccountManager()));
    connect(showContactListAction, SIGNAL(triggered()), this, SLOT(startContactList()));

    m_contextActions.append(goOnlineAction);
    m_contextActions.append(goBusyAction);
    m_contextActions.append(goAwayAction);
    m_contextActions.append(goExtendedAwayAction);
    m_contextActions.append(goHiddenAction);
    m_contextActions.append(goOfflineAction);

    m_contextActions.append(moreMenu->addSeparator());
    m_contextActions.append(showAccountManagerAction);
    m_contextActions.append(showContactListAction);

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
    setPopupIcon(m_globalPresence->currentPresence().icon());
}

void TelepathyPresenceApplet::onPresenceChanged(KTp::Presence presence)
{
    setPopupIcon(presence.icon());
}

void TelepathyPresenceApplet::onPresenceActionClicked()
{
    KTp::Presence p = qobject_cast<KAction*>(sender())->data().value<KTp::Presence>();
    //TODO: lobby for the setStatusMessage() in tp-qt4
    p.setStatus(p.type(), p.status(), m_globalPresence->currentPresence().statusMessage());

    m_globalPresence->setPresence(p);
    setPopupIcon(p.icon());
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

    QString presenceMsg = QString(m_globalPresence->currentPresence().statusMessage());

    content.setImage(KIcon("telepathy-kde"));
    content.setMainText(user.loginName());

    if (m_globalPresence->isChangingPresence()) {
        content.setSubText(i18n("Connecting..."));
    } else if (!presenceMsg.isEmpty() && !m_globalPresence->isChangingPresence()) {
        content.setSubText(presenceMsg);
    } else if (presenceMsg.isEmpty() && !m_globalPresence->isChangingPresence()) {
        content.setSubText(m_globalPresence->currentPresence().displayString());
    }

    Plasma::ToolTipManager::self()->setContent(this, content);
}

void TelepathyPresenceApplet::toolTipHidden()
{
    Plasma::ToolTipManager::self()->clearContent(this);
}

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(telepathy-kde-presence-applet, TelepathyPresenceApplet)

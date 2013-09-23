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

#include "presence-applet.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QGraphicsLinearLayout>

#include <KAction>
#include <KActionMenu>
#include <KConfigDialog>
#include <KConfigGroup>
#include <KToolInvocation>
#include <KUser>
#include <KMessageBox>
#include <KStandardDirs>

#include <KTp/actions.h>
#include <KTp/global-presence.h>
#include <KTp/Widgets/add-contact-dialog.h>
#include <KTp/Widgets/join-chat-room-dialog.h>

#include <Plasma/ToolTipManager>
#include <Plasma/Svg>

#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingContacts>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/Account>

int TelepathyPresenceApplet::s_instanceCount = 0;

TelepathyPresenceApplet::TelepathyPresenceApplet(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
      m_globalPresence(new KTp::GlobalPresence(this))
{
    s_instanceCount ++;
    setupContextMenuActions();

    setAspectRatioMode(Plasma::ConstrainedSquare);
    setBackgroundHints(NoBackground);
    resize(150, 150);

    m_contactListRunning = false;

    //find out if contact list is already running
    QDBusPendingCall async = QDBusConnection::sessionBus().interface()->asyncCall(QLatin1String("NameHasOwner"),
                                                                                  QLatin1String("org.kde.ktp-contactlist"));

    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(async, this);
    connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(serviceNameFetchFinished(QDBusPendingCallWatcher*)));

    m_icon = new Plasma::IconWidget(this);
    connect(m_icon, SIGNAL(clicked()), this, SLOT(toggleContactList()));

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->setOrientation(Qt::Horizontal);
    layout->addItem(m_icon);
    layout->setAlignment(m_icon, Qt::AlignCenter);
    setLayout(layout);

    if (formFactor() == Plasma::Planar) {
        int iconSize = IconSize(KIconLoader::Small);
        setMinimumSize(QSize(iconSize, iconSize));
    }

    connect(m_globalPresence, SIGNAL(currentPresenceChanged(KTp::Presence)), SLOT(onPresenceChanged(KTp::Presence)));
    onPresenceChanged(m_globalPresence->currentPresence());

    connect(m_globalPresence, SIGNAL(connectionStatusChanged(Tp::ConnectionStatus)), SLOT(onConnectionStatusChanged(Tp::ConnectionStatus)));
    onConnectionStatusChanged(m_globalPresence->connectionStatus());


    setStatus(Plasma::PassiveStatus);

    // register plasmoid for tooltip
    Plasma::ToolTipManager::self()->registerWidget(this);
}

TelepathyPresenceApplet::~TelepathyPresenceApplet()
{
    m_contextActions.clear();

    s_instanceCount--;
    if (s_instanceCount == 0) {
        QDBusConnection::sessionBus().unregisterService("org.kde.Telepathy.PresenceAppletActive");
    }
}

QList<QAction*> TelepathyPresenceApplet::contextualActions()
{
    return m_contextActions;
}

void TelepathyPresenceApplet::init()
{
    QDBusConnection::sessionBus().registerService("org.kde.Telepathy.PresenceAppletActive");
    QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSlots);

    Tp::registerTypes();

    // setup the telepathy account manager from where I'll retrieve info on accounts and contacts
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile
                                                                       << Tp::Account::FeatureCapabilities);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory);

    connect(m_accountManager.data(), SIGNAL(newAccount(Tp::AccountPtr)), SLOT(onAccountsChanged()));
    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(onAccountManagerReady(Tp::PendingOperation*)));
    connect(this, SIGNAL(activate()), this, SLOT(toggleContactList()));

}

KIcon TelepathyPresenceApplet::getThemedIcon(const QString &iconBaseName) const
{
    Plasma::Svg svgIcon;
    svgIcon.setImagePath("icons/presence-applet");

    QString iconPlasmaName = iconBaseName + "-plasma";

    if (svgIcon.hasElement(iconPlasmaName)) {
        svgIcon.resize(150, 150);
        return KIcon(svgIcon.pixmap(iconPlasmaName));
    } else {
        return KIcon(iconBaseName);
    }
}

int TelepathyPresenceApplet::handleCustomPresenceChange() {
    setupContextMenuActions();
    return 0;
}

void TelepathyPresenceApplet::addPresence(const KTp::Presence &presence)
{
    if(m_presences.contains(presence)) {
        return;
    }

    //Add presence to correct place in list, sorted by presence type
    QList<KTp::Presence>::iterator i = qLowerBound(m_presences.begin(), m_presences.end(), KTp::Presence(presence));
    m_presences.insert(i, presence);
}

void TelepathyPresenceApplet::setupContextMenuActions()
{
    m_contextActions.clear();
    m_presences.clear();

    KActionMenu *moreMenu = new KActionMenu(i18n("More"), this);

    KSharedConfigPtr config = KSharedConfig::openConfig("ktelepathyrc");
    KConfigGroup m_presenceGroup = config->group("Custom Presence List");

    //add default presences
    addPresence(Tp::Presence::available());
    addPresence(Tp::Presence::busy());
    addPresence(Tp::Presence::away());
    addPresence(Tp::Presence::xa());
    addPresence(Tp::Presence::hidden());
    addPresence(Tp::Presence::offline());

    //add custom presences
    Q_FOREACH(const QString &key, m_presenceGroup.keyList()) {
        QVariantList entry = m_presenceGroup.readEntry(key, QVariantList());

        if(entry.size()!=2)
          continue;

        QString statusMessage = entry.last().toString();

        switch (entry.first().toInt()) {
            case Tp::ConnectionPresenceTypeAvailable:
                addPresence(Tp::Presence::available(statusMessage));
                break;
            case Tp::ConnectionPresenceTypeAway:
                addPresence(Tp::Presence::away(statusMessage));
                break;
            case Tp::ConnectionPresenceTypeBusy:
                addPresence(Tp::Presence::busy(statusMessage));
                break;
            case Tp::ConnectionPresenceTypeExtendedAway:
                addPresence(Tp::Presence::xa(statusMessage));
        }
    }

    //This loops through the all presences and creates a menu, connects to slot and appends it to the context menu
    Q_FOREACH(const KTp::Presence &presence, m_presences) {
        KAction *action;
        QString menuentry, icon;
        switch (presence.type()) {
            case Tp::ConnectionPresenceTypeAvailable:
                menuentry = (presence.statusMessage()=="") ? i18n("Online") : presence.statusMessage();
                icon = "user-online";
                break;
            case Tp::ConnectionPresenceTypeBusy:
                menuentry = (presence.statusMessage()=="") ? i18n("Busy") : presence.statusMessage();
                icon = "user-busy";
                break;
            case Tp::ConnectionPresenceTypeAway:
                menuentry = (presence.statusMessage()=="") ? i18n("Away") : presence.statusMessage();
                icon = "user-away";
                break;
            case Tp::ConnectionPresenceTypeExtendedAway:
                menuentry = (presence.statusMessage()=="") ? i18n("Not Available") : presence.statusMessage();
                icon = "user-away-extended";
                break;
            case Tp::ConnectionPresenceTypeHidden:
                menuentry = (presence.statusMessage()=="") ? i18n("Invisible") : presence.statusMessage();
                icon = "user-invisible";
                break;
            case Tp::ConnectionPresenceTypeOffline:
                menuentry = (presence.statusMessage()=="") ? i18n("Offline") : presence.statusMessage();
                icon = "user-offline";
                break;
            default: continue;
        }
        action = new KAction(getThemedIcon(icon), menuentry, this);
        action->setData(QVariant::fromValue(presence));
        connect(action, SIGNAL(triggered()), this, SLOT(onPresenceActionClicked()));
        m_contextActions.append(action);
    }

    KAction *joinChatroomAction = new KAction(KIcon("user-group-new"), i18n("Join Chat Room..."), this);

    // application actions
    KAction *showAccountManagerAction = new KAction(KIcon("telepathy-kde"), i18n("Account Manager..."), this);
    KAction *showContactListAction = new KAction(KIcon("meeting-attending"), i18n("Contact List..."), this);
    KAction *addContactAction = new KAction(KIcon("list-add-user"), i18n("Add New Contacts..."), this);
    KAction *makeCallAction = 0;
    if (!KStandardDirs::findExe(QLatin1String("ktp-dialout-ui")).isEmpty()) {
        makeCallAction = new KAction(KIcon("internet-telephony"), i18n("Make a Call..."), this);
    }
    KAction *sendFileAction = 0;
    if (!KStandardDirs::findExe(QLatin1String("ktp-send-file")).isEmpty()) {
        sendFileAction = new KAction(KIcon("mail-attachment"), i18n("Send a File..."), this);
    }

    // connect actions
    connect(showAccountManagerAction, SIGNAL(triggered()), this, SLOT(startAccountManager()));
    connect(showContactListAction, SIGNAL(triggered()), this, SLOT(toggleContactList()));
    connect(addContactAction, SIGNAL(triggered()), this, SLOT(onAddContactRequest()));
    connect(joinChatroomAction, SIGNAL(triggered()), this, SLOT(onJoinChatRoomRequest()));
    if (makeCallAction) {
        connect(makeCallAction, SIGNAL(triggered()), this, SLOT(onMakeCallRequest()));
    }
    if (sendFileAction) {
        connect(sendFileAction, SIGNAL(triggered()), this, SLOT(onSendFileRequest()));
    }

    m_contextActions.append(moreMenu->addSeparator());
    m_contextActions.append(showAccountManagerAction);
    m_contextActions.append(showContactListAction);

    m_contextActions.append(moreMenu->addSeparator());
    m_contextActions.append(addContactAction);
    m_contextActions.append(joinChatroomAction);
    if (makeCallAction) {
        m_contextActions.append(makeCallAction);
    }
    if (sendFileAction) {
        m_contextActions.append(sendFileAction);
    }
    m_contextActions.append(moreMenu->addSeparator());
}

void TelepathyPresenceApplet::onAccountManagerReady(Tp::PendingOperation* op)
{
    if (op->isError()) {
        kDebug() << op->errorName();
        kDebug() << op->errorMessage();
    }

    onAccountsChanged();

    // set the manager to the globalpresence
    m_globalPresence->setAccountManager(m_accountManager);
}

void TelepathyPresenceApplet::onAccountsChanged()
{
    //if connection to MC failed, or user has no accounts, hide presence icon.
    if (m_accountManager->isValid() && m_accountManager->allAccounts().size() > 0) {
        setStatus(Plasma::ActiveStatus);
    } else {
        setStatus(Plasma::PassiveStatus);
    }
}

void TelepathyPresenceApplet::startAccountManager()
{
    KToolInvocation::startServiceByDesktopName("kcm_ktp_accounts");
}

void TelepathyPresenceApplet::toggleContactList()
{
    if (!m_contactListRunning) {
        KToolInvocation::startServiceByDesktopName(QLatin1String("ktp-contactlist"));
    } else {
        //contact list is registered, call toggleWindowVisibility in contact list
        QDBusMessage methodCall = QDBusMessage::createMethodCall(QLatin1String("org.kde.ktp-contactlist"),
                                                                 QLatin1String("/ktp_contactlist/MainWindow"),
                                                                 QLatin1String("org.kde.KTp.ContactList"),
                                                                 QLatin1String("toggleWindowVisibility"));

        QDBusConnection::sessionBus().asyncCall(methodCall);
    }
}

void TelepathyPresenceApplet::onAddContactRequest()
{
    KTp::AddContactDialog *dialog = new KTp::AddContactDialog(m_accountManager);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void TelepathyPresenceApplet::onJoinChatRoomRequest()
{
    KTp::JoinChatRoomDialog *dialog = new KTp::JoinChatRoomDialog(m_accountManager);
    connect(dialog, SIGNAL(accepted()), this, SLOT(onJoinChatRoomSelected()));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

void TelepathyPresenceApplet::onJoinChatRoomSelected()
{
    KTp::JoinChatRoomDialog *dialog = qobject_cast<KTp::JoinChatRoomDialog*>(sender());
    if (!dialog) {
        return;
    }

    Tp::AccountPtr account = dialog->selectedAccount();
    // check account validity. Should NEVER be invalid
    if (!account.isNull()) {
        Tp::PendingChannelRequest *channelRequest = KTp::Actions::startGroupChat(account, dialog->selectedChatRoom());

        connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onGenericOperationFinished(Tp::PendingOperation*)));
    }
}

void TelepathyPresenceApplet::onMakeCallRequest()
{
    KToolInvocation::kdeinitExec(QLatin1String("ktp-dialout-ui"));
}

void TelepathyPresenceApplet::onSendFileRequest()
{
    KToolInvocation::kdeinitExec(QLatin1String("ktp-send-file"));
}

void TelepathyPresenceApplet::onPresenceChanged(KTp::Presence presence)
{
    QString iconBaseName = presence.iconName(false);

    Plasma::Svg svgIcon;
    svgIcon.setImagePath("icons/presence-applet");
    if (svgIcon.hasElement(iconBaseName+"-plasma")) {
        svgIcon.resize(150,150);
        KIcon icon = KIcon(svgIcon.pixmap(iconBaseName+"-plasma"));
        m_icon->setIcon(icon);
    } else {
        m_icon->setIcon(presence.icon());
    }
}

void TelepathyPresenceApplet::onConnectionStatusChanged(Tp::ConnectionStatus connectionStatus)
{
    if (connectionStatus == Tp::ConnectionStatusConnecting) {
        setBusy(true);
    } else {
        setBusy(false);
    }
}

void TelepathyPresenceApplet::onPresenceActionClicked()
{
    KTp::Presence p = qobject_cast<KAction*>(sender())->data().value<KTp::Presence>();
    p.setStatus(p.type(), p.status(), p.statusMessage());

    m_globalPresence->setPresence(p);
}

void TelepathyPresenceApplet::toolTipAboutToShow()
{
    Plasma::ToolTipContent content;
    KUser user;

    QString presenceMsg = QString(m_globalPresence->currentPresence().statusMessage());

    content.setImage(KIcon("telepathy-kde"));
    content.setMainText(user.loginName());

    if (m_globalPresence->connectionStatus() == Tp::ConnectionStatusConnecting) {
        content.setSubText(i18n("Connecting..."));
    } else if (!presenceMsg.isEmpty()) {
        content.setSubText(presenceMsg);
    } else {
        content.setSubText(m_globalPresence->currentPresence().displayString());
    }

    Plasma::ToolTipManager::self()->setContent(this, content);
}

void TelepathyPresenceApplet::toolTipHidden()
{
    Plasma::ToolTipManager::self()->clearContent(this);
}

void TelepathyPresenceApplet::serviceNameFetchFinished(QDBusPendingCallWatcher *callWatcher)
{
    QDBusPendingReply<bool> reply = *callWatcher;
    if (reply.isError()) {
        kWarning() << reply.error();
        return;
    }

    m_contactListRunning = reply.value();

    callWatcher->deleteLater();

    //start watching for contact list appearing on the bus
    m_contactListWatcher = new QDBusServiceWatcher(QLatin1String("org.kde.ktp-contactlist"),
                                                   QDBusConnection::sessionBus(),
                                                   QDBusServiceWatcher::WatchForRegistration
                                                       | QDBusServiceWatcher::WatchForUnregistration,
                                                   this);

    connect(m_contactListWatcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(contactListServiceRegistered()));

    connect(m_contactListWatcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(contactListServiceUnregistered()));
}

void TelepathyPresenceApplet::contactListServiceRegistered()
{
    m_contactListRunning = true;
}

void TelepathyPresenceApplet::contactListServiceUnregistered()
{
    m_contactListRunning = false;
}

#include "presence-applet.moc"

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(org.kde.ktp-presence, TelepathyPresenceApplet)



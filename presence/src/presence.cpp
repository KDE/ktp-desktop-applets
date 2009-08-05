/*
 * Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 * Copyright (C) 2009 Collabora Ltd <http://www.collabora.co.uk>
 * Copyright (C) 2009 Andre Moreira Magalhaes <andrunko@gmail.com>
 * Copyright (C) 2009 Abner Silva <abner.silva@kdemail.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Own
#include "presence.h"
#include "accountwidget.h"
#include "globalpresencewidget.h"

// Plasma
#include <Plasma/Theme>
#include <Plasma/Service>
#include <Plasma/Extender>
#include <Plasma/ExtenderItem>

// Kde
#include <KColorScheme>
#include <KDebug>

//Qt
#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QLabel>

using namespace Plasma;

PresenceApplet::PresenceApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args),
      m_engine(0),
      m_widget(0),
      m_layout(0),
      m_colorScheme(0),
      m_global(0),
      m_globalWidget(0),
      m_globalLayout(0)
{
    KGlobal::locale()->insertCatalog("plasma_applet_presence");
    setBackgroundHints(StandardBackground);
    setAspectRatioMode(IgnoreAspectRatio);
    //setHasConfigurationInterface(true);
    setPassivePopup(false);

    setPopupIcon("user-offline");
}

PresenceApplet::~PresenceApplet()
{
    delete m_colorScheme;
}

void PresenceApplet::init()
{
    kDebug() << "PresenceApplet::init: initializing applet";

    // Set up the color scheme.
    m_colorScheme = new KColorScheme(QPalette::Active,
            KColorScheme::View,
            Plasma::Theme::defaultTheme()->colorScheme());

    // Create a new extender item for global presence
    if (!extender()->hasItem("Global")) {
        ExtenderItem *item = new ExtenderItem(extender());
        item->setName("Global");
        initExtenderItem(item);
    }

    // Create a new extender item for accounts
    if (!extender()->hasItem("Accounts")) {
        ExtenderItem *item = new ExtenderItem(extender());
        item->setName("Accounts");
        initExtenderItem(item);
    }

    // Set up the data engine
    m_engine = dataEngine("presence");

    QStringList sources = m_engine->sources();
    foreach(const QString &source, sources)
        onSourceAdded(source);

    connect(m_engine,
            SIGNAL(sourceAdded(const QString &)),
            SLOT(onSourceAdded(const QString &)));
    connect(m_engine,
            SIGNAL(sourceRemoved(const QString &)),
            SLOT(onSourceRemoved(const QString &)));
}

void PresenceApplet::initExtenderItem(Plasma::ExtenderItem *item)
{
    // Set up global presence
    if (item->name() == "Global") {
        m_globalWidget = new QGraphicsWidget(item);
        m_globalLayout = new QGraphicsLinearLayout(Qt::Vertical, m_globalWidget);
        m_globalLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_globalWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_globalWidget->setLayout (m_globalLayout);

        m_global = new GlobalPresenceWidget(this);

        // Listening for global changes
        connect(m_global, SIGNAL(presenceStatusChanged(const QString&)),
                this, SLOT(onGlobalPresenceStatusChanged(const QString&)));
        connect(m_global, SIGNAL(presenceMessageChanged(const QString&)),
                this, SLOT(onGlobalPresenceMessageChanged(const QString&)));

        m_globalLayout->addItem(m_global);
        item->setWidget(m_globalWidget);
        item->setTitle(i18n("Global Presence"));
    }

    // Set up user accounts
    if (item->name() == "Accounts") {
        m_widget = new QGraphicsWidget(item);
        m_layout = new QGraphicsLinearLayout(Qt::Vertical, m_widget);
        m_layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_widget->setLayout (m_layout);

        item->setWidget(m_widget);
        item->setTitle(i18n("User Accounts"));
    }
}

void PresenceApplet::onSourceAdded(const QString &source)
{
    kDebug() << "PresenceApplet::onSourceAdded: source:" << source;

    if (!m_accounts.contains(source)) {
        AccountWidget *account = new AccountWidget(this);
        account->setId(source);
        connect(account, SIGNAL(presenceChanged(const QString&, const QString&)),
                this, SLOT(onPresenceChanged(const QString&, const QString&)));
        m_accounts[source] = account;
        m_engine->connectSource(source, this);
        m_layout->addItem(account);

        updateConstraints();
        updateSize();
    }
}

void PresenceApplet::onSourceRemoved(const QString &source)
{
    kDebug() << "PresenceApplet::onSourceAdded: source:" << source;

    if (m_accounts.contains(source)) {
        AccountWidget *account = m_accounts.value(source);
        m_accounts.remove(source);
        m_layout->removeItem(account);
        delete account;
        m_engine->disconnectSource(source, this);

        updateConstraints();
        updateSize();
    }
}

void PresenceApplet::dataUpdated(const QString &source,
        const Plasma::DataEngine::Data &data)
{
    kDebug() << "PresenceApplet::onDataUpdated: source:" << source;

    if (m_accounts.contains(source)) {
        // Get the stored account
        AccountWidget *account = m_accounts[source];

        // Set the DisplayName
        account->setName(data["DisplayName"].toString());

        // Set the Nickname
        account->setAlias(data["Nickname"].toString());

        // Set the account avatar
        QPixmap avatar;
        avatar.loadFromData(data["AccountAvatar"].toByteArray());
        account->setImage(avatar);

        // Set the presence and presence msg
        account->setPresence(data["PresenceType"].toString(),
                data["PresenceStatusMessage"].toString());

        // Everytime data is updated it's also needed to update
        // the global presence.
        updateMasterPresence();
    }
}

/*
 * Update the master presence state.
 *
 * We must check what presence state each account is in and use that to decide
 * which icon we display. We must also check the presence message for each
 * account to see if we can display one overall presence message.
 */
void PresenceApplet::updateMasterPresence()
{
    QString accountMessage;
    bool sameMessage = false;

    // Next, we work out the overall presence status.
    int accountsOffline = 0;
    int accountsAvailable = 0;
    int accountsAway = 0;
    //int accountsExtendedAway = 0;
    int accountsHidden = 0;
    int accountsBusy = 0;

    bool okOffline = true;
    bool okAway = true;
    bool okExtendedAway = true;
    bool okHidden = true;
    bool okBusy = true;

    // Iterate over all the accounts in the model, and total up how many are
    // in each type of presence state.
    foreach (AccountWidget *account, m_accounts.values())
    {
        QString status_type = account->presenceStatus();

        if ((status_type == "offline") || (status_type == "unknown") ||
            (status_type == "error") || (status_type == "unset")) {
            accountsOffline++;
        } else if (status_type == "available") {
            accountsAvailable++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okAway = false;
            okBusy = false;
        } else if (status_type == "away") {
            accountsAway++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okBusy = false;
        }
/*        else if (status_type == "xa") {
            accountsExtendedAway++;
            okOffline = false;
            okHidden = false;
            okBusy = false;
        }*/ else if (status_type == "invisible") {
            accountsHidden++;
            okOffline = false;
            okExtendedAway = false;
            okAway = false;
            okBusy = false;
        } else if(status_type == "busy") {
            accountsBusy++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okAway = false;
        }

        sameMessage = (accountMessage == account->presenceMessage());
        accountMessage = account->presenceMessage();
    }

    // Chose a master presence state from this.
    // FIXME: What should be the logic for choosing a master presence state?
    //        Should this be user customisable?
    //        Currently follows the kopete approach.
    QString popupIcon;
    if (okOffline == true) {
        m_global->setPresenceStatus("offline");
        popupIcon = "user-offline";
    } else if(okHidden == true) {
        m_global->setPresenceStatus("invisible");
        popupIcon = "user-invisible";
    } else if(okBusy == true) {
        m_global->setPresenceStatus("busy");
        popupIcon = "user-busy";
//    } else if(okExtendedAway == true) {
//        m_globalWidget->setPresenceStatus("offline");
    } else if(okAway == true) {
        m_global->setPresenceStatus("away");
        popupIcon = "user-away";
    } else {
        m_global->setPresenceStatus("available");
        popupIcon = "user-online";
    }

    setPopupIcon(popupIcon);

    if (sameMessage)
        m_global->setPresenceMessage(accountMessage);
}

void PresenceApplet::onPresenceChanged(const QString &presence,
        const QString &msg)
{
    AccountWidget *account = static_cast<AccountWidget *>(sender());

    Q_ASSERT(account);

    setSourcePresence(account->id(), presence, msg);
}

void PresenceApplet::onJobCompleted()
{
    Plasma::Service *service = static_cast<Plasma::Service *>(sender());

    if (service)
        service->deleteLater();
}

void PresenceApplet::onGlobalPresenceStatusChanged(const QString &status)
{
    foreach (AccountWidget *account, m_accounts.values())
        setSourcePresence(account->id(), status, account->presenceMessage());
}

void PresenceApplet::onGlobalPresenceMessageChanged(const QString &msg)
{
    foreach (AccountWidget *account, m_accounts.values())
        setSourcePresence(account->id(), account->presenceStatus(), msg);
}

void PresenceApplet::updateSize()
{
    ExtenderItem *item = extender()->item("Accounts");

    item->setMinimumSize(m_widget->minimumSize());
    item->resize(m_widget->contentsRect().size().toSize());
    item->adjustSize();
}

void PresenceApplet::setSourcePresence(const QString &id, const QString &status,
        const QString &msg)
{
    Plasma::Service *service = m_engine->serviceForSource(id);

    if (service != NULL) {
        KConfigGroup op = service->operationDescription("setPresence");
        op.writeEntry("status", status);
        op.writeEntry("status_message", msg);
        connect(service, SIGNAL(finished(Plasma::ServiceJob *)),
                this, SLOT(onJobCompleted()));
        service->startOperationCall(op);
    }
}

#include "presence.moc"


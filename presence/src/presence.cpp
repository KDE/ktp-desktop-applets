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

// Plasma
#include <Plasma/Theme>
#include <Plasma/Service>

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
      m_colorScheme(0)
{
    KGlobal::locale()->insertCatalog("plasma_applet_presence");
    setBackgroundHints(StandardBackground);
    setAspectRatioMode(IgnoreAspectRatio);
    //setHasConfigurationInterface(true);
    setPassivePopup(false);

    setPopupIcon("user-offline");

    (void) graphicsWidget();
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

QGraphicsWidget *PresenceApplet::graphicsWidget()
{
    if (!m_widget) {
        m_widget = new QGraphicsWidget(this);
        m_layout = new QGraphicsLinearLayout(Qt::Vertical, m_widget);
        m_layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_widget->setLayout (m_layout);
    }

    return m_widget;
}

void PresenceApplet::onSourceAdded(const QString &source)
{
    kDebug() << "PresenceApplet::onSourceAdded: source:" << source;

    if (!m_accounts.contains(source)) {
        AccountWidget *account = new AccountWidget();
        account->setId(source);
        connect(account, SIGNAL(presenceChanged(const QString&, const QString&)),
                this, SLOT(onPresenceChanged(const QString&, const QString&)));
        m_accounts[source] = account;
        m_engine->connectSource(source, this);
        m_layout->addItem(account);

        updateConstraints();
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
                data["PresenceMessage"].toString());
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
/*    // Get data we can use to iterate over the contents of the accounts model.
    int rowCount = m_accountsModel->rowCount();

    // First we workout the overall presence message.
    QStringList statusMessages;
    for (int i = 0; i < rowCount; ++i) {
        statusMessages <<
            m_accountsModel->data(m_accountsModel->index(i, 3)).toString();
    }

    bool statusMessagesAllTheSame = true;
    QString previousStatusMessage = statusMessages.at(0);
    foreach (const QString &statusMessage, statusMessages) {
        if (statusMessage == previousStatusMessage) {
            continue;
        } else {
            statusMessagesAllTheSame = false;
            break;
        }
    }

    if (statusMessagesAllTheSame) {
        setMasterStatusMessage(previousStatusMessage);
    }
    else {
        setMasterStatusMessage(QString());
    }

    // Next, we work out the overall presence status.
    int accountsOffline = 0;
    int accountsAvailable = 0;
    int accountsAway = 0;
    int accountsExtendedAway = 0;
    int accountsHidden = 0;
    int accountsBusy = 0;

    bool okOffline = true;
    bool okAway = true;
    bool okExtendedAway = true;
    bool okHidden = true;
    bool okBusy = true;

    // Iterate over all the accounts in the model, and total up how many are
    // in each type of presence state.
    for (int i = 0; i < rowCount; i++)
    {
        QString status_type =
            m_accountsModel->data(m_accountsModel->index(i, 1)).toString();

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
        else if (status_type == "xa") {
            accountsExtendedAway++;
            okOffline = false;
            okHidden = false;
            okBusy = false;
        } else if (status_type == "invisible") {
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
    }

    // Chose a master presence state from this.
    // FIXME: What should be the logic for choosing a master presence state?
    //        Should this be user customisable?
    //        Currently follows the kopete approach.
    if (okOffline == true) {
        m_icon->setIcon(KIcon("user-offline"));
    } else if(okHidden == true) {
        m_icon->setIcon(KIcon("user-invisible"));
    } else if(okBusy == true) {
        m_icon->setIcon(KIcon("user-busy"));
    } else if(okExtendedAway == true) {
        m_icon->setIcon(KIcon("user-away-extended"));
    } else if(okAway == true) {
        m_icon->setIcon(KIcon("user-away"));
    } else {
        m_icon->setIcon(KIcon("user-online"));
    }

    // call the method to update the masterPresenceIcon
    setPopupIcon(m_icon->icon());
    updateMasterIcon();*/
}

void PresenceApplet::updateMasterIcon()
{
    // The icon has been changed. We must update the pixmap of the icon for
    // display in the main widget.
/*    if (m_masterIconLabel) {
        m_masterIconLabel->setPixmap(m_icon->icon().pixmap(QSize(32, 32)));
    }*/
}

void PresenceApplet::setMasterStatusMessage(const QString & message)
{
    Q_UNUSED(message);
/*    // If m_masterStatusMessageLabel points to a valid QLabel, update it
    if (m_masterStatusMessageLabel) {
        m_masterStatusMessageLabel->setText(message);
    }

    // Store the master presence message as a member var
    m_masterStatusMessage = message;*/
}

void PresenceApplet::onPresenceChanged(const QString &presence,
        const QString &msg)
{
    Q_UNUSED(msg);

    AccountWidget *account = static_cast<AccountWidget *>(sender());

    Q_ASSERT(account);

    Plasma::Service *service = m_engine->serviceForSource(account->id());

    if (service != NULL) {
        KConfigGroup op = service->operationDescription("setPresence");
        op.writeEntry("status", presence);
        connect(service, SIGNAL(finished(Plasma::ServiceJob *)),
                this, SLOT(onJobCompleted()));
        service->startOperationCall(op);
    }
}

void PresenceApplet::onJobCompleted()
{
    Plasma::Service *service = static_cast<Plasma::Service *>(sender());

    if (service)
        service->deleteLater();
}
#include "presence.moc"


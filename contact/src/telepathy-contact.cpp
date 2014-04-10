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

#include "telepathy-contact.h"
#include "contact-wrapper.h"

#include <KConfig>
#include <KStandardDirs>

#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include <QtGui/QPainter>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/Types>

#include <KTp/contact-factory.h>
#include <KTp/Widgets/contact-grid-dialog.h>
#include <KTp/Models/contacts-filter-model.h>

TelepathyContact::TelepathyContact(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_declarative(new Plasma::DeclarativeWidget(this))
    , m_contactWrapper(new ContactWrapper(parent))
{
    // setup plasmoid
    resize(128, 128);
    setBackgroundHints(NoBackground);

    // user shouldn't be able to resize the plasmoid
    setAspectRatioMode(Plasma::FixedSize);

    // prepare account manager
    setupAccountManager();

    if (args.length() == 1) {
        m_fileToLoad = args.first().toString();
    }
}

TelepathyContact::~TelepathyContact()
{
    m_declarative->deleteLater();
//     delete m_contact;
//     delete m_qmlObject;
}

void TelepathyContact::init()
{
    Plasma::Applet::init();

    kDebug() << "APPLET ID: " << id();

    if (m_declarative) {
        QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.ktp-contact/contents/ui/main.qml");
        kDebug() << "LOADING: " << qmlFile;
        m_declarative->engine()->rootContext()->setContextProperty("TelepathyContact", m_contactWrapper);
        m_declarative->setQmlPath(qmlFile);
    }
}

void TelepathyContact::loadConfig()
{
    KConfigGroup group = Plasma::Applet::config();

    QString contactId;
    QString relatedAcc;
    QString tempAvatar;

    //if provided with a file to load data from.
    //otherwise load from configs
    if (m_fileToLoad.isEmpty()) {
        contactId = group.readEntry("id", QString());
        relatedAcc = group.readEntry("relatedAccount", QString());
        tempAvatar = group.readEntry("tempAvatar", QString());
    } else {
        QFile file(m_fileToLoad);
        if (file.open(QFile::ReadOnly)) {
            QDataStream ds(&file);
            ds >> contactId;
            ds >> relatedAcc;
            file.close();
        }
    }

    if (!contactId.isEmpty() && !relatedAcc.isEmpty()) {
        Tp::AccountPtr account = m_accountManager->accountForObjectPath(relatedAcc);
        Tp::ContactPtr contact;

        // check on account. Shouldn't ever be invalid
        if (!account || !account->isValidAccount()) {
            /// TODO add error popup box?
            kWarning() << "Invalid account";
            return;
        }

        if (account->connection()) {
            QList<Tp::ContactPtr> contactList = account->connection()->contactManager()->allKnownContacts().toList();

            for (int i = 0; i < contactList.count(); ++i) {
                if (contactList.at(i)->id() == contactId) {
                    contact = contactList.at(i);
                    m_contactWrapper->setContact(contact);
                    m_contactWrapper->setAccount(account);
                }
            }
        } else {
            // just load cached avatar image
            m_contactWrapper->setTempAvatar(tempAvatar);
            m_contactWrapper->setTempContactId(contactId);

            // just set account. When this will go online it will automatically load the contact pointer
            // shown in the plasmoid
            m_contactWrapper->setAccount(account);
        }
    }
}

void TelepathyContact::onAccountManagerReady(Tp::PendingOperation* op)
{
    if (op->isError()) {
        kDebug() << op->errorName();
        kDebug() << op->errorMessage();
    }

    // now I can load config
    loadConfig();
}

void TelepathyContact::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}

void TelepathyContact::saveConfig()
{
    KConfigGroup group = Plasma::Applet::config();
    group.writeEntry("id", m_contactWrapper->contact()->id());
    group.writeEntry("tempAvatar", m_contactWrapper->contact()->avatarData().fileName);
    group.writeEntry("relatedAccount", m_accountPath);
    group.sync();

    // update contactWrapper temp id
    m_contactWrapper->setTempContactId(m_contactWrapper->contact()->id());

    // tell plasmoid to save config
    configNeedsSaving();
}

void TelepathyContact::setupAccountManager()
{
    Tp::registerTypes();

    // setup the telepathy account manager from where I'll retrieve info on accounts and contacts
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureCapabilities);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureSelfContact
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster);

    Tp::ContactFactoryPtr contactFactory = KTp::ContactFactory::create(Tp::Features()
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

void TelepathyContact::setContact(const Tp::ContactPtr& newContact, const Tp::AccountPtr &relatedAccount)
{
    Q_ASSERT(newContact);
    Q_ASSERT(relatedAccount);

    if (!m_contactWrapper->contact() || m_contactWrapper->contact()->id() != newContact->id()) {
        m_contactWrapper->setContact(newContact);
        m_contactWrapper->setAccount(relatedAccount);
        m_accountPath = relatedAccount->objectPath();
    }

    saveConfig();
}

void TelepathyContact::showConfigurationInterface()
{
    if (!isUserConfiguring()) {
        KTp::ContactGridDialog *dlg = new KTp::ContactGridDialog(0);
        dlg->filter()->setPresenceTypeFilterFlags(KTp::ContactsFilterModel::ShowAll);

        connect(dlg, SIGNAL(accepted()), this, SLOT(configurationAccepted()));
        connect(dlg, SIGNAL(rejected()), dlg, SLOT(deleteLater()));
        dlg->show();
    }
}

void TelepathyContact::configurationAccepted()
{
    KTp::ContactGridDialog *dlg = qobject_cast<KTp::ContactGridDialog*>(sender());
    Q_ASSERT(dlg);

    setContact(dlg->contact(), dlg->account());

    dlg->deleteLater();
}


// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(org.kde.ktp-contact, TelepathyContact)

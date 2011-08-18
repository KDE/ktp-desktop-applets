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

#include "config.h"
#include "models/accounts-model.h"
#include "models/groups-model.h"
#include "models/accounts-filter-model.h"

#include <KDebug>
#include <KPushButton>

#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Types>

Config::Config(QWidget* parent)
    : KDialog(parent)
    , m_model(0)
    , m_modelFilter(0)
    , m_groupsModel(0)
{
    QWidget *widget = new QWidget(this);

    ui.setupUi(widget);
    setMainWidget(widget);

    Tp::registerTypes();
//     Tp::enableDebug(KCmdLineArgs::parsedArgs()->isSet("debug"));
//     Tp::enableWarnings(true);

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

    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

Config::~Config()
{
}

void Config::activateOkButton()
{
    button(Ok)->setEnabled(true);
}

void Config::enableGroupsView(bool enable)
{
    if (enable) {
        m_modelFilter->setSourceModel(m_groupsModel);
    } else {
        m_modelFilter->setSourceModel(m_model);
    }
}

void Config::enableOfflineContacts(bool enable)
{
    m_modelFilter->showOfflineUsers(enable);
}

void Config::onAccountManagerReady(Tp::PendingOperation* op)
{
    if (op->isError()) {
        kDebug() << op->errorName();
        kDebug() << op->errorMessage();
    }

    m_model = new AccountsModel(m_accountManager, this);
    m_groupsModel = new GroupsModel(m_model, this);
    m_modelFilter = new AccountsFilterModel(this);

    // setup model visualization type
    if (ui.showGroups->isChecked()) {
        m_modelFilter->setSourceModel(m_groupsModel);
    } else {
        m_modelFilter->setSourceModel(m_model);
    }

    m_modelFilter->setDynamicSortFilter(true);
    m_modelFilter->showOfflineUsers(ui.showOfflineContacts->isChecked());
    m_modelFilter->setSortRole(Qt::DisplayRole);
    m_modelFilter->setSortByPresence(true);

//     // search input
//     m_modelFilter->clearFilterString();
//     m_modelFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);

    setupContactsList();
}

void Config::setupContactsList()
{
    // set model for the contacts tree view
    ui.contactsList->setModel(m_modelFilter);

    // disable ok button until a list item is selected
    button(Ok)->setEnabled(false);

    ui.contactsList->setExpandsOnDoubleClick(false);
    ui.contactsList->setSortingEnabled(true);
    ui.contactsList->sortByColumn(0, Qt::AscendingOrder);

    connect(ui.contactsList, SIGNAL(clicked(QModelIndex)), this, SLOT(activateOkButton()));
    connect(ui.showOfflineContacts, SIGNAL(toggled(bool)), this, SLOT(enableOfflineContacts(bool)));
    connect(ui.showGroups, SIGNAL(toggled(bool)), this, SLOT(enableGroupsView(bool)));
}

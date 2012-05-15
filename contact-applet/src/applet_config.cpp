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

#include "applet_config.h"
#include "contact-delegate.h"

#include <KTp/Models/accounts-model.h>
#include <KTp/Models/contact-model-item.h>
#include <KTp/Models/groups-model.h>
#include <KTp/Models/accounts-filter-model.h>

#include <KDebug>
#include <KPushButton>

AppletConfig::AppletConfig(const Tp::AccountManagerPtr &accountManager, QWidget* parent)
    : KDialog(parent)
    , m_model(0)
    , m_modelFilter(0)
    , m_groupsModel(0)
    , m_accountManager(accountManager)
{
    QWidget *widget = new QWidget(this);

    ui.setupUi(widget);
    setMainWidget(widget);
    setCaption(i18n("Select a contact"));
    setupContactsList();

    connect(this, SIGNAL(buttonClicked(KDialog::ButtonCode)), this, SLOT(slotButtonClicked(int)));
}

AppletConfig::~AppletConfig()
{
    m_groupsModel->deleteLater();
    m_model->deleteLater();
    m_modelFilter->deleteLater();
}

void AppletConfig::activateOkButton()
{
    button(Ok)->setEnabled(true);
}

void AppletConfig::enableGroupsView(bool enable)
{
    if (enable) {
        m_modelFilter->setSourceModel(m_groupsModel);
    } else {
        m_modelFilter->setSourceModel(m_model);
    }
}

void AppletConfig::enableOfflineContacts(bool enable)
{
    m_modelFilter->setPresenceTypeFilterFlags(enable ? AccountsFilterModel::DoNotFilterByPresence : AccountsFilterModel::ShowOnlyConnected);
}

void AppletConfig::setupContactsList()
{
    // prepare models
    m_model = new AccountsModel(this);
    m_model->setAccountManager(m_accountManager);
    m_groupsModel = new GroupsModel(m_model, this);
    m_modelFilter = new AccountsFilterModel(this);

    // setup model visualization type
    if (ui.showGroups->isChecked()) {
        m_modelFilter->setSourceModel(m_groupsModel);
    } else {
        m_modelFilter->setSourceModel(m_model);
    }

    m_modelFilter->setDynamicSortFilter(true);
    m_modelFilter->setPresenceTypeFilterFlags(ui.showOfflineContacts->isChecked() ? AccountsFilterModel::DoNotFilterByPresence : AccountsFilterModel::ShowOnlyConnected);
    m_modelFilter->setSortMode(AccountsFilterModel::SortByPresence);

    // set model for the contacts tree view
    ui.contactsList->setModel(m_modelFilter);

    // disable ok button until a list item is selected
    button(Ok)->setEnabled(false);

    ui.contactsList->header()->hide();
    ui.contactsList->setRootIsDecorated(false);
    ui.contactsList->setItemDelegate(new ContactDelegate());
    ui.contactsList->setSortingEnabled(true);
    ui.contactsList->sortByColumn(0, Qt::AscendingOrder);
    ui.contactsList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.contactsList->setSelectionBehavior(QAbstractItemView::SelectItems);

    connect(ui.contactsList, SIGNAL(clicked(QModelIndex)), this, SLOT(activateOkButton()));
    connect(ui.showOfflineContacts, SIGNAL(toggled(bool)), this, SLOT(enableOfflineContacts(bool)));
    connect(ui.showGroups, SIGNAL(toggled(bool)), this, SLOT(enableGroupsView(bool)));
}

void AppletConfig::slotButtonClicked(int button)
{
    QModelIndex selectedItem = ui.contactsList->currentIndex();

    if (button == KDialog::Ok && selectedItem.isValid()) {
        if (selectedItem.data(AccountsModel::ItemRole).userType() == qMetaTypeId<ContactModelItem*>()) {
            ContactModelItem *item = selectedItem.data(AccountsModel::ItemRole).value<ContactModelItem*>();

            // retrieve account related to the contact
            Tp::AccountPtr account = m_model->accountForContactItem(item);

            setNewContact(item->contact(), account);
            accept();
        }
    } else if (button == KDialog::Cancel) {
        delayedDestruct();
    }

    KDialog::slotButtonClicked(button);
}

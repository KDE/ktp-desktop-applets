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
#include "contact-delegate-compact.h"

#include <KTp/Models/contacts-model.h>

#include <KDebug>
#include <KPushButton>

AppletConfig::AppletConfig(const Tp::AccountManagerPtr &accountManager, QWidget* parent)
    : KDialog(parent)
    , m_model(0)
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
    m_model->deleteLater();
}

void AppletConfig::enableGroupsView(bool enable)
{
    if (enable) {
        m_model->setGroupMode(KTp::ContactsModel::GroupGrouping);
    } else {
        m_model->setGroupMode(KTp::ContactsModel::AccountGrouping);
    }
}

void AppletConfig::enableOfflineContacts(bool enable)
{
    m_model->setPresenceTypeFilterFlags(enable ? KTp::ContactsFilterModel::DoNotFilterByPresence : KTp::ContactsFilterModel::ShowOnlyConnected);
}

void AppletConfig::setupContactsList()
{
    // prepare models
    m_model = new KTp::ContactsModel(this);
    m_model->setAccountManager(m_accountManager);

    m_model->setDynamicSortFilter(true);
    m_model->setPresenceTypeFilterFlags(ui.showOfflineContacts->isChecked() ? KTp::ContactsFilterModel::DoNotFilterByPresence : KTp::ContactsFilterModel::ShowOnlyConnected);
    m_model->setSortRole(Qt::DisplayRole);

    // set model for the contacts tree view
    ui.contactsList->setModel(m_model);

    // disable ok button until a list item is selected
    button(Ok)->setEnabled(false);

    ui.contactsList->header()->hide();
    ui.contactsList->setRootIsDecorated(false);
    ui.contactsList->setItemDelegate(new ContactDelegateCompact());
    ui.contactsList->setSortingEnabled(true);
    ui.contactsList->sortByColumn(0, Qt::AscendingOrder);
    ui.contactsList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.contactsList->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui.contactsList->setExpandsOnDoubleClick(false);

    connect(ui.contactsList, SIGNAL(clicked(QModelIndex)), this, SLOT(contactListClicked(QModelIndex)));
    connect(ui.contactsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(contactListDoubleClicked(QModelIndex)));
    connect(ui.showOfflineContacts, SIGNAL(toggled(bool)), this, SLOT(enableOfflineContacts(bool)));
    connect(ui.showGroups, SIGNAL(toggled(bool)), this, SLOT(enableGroupsView(bool)));
}

void AppletConfig::contactListClicked(const QModelIndex& index)
{
    if (index.data(KTp::RowTypeRole).toInt() == KTp::ContactRowType) {
        button(Ok)->setEnabled(true);
    } else {
        button(Ok)->setEnabled(false);

        if (ui.contactsList->isExpanded(index)) {
            ui.contactsList->collapse(index);
        } else {
            ui.contactsList->expand(index);
        }
    }
}

void AppletConfig::contactListDoubleClicked(const QModelIndex& index)
{
    if (index.data(KTp::RowTypeRole).toUInt() == KTp::ContactRowType) {
        button(Ok)->setEnabled(true);
        slotButtonClicked(Ok);
    }
}

void AppletConfig::slotButtonClicked(int button)
{
    QModelIndex selectedItem = ui.contactsList->currentIndex();

    if (button == KDialog::Ok && selectedItem.isValid()) {
        if (selectedItem.data(KTp::RowTypeRole).toInt() == KTp::ContactRowType) {
            KTp::ContactPtr contact = selectedItem.data(KTp::ContactRole).value<KTp::ContactPtr>();
            Tp::AccountPtr account = selectedItem.data(KTp::AccountRole).value<Tp::AccountPtr>();

            setNewContact(contact, account);
            accept();
        }
    } else if (button == KDialog::Cancel) {
        delayedDestruct();
    }

    KDialog::slotButtonClicked(button);
}

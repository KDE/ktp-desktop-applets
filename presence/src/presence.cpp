/*
 *   Copyright (C) 2008 George Goldberg <grundleborg@googlemail.com>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presence.h"

#include "presenceitemdelegate.h"

#include <Plasma/Dialog>
#include <Plasma/IconWidget>
#include <Plasma/Theme>

#include <KColorScheme>
#include <KDebug>
#include <KIcon>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Client/PendingReadyAccountManager>
#include <TelepathyQt4/Client/PendingReady>
#include <TelepathyQt4/Client/Account>

#include <QtCore/QList>
#include <QtCore/QSharedPointer>

#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTreeView>
#include <QtGui/QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QtGui/QVBoxLayout>

PresenceApplet::PresenceApplet(QObject * parent, const QVariantList & args)
  : Plasma::PopupApplet(parent, args),
    m_icon(0),
    m_widget(0),
    m_engine(0),
    m_colorScheme(0),
    m_masterStatusLayout(0),
    m_masterIconLabel(0),
    m_masterStatusMessageLabel(0),
    m_accountsModel(0),
    m_accountsView(0),
    m_layout(0),
    m_userSet(false),
    m_accountManager(0)
{
    setBackgroundHints(StandardBackground);
}

PresenceApplet::~PresenceApplet()
{
    if(m_widget)
        delete m_widget;
    delete m_colorScheme;
}

void PresenceApplet::init()
{
    kDebug() << "Initializing applet.";

    // Set up the color scheme.
    Q_ASSERT(!m_colorScheme);  // Pointer should still be assigned to 0.
    m_colorScheme = new KColorScheme(QPalette::Active, 
                                     KColorScheme::View,
                                     Plasma::Theme::defaultTheme()->colorScheme());

    // Set up the icon.
    Q_ASSERT(!m_icon);  // Pointer should still be assigned to 0.
    m_icon = new Plasma::IconWidget(KIcon("user-offline",NULL), QString());
    setPopupIcon(m_icon->icon());

    // The icon has been changed.
    iconChanged();

    // Set up the accounts model.
    Q_ASSERT(!m_accountsModel);  //Pointer should still be assigned to 0.
    m_accountsModel = new QStandardItemModel(this);
    m_accountsModel->setColumnCount(4);

    connect(m_accountsModel, SIGNAL(itemChanged(QStandardItem*)), this,
            SLOT(onItemChanged(QStandardItem*)));

    m_accountsModel->setHeaderData(1, Qt::Horizontal,
                                   QVariant("status-type"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(2, Qt::Horizontal,
                                   QVariant("status-name"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(3, Qt::Horizontal,
                                   QVariant("status-message"), Qt::DisplayRole);
    //setup Telepathy Account Manager
    m_accountManager = new Telepathy::Client::AccountManager(QDBusConnection::sessionBus());
    connect(m_accountManager->becomeReady(),
            SIGNAL(finished(Telepathy::Client::PendingOperation *)), this,
            SLOT(onReady(Telepathy::Client::PendingOperation *)));

    Q_ASSERT(!m_engine);  // Pointer should still be assigned to 0.
    m_engine = dataEngine("presence");

    QStringList sources = m_engine->sources();
    foreach(const QString & source, sources)
    {
        sourceAdded(source);
    }

    connect(m_engine, SIGNAL(sourceAdded(QString)),
            this, SLOT(sourceAdded(QString)));
    connect(m_engine, SIGNAL(sourceRemoved(QString)),
            this, SLOT(sourceRemoved(QString)));
}

void PresenceApplet::onReady(Telepathy::Client::PendingOperation *result)
{

}
QWidget *PresenceApplet::widget()
{
	if(!m_widget)
    {
        // Set up the accounts view.
        Q_ASSERT(!m_accountsView);  // Pointer should still be assigned to 0.
        m_accountsView = new QTreeView;
        m_accountsView->setItemDelegate(new PresenceItemDelegate);
        m_accountsView->setModel(m_accountsModel);
        m_accountsView->header()->setVisible(true);
        m_accountsView->setColumnHidden(0, true);   //Hide the source id column

        // Set up the master status section.
        Q_ASSERT(!m_masterStatusLayout);  // Pointer should still be assigned to 0.
        Q_ASSERT(!m_masterIconLabel);  // Pointer should still be assigned to 0.
        Q_ASSERT(!m_masterStatusMessageLabel);  // Pointer should still be assigned to 0.

        m_masterStatusLayout = new QHBoxLayout(m_widget);

        m_masterIconLabel = new QLabel;
        m_masterStatusMessageLabel = new QLabel;

        m_masterStatusLayout->addWidget(m_masterIconLabel);
        m_masterStatusLayout->addWidget(m_masterStatusMessageLabel);

        iconChanged();
        masterStatusMessageChanged(m_masterStatusMessage);

        // Set up the rest of the view/layout etc. stuff.
        Q_ASSERT(!m_widget);  // Pointer should still be assigned to 0.
        Q_ASSERT(!m_layout);  // Pointer should still be assigned to 0.
        m_widget = new QWidget();
        m_layout = new QVBoxLayout(m_widget);
        m_layout->addLayout(m_masterStatusLayout);
        m_layout->addWidget(m_accountsView);
        m_widget->setLayout(m_layout);

        // Apply the theme's color scheme to the widget.
        Q_ASSERT(m_colorScheme);
        QPalette editPalette = m_widget->palette();
        editPalette.setBrush(QPalette::Window,
                             m_colorScheme->background());
        editPalette.setBrush(QPalette::WindowText,
                             m_colorScheme->foreground());
        m_widget->setPalette(editPalette);
    }

    Q_ASSERT(m_widget);  // We must have a valid m_widget by now.

    return m_widget;
}

void PresenceApplet::sourceAdded(const QString & source)
{
    Q_ASSERT(m_engine);  // Engine must be valid.
    kDebug() << "started with source: " << source;
    m_engine->connectSource(source, this);
}

void PresenceApplet::sourceRemoved(const QString & source)
{
    Q_ASSERT(m_engine);  // Engine must be valid.
    kDebug() << "started with source: " << source;
    m_engine->disconnectSource(source, this);
}

void PresenceApplet::commitData(QWidget * editor)
{
    kDebug()<<m_userSet;
    //m_userSet = true;
}

void PresenceApplet::dataUpdated(const QString & source,
                            const Plasma::DataEngine::Data & data)
{
    kDebug() << "Started with source: " << source; 

   /*
     * the data has been updated for one or more source.
     * We must see if there is already a row in the
     * model representing that source.
     * If there is, then we update the data for that
     * row. If not, then we create a new row with
     * the data for that source.
     */
    QStandardItem * presence_type = new QStandardItem;
    QStandardItem * presence_state = new QStandardItem;
    QStandardItem * message = new QStandardItem;
    QStandardItem * accountItem = new  QStandardItem;
    
    // \brief: setup color roles
    presence_type->setData(Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor));
    presence_state->setData(Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor));
    message->setData(Plasma::Theme::defaultTheme()->color(Plasma::Theme::BackgroundColor));

    // \brief: setup account presence
    Telepathy::SimplePresence currentPresence
        = data.value("current_presence").value<Telepathy::SimplePresence>();
    if (m_currentPresence == currentPresence.status) {
        return;
    }

    m_currentPresence = currentPresence.status;

    presence_type->setData(static_cast<uint>(currentPresence.type),
    						Qt::DisplayRole);
    presence_state->setData(currentPresence.status, Qt::DisplayRole);
    message->setData(currentPresence.statusMessage, Qt::DisplayRole);
    accountItem->setData(source, Qt::DisplayRole);

    /*
     * so, we need to look in the first column
     * to see if we can find a row with that value
     */
    Q_ASSERT(m_accountsModel);  // This must exist now.
    QList<QStandardItem*> items;
    items = m_accountsModel->findItems(source, Qt::MatchExactly, 0);
    int itemsCount = items.count();
    if(0 == itemsCount)
    {
        /*
         * the source is new, so create
         * a new row for it.
         */
        QStandardItem * id = new QStandardItem;
        id->setData(source, Qt::DisplayRole);
        QList<QStandardItem*> row;
        row.append(id);
        row.append(presence_type);
        row.append(presence_state);
        row.append(message);
        m_accountsModel->appendRow(row);
    }
    else if(1 == itemsCount)
    {
        /*
         * the data source is NOT new,
         * so we update the row that
         * is already there for it.
         */
        int row = items.first()->row();
        m_accountsModel->setItem(row, 0, accountItem);
        m_accountsModel->setItem(row, 1, presence_type);
        m_accountsModel->setItem(row, 2, presence_state);
        m_accountsModel->setItem(row, 3, message);
    }
    else
    {
        /*
         * we shouldn't get here because
         * if we do then there are two
         * or more rows for the same data
         * source !!!
         */
        kDebug() << "ERROR: two or more rows for the same data source in the model!";
    }

    // Update the master presence.
    updateMasterPresence();
}

void PresenceApplet::onItemChanged(QStandardItem * item)
{
   QModelIndex index = m_accountsModel->indexFromItem(item);
   QString source = m_accountsModel->data(m_accountsModel->index(index.row(), 0)).toString();
   uint type = m_accountsModel->data(m_accountsModel->index(index.row(), 1)).toUInt();
   QString status = m_accountsModel->data(m_accountsModel->index(index.row(), 2)).toString();
   QString statusMessage = m_accountsModel->data(m_accountsModel->index(index.row(), 3)).toString();
   //work around account object patch gets trimmed 
   QString accountObjectPath = "/org/freedesktop/Telepath" + source;
   if (m_accountManager->isReady()) {
       Telepathy::Client::Account * account =
       m_accountManager->accountForPath(accountObjectPath).data();
       if (account) {
          Telepathy::SimplePresence simplePresence;
          simplePresence.type = type;
          simplePresence.status = status;
          simplePresence.statusMessage =  statusMessage;
          account->setRequestedPresence(simplePresence);
          m_userSet = false;
        }
    }

}

/**
 * @brief Update the master presence state.
 *
 * We must check what presence state each account is in and use that to decide
 * which icon we display. We must also check the presence message for each
 * account to see if we can display one overall presence message.
 */
void PresenceApplet::updateMasterPresence()
{
    Q_ASSERT(m_accountsModel);
    Q_ASSERT(m_icon);
    // Get data we can use to iterate over the contents of the accounts model.
    int rowCount = m_accountsModel->rowCount();

    // First we workout the overall presence message.
    QStringList statusMessages;
    for(int i=0; i<rowCount; i++)
    {
        statusMessages << m_accountsModel->data(m_accountsModel->index(i, 3)).toString();
    }

    bool statusMessagesAllTheSame = true;
    QString previousStatusMessage = statusMessages.at(0);
    foreach(const QString & statusMessage, statusMessages)
    {
        if(statusMessage == previousStatusMessage)
        {
            previousStatusMessage = statusMessage;
            continue;
        }
        else
        {
            statusMessagesAllTheSame = false;
            break;
        }
    }

    if(statusMessagesAllTheSame)
    {
        masterStatusMessageChanged(previousStatusMessage);
    }
    else
    {
        masterStatusMessageChanged(QString());
    }

    // Next, we work out the overall presence status.
    int accountsOffline = 0;
    int accountsAvailable = 0;
    int accountsAway = 0;
    int accountsExtendedAway = 0;
    int accountsHidden = 0;
    int accountsBusy = 0;

    bool okOffline = false;
    bool okAway = false;
    bool okExtendedAway = false;
    bool okHidden = false;
    bool okBusy = false;

    // Iterate over all the accounts in the model, and total up how many are
    // in each type of presence state.
    for(int i=0; i<rowCount; i++)
    {
        Telepathy::ConnectionPresenceType status_type =
            static_cast<Telepathy::ConnectionPresenceType>(m_accountsModel->data(m_accountsModel->index(i, 1)).toUInt());

        switch(status_type)
        {
        case Telepathy::ConnectionPresenceTypeOffline:
        case Telepathy::ConnectionPresenceTypeUnknown:
            accountsOffline++;
            okOffline = true;
            break;
        case Telepathy::ConnectionPresenceTypeAvailable:
            accountsAvailable++;
            break;
        case Telepathy::ConnectionPresenceTypeAway:
            accountsAway++;
            okAway = true;
            break;
        case Telepathy::ConnectionPresenceTypeExtendedAway:
            accountsExtendedAway++;
            okExtendedAway = true;
            break;
        case Telepathy::ConnectionPresenceTypeHidden:
            accountsHidden++;
            okHidden = true;
            break;
        case Telepathy::ConnectionPresenceTypeBusy:
            accountsBusy++;
            okBusy = true;
            break;
        case Telepathy::ConnectionPresenceTypeUnset:
        case Telepathy::ConnectionPresenceTypeError:
        	break;
        }
    }

    // Chose a master presence state from this.
    // FIXME: What should be the logic for choosing a master presence state?
    //        Shoud this be user customisable?
    //        Currently follows the kopete approach.
    if(okOffline == true)
    {
        m_icon->setIcon(KIcon("user-offline"));
    }
    else if(okHidden == true)
    {
        m_icon->setIcon(KIcon("user-invisible"));
    }
    else if(okBusy == true)
    {
        m_icon->setIcon(KIcon("user-busy"));
    }
    else if(okExtendedAway == true)
    {
        m_icon->setIcon(KIcon("user-away-extended"));
    }
    else if(okAway == true)
    {
        m_icon->setIcon(KIcon("user-away"));
    }
    else
    {
        m_icon->setIcon(KIcon("user-online"));
    }

    // Call the method to update the masterPresenceIcon.
    setPopupIcon(m_icon->icon());
    iconChanged();
}

void PresenceApplet::iconChanged()
{
    // The icon has been changed. We must update the pixmap of the icon for
    // display in the main widget.
    if(m_masterIconLabel)
    {
        m_masterIconLabel->setPixmap(m_icon->icon().pixmap(QSize(32, 32)));
    }
}

void PresenceApplet::masterStatusMessageChanged(const QString & message)
{
    // If m_masterStatusMessageLabel points to a valid QLabel, update it.
    if(m_masterStatusMessageLabel)
    {
        m_masterStatusMessageLabel->setText(message);
    }

    // Store the master presence message as a member var.
    m_masterStatusMessage = message;
}

#include "presence.moc"


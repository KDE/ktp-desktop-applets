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

#include <plasma/theme.h>
#include <plasma/widgets/icon.h>

#include <Decibel/Types>

#include <KColorScheme>
#include <KDebug>
#include <KIcon>
#include <KLineEdit>

#include <QtTapioca/PresenceState>

#include <QtCore/QList>

#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>

PresenceApplet::PresenceApplet(QObject * parent, const QVariantList & args)
  : PlasmaAppletDialog(parent, args),
    m_engine(0),
    m_colorScheme(0),
    m_messageEdit(0),
    m_masterStatusLayout(0),
    m_masterIconLabel(0),
    m_masterStatusMessageLabel(0),
    m_accountsModel(0),
    m_accountsView(0),
    m_layout(0),
    m_widget(0)
{ }

PresenceApplet::~PresenceApplet()
{
    delete m_colorScheme;
}

void
PresenceApplet::initialize()
{
    kDebug() << "Initializing applet.";

    // Set up the color scheme.
    Q_ASSERT(!m_colorScheme);  // Pointer should still be assigned to 0.
    m_colorScheme = new KColorScheme(QPalette::Active, 
                                     KColorScheme::View,
                                     Plasma::Theme::defaultTheme()->colorScheme());

    // Set up the icon.
    Q_ASSERT(!m_icon);  // Pointer should still be assigned to 0.
    m_icon = new Plasma::Icon(KIcon("user-offline"), QString(), this);

    // The icon has been changed.
    iconChanged();

    // Set up the accounts model.
    Q_ASSERT(!m_accountsModel);  //Pointer should still be assigned to 0.
    m_accountsModel = new QStandardItemModel(this);
    m_accountsModel->setColumnCount(4);

    m_accountsModel->setHeaderData(1, Qt::Horizontal,
                                   QVariant("online?"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(2, Qt::Horizontal,
                                   QVariant("status"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(3, Qt::Horizontal,
                                   QVariant("message"), Qt::DisplayRole);

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

QWidget *
PresenceApplet::widget()
{
    if(!m_widget)
    {
        // Set up the accounts view.
        Q_ASSERT(!m_accountsView);  // Pointer should still be assigned to 0.
        m_accountsView = new QTreeView;
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
        Q_ASSERT(!m_messageEdit);  // Pointer should still be assigned to 0.
        m_messageEdit = new KLineEdit;

        Q_ASSERT(!m_widget);  // Pointer should still be assigned to 0.
        Q_ASSERT(!m_layout);  // Pointer should still be assigned to 0.
        m_widget = new QWidget();
        m_layout = new QVBoxLayout(m_widget);
        m_layout->addLayout(m_masterStatusLayout);
        m_layout->addWidget(m_accountsView);
        m_layout->addWidget(m_messageEdit);
        m_widget->setLayout(m_layout);

        // Apply the theme's color scheme to the widget.
        Q_ASSERT(m_colorScheme);
        QPalette editPalette = m_widget->palette();
        editPalette.setBrush(QPalette::Window,
                             m_colorScheme->background());
        m_widget->setPalette(editPalette);
    }

    Q_ASSERT(m_widget);  // We must have a valid m_widget by now.

    return m_widget;
}

void
PresenceApplet::sourceAdded(const QString & source)
{
    Q_ASSERT(m_engine);  // Engine must be valid.
    kDebug() << "started with source: " << source;
    m_engine->connectSource(source, this);
}


void
PresenceApplet::sourceRemoved(const QString & source)
{
    Q_ASSERT(m_engine);  // Engine must be valid.
    kDebug() << "started with source: " << source;
    m_engine->disconnectSource(source, this);
}

void
PresenceApplet::dataUpdated(const QString & source,
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
    // FIXME: Reimplement the lines below using new plasma API
    //online->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    //status->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    //message->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    QtTapioca::PresenceState currentPresence
        = data.value("current_presence").value<QtTapioca::PresenceState>();

    presence_type->setData(static_cast<int>(currentPresence.type()),
                           Qt::DisplayRole);
    presence_state->setData(currentPresence.name(), Qt::DisplayRole);
    message->setData(data.value("status_message").toString(),
                     Qt::DisplayRole);
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

/**
 * @brief Update the master presence state.
 *
 * We must check what presence state each account is in and use that to decide
 * which icon we display. We must also check the presence message for each
 * account to see if we can display one overall presence message.
 */
void
PresenceApplet::updateMasterPresence()
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
    QString previousStatusMessage;
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

    bool okOffline = true;
    bool okAvailable = true;
    bool okAway = true;
    bool okExtendedAway = true;
    bool okHidden = true;
    bool okBusy = true;

    // Iterate over all the accounts in the model, and total up how many are
    // in each type of presence state.
    for(int i=0; i<rowCount; i++)
    {
        QtTapioca::PresenceState::Type status_type = 
            static_cast<QtTapioca::PresenceState::Type>(m_accountsModel->data(m_accountsModel->index(i, 1)).toInt());

        switch(status_type)
        {
        case QtTapioca::PresenceState::OfflineType:
        case QtTapioca::PresenceState::UnsetType:
            accountsOffline++;
            break;
        case QtTapioca::PresenceState::AvailableType:
            accountsAvailable++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okAway = false;
            okBusy = false;
            break;
        case QtTapioca::PresenceState::AwayType:
            accountsAway++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okBusy = false;
            break;
        case QtTapioca::PresenceState::ExtendedAwayType:
            accountsExtendedAway++;
            okOffline = false;
            okHidden = false;
            okBusy = false;
            break;
        case QtTapioca::PresenceState::HiddenType:
            accountsHidden++;
            okOffline = false;
            okExtendedAway = false;
            okAway = false;
            okBusy = false;
            break;
        case QtTapioca::PresenceState::BusyType:
            accountsBusy++;
            okOffline = false;
            okHidden = false;
            okExtendedAway = false;
            okAway = false;
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
    iconChanged();
}

void
PresenceApplet::iconChanged()
{
    // The icon has been changed. We must update the pixmap of the icon for
    // display in the main widget.
    if(m_masterIconLabel)
    {
        m_masterIconLabel->setPixmap(m_icon->icon().pixmap(QSize(32, 32)));
    }
}

void
PresenceApplet::masterStatusMessageChanged(const QString & message)
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


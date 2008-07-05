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
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "presence.h"

#include <plasma/theme.h>
#include <plasma/widgets/icon.h>

#include <QString>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QWidget>
#include <QList>
#include <QVBoxLayout>

#include <KLineEdit>
#include <KDebug>
#include <KColorScheme>
#include <KIcon>
#include <KColorScheme>

Presence::Presence(QObject *parent, const QVariantList &args)
    : PlasmaAppletDialog(parent, args),
    m_accountsModel(0),
    m_accountsView(0),
    m_messageEdit(0),
    m_colorScheme(0)
{
    m_layout = 0;
    m_widget = 0;
}

void Presence::initialize()
{
    kDebug() << "Initializing applet.";

    // Set up the color scheme.
    m_colorScheme = new KColorScheme(QPalette::Active, KColorScheme::View, Plasma::Theme::defaultTheme()->colorScheme());

    // Set up the icon.
    m_icon = new Plasma::Icon(KIcon("user-offline"), QString(), this);

    // Set up the accounts model.
    m_accountsModel = new QStandardItemModel(this);
    m_accountsModel->setColumnCount(4);
    m_accountsModel->setHeaderData(1, Qt::Horizontal, QVariant("online?"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(2, Qt::Horizontal, QVariant("status"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(3, Qt::Horizontal, QVariant("message"), Qt::DisplayRole);

    m_engine = dataEngine("presence");

    QStringList sources = m_engine->sources();
    foreach(const QString & source, sources)
    {
        sourceAdded(source);
    }

    connect(m_engine, SIGNAL(sourceAdded(QString)), this, SLOT(sourceAdded(QString)));
    connect(m_engine, SIGNAL(sourceRemoved(QString)), this, SLOT(sourceRemoved(QString)));
}

QWidget * Presence::widget()
{
    if(!m_widget)
    {
        // Set up the accounts view.
        m_accountsView = new QTreeView;
        m_accountsView->setModel(m_accountsModel);
        m_accountsView->header()->setVisible(true);
        m_accountsView->setColumnHidden(0, true);   //Hide the source id column

        // Set up the rest of the view/layout etc. stuff.
        m_messageEdit = new KLineEdit;

        m_widget = new QWidget();
        m_layout = new QVBoxLayout(m_widget);
        m_layout->addWidget(m_accountsView);
        m_layout->addWidget(m_messageEdit);
        m_widget->setLayout(m_layout);

        // Apply the theme's color scheme to the widget.
        QPalette editPalette = m_widget->palette();
        editPalette.setColor(QPalette::Window, m_colorScheme->background().color());
        m_widget->setPalette(editPalette);
    }

    return m_widget;
}

Presence::~Presence()
{
    delete m_colorScheme;
}

/*
QSizeF Presence::contentSizeHint() const
{
    if (m_layout) {
        kDebug() << "Returning the m_form size geometry";
        return m_form->effectiveSizeHint(Qt::PreferredSize, contentSize());
    }*/
    /*
     * this is the hardcoded default size
     * of the plasmoid. (apparently not...)
     */
    // FIXME: change this to be a good size...
    /*
    return QSizeF(300, 300);
}
*/

void Presence::sourceAdded(const QString& source)
{
    kDebug() << "started with source: " << source;
    m_engine->connectSource(source, this);
}


void Presence::sourceRemoved(const QString& source)
{
    kDebug() << "started with source: " << source;
    m_engine->disconnectSource(source, this);
}

void Presence::dataUpdated(const QString &source, const Plasma::DataEngine::Data &data)
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
    QStandardItem *online = new QStandardItem;
    QStandardItem *status = new QStandardItem;
    QStandardItem *message = new QStandardItem;
    //online->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    //status->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    //message->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    QString decibelCurrentPresence = data.value("decibel_current_presence").toString();
    if(decibelCurrentPresence == "1")
    {
       /*
        * we are offline.
        */
        kDebug() << "decibel_current_presence == 1";
        online->setData(false, Qt::DisplayRole);
    }
    else
    {
       /*
        * we are online
        */
        kDebug() << "decibel_current_presence != 1";
        online->setData(true, Qt::DisplayRole);
    }
    status->setData(data.value("decibel_current_presence").toString(), Qt::DisplayRole);
    message->setData(data.value("decibel_presence_message").toString(), Qt::DisplayRole);
    /*
     * so, we need to look in the first column
     * to see if we can find a row with that value
     */
    QList<QStandardItem*> items;
    items = m_accountsModel->findItems(source, Qt::MatchExactly, 0);
    int itemsCount = items.count();
    if ( 0 == itemsCount )
    {
        /*
         * the source is new, so create
         * a new row for it.
         */
        QStandardItem *id = new QStandardItem;
        id->setData(source, Qt::DisplayRole);
        QList<QStandardItem*> row;
        row.append(id);
        row.append(online);
        row.append(status);
        row.append(message);
        m_accountsModel->appendRow(row);
    }
    else if ( 1 == itemsCount )
    {
        /*
         * the data source is NOT new,
         * so we update the row that
         * is already there for it.
         */
        int row = items.first()->row();
        m_accountsModel->setItem(row, 1, online);
        m_accountsModel->setItem(row, 2, status);
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
void Presence::updateMasterPresence()
{
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
    foreach(QString statusMessage, statusMessages)
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
        // TODO: Set the master presence status message.
    }
    else
    {
        // TODO: Set the master presence status message to be QString();
    }

    // Next, we work out the overall presence status.
    int accountsOffline = 0;
    int accountsAvailable = 0;
    int accountsAway = 0;
    int accountsExtendedAway = 0;
    int accountsHidden = 0;
    int accountsBusy = 0;

    // TODO: complete this bit once the decibel AccountManager provides a
    // QtTapioca::PresenceState object for the presence state.
}

#include "presence.moc"

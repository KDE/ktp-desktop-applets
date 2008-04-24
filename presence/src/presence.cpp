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

#include <QPainter>
#include <QString>
#include <QStandardItemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QList>

#include <KLineEdit>
#include <KDebug>
#include <KColorScheme>

Presence::Presence(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args),
    m_accountsModel(0),
    m_accountsView(0),
    m_accountsViewProxyWidget(0),
    m_messageEditProxyWidget(0),
    m_messageEdit(0)
{
    m_layout = 0;
    m_form = 0;
}

void Presence::init() 
{
    setHasConfigurationInterface(false);

    /*
     * set up the accounts model.
     */
    m_accountsModel = new QStandardItemModel(this);
    m_accountsModel->setColumnCount(4);
    m_accountsModel->setHeaderData(1, Qt::Horizontal, QVariant("online?"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(2, Qt::Horizontal, QVariant("status"), Qt::DisplayRole);
    m_accountsModel->setHeaderData(3, Qt::Horizontal, QVariant("message"), Qt::DisplayRole);

    /*
     * set up the accounts view.
     */
    m_accountsView = new QTreeView;
    m_accountsView->setModel(m_accountsModel);
    m_accountsView->header()->setVisible(true);
    m_accountsView->setColumnHidden(0, true);   //Hide the source id column

    /*
     * set up the rest of the view/layout etc. stuff
     */
    m_accountsViewProxyWidget = new QGraphicsProxyWidget(this);
    m_accountsViewProxyWidget->setWidget(m_accountsView);
    m_accountsView->show();
    m_accountsViewProxyWidget->show();
    m_messageEdit = new KLineEdit;
    m_messageEditProxyWidget = new QGraphicsProxyWidget(this);
    m_messageEditProxyWidget->setWidget(m_messageEdit);
    m_messageEdit->show();
    m_messageEditProxyWidget->show();

    m_form = new QGraphicsWidget(this);
    m_layout = new QGraphicsLinearLayout(Qt::Vertical, m_form);
    m_layout->addItem(m_accountsViewProxyWidget);
    m_layout->addItem(m_messageEditProxyWidget);
    m_form->setLayout(m_layout);

    updateGeometry();

    m_engine = dataEngine("presence");

    connect(m_engine, SIGNAL(sourceAdded(QString)), this, SLOT(sourceAdded(QString)));
    connect(m_engine, SIGNAL(sourceRemoved(QString)), this, SLOT(sourceRemoved(QString)));
}

Presence::~Presence()
{

}

void Presence::constraintsUpdated(Plasma::Constraints constraints)
{
    if (constraints & Plasma::SizeConstraint) {
        if (m_layout) {
            m_layout->setGeometry(QRectF(QPointF(0,0), contentSize()));
            m_layout->setPreferredSize(contentSize());
            m_form->resize(contentSize());
            prepareGeometryChange();
            update();
        }
    }
}

QSizeF Presence::contentSizeHint() const
{
    if (m_layout) {
        kDebug() << "Returning the m_form size geometry";
        return m_form->effectiveSizeHint(Qt::PreferredSize, contentSize());
    }
    /*
     * this is the hardcoded default size
     * of the plasmoid. (apparently not...)
     */
    // FIXME: change this to be a good size...
    return QSizeF(300, 300);
}


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
    online->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    status->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
    message->setData(Plasma::Theme::self()->textColor(), Qt::ForegroundRole);
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
}

void Presence::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(p);
    Q_UNUSED(option);
    Q_UNUSED(contentsRect);
}

#include "presence.moc"

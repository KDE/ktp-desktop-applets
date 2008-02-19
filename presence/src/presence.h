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

#ifndef PRESENCE_H
#define PRESENCE_H

#include <plasma/applet.h>
#include <plasma/dataengine.h>

#include <QGraphicsItem>

class QString;
class QStandardItemModel;
class QTreeView;
class QGraphicsProxyWidget;
class QGraphicsLinearLayout;
class QGraphicsWidget;

class KLineEdit;

class Presence : public Plasma::Applet
{
        Q_OBJECT
    public:
        Presence(QObject *parent, const QVariantList &args);
        ~Presence();

        QSizeF contentSizeHint() const;
        virtual void init(void);
        void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

    private slots:
        void sourceAdded(const QString& source);
        void sourceRemoved(const QString& source);
        void dataUpdated(const QString &source, const Plasma::DataEngine::Data &data);
        void constraintsUpdated(Plasma::Constraints constraints);

    private:
        QGraphicsLinearLayout* m_layout;
        Plasma::DataEngine* m_engine;
        QStandardItemModel *m_accountsModel;
        QTreeView *m_accountsView;
        QGraphicsProxyWidget *m_accountsViewProxyWidget;
        QGraphicsProxyWidget *m_messageEditProxyWidget;
        QGraphicsWidget *m_form;
        KLineEdit *m_messageEdit;
};

K_EXPORT_PLASMA_APPLET(presence, Presence)

#endif

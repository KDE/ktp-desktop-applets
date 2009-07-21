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

#ifndef PLASMA_APPLET_PRESENCE_H
#define PLASMA_APPLET_PRESENCE_H

// Qt
#include <QtCore/QString>
#include <QtGui/QLabel>

// Plasma
#include <plasma/popupapplet.h>
#include <plasma/dataengine.h>

class KColorScheme;

class AccountWidget;

class PresenceApplet : public Plasma::PopupApplet
{
    Q_OBJECT

public:
    PresenceApplet(QObject *parent, const QVariantList &args);
    ~PresenceApplet();
    void init();

    QGraphicsWidget *graphicsWidget();

private Q_SLOTS:
    void onSourceAdded(const QString &source);
    void onSourceRemoved(const QString &source);
    void dataUpdated(const QString &source,
            const Plasma::DataEngine::Data &data);
    // void onItemChanged(QStandardItem *item);

private:
    void updateMasterIcon();
    void setMasterStatusMessage(const QString &message);
    void updateMasterPresence();

    Plasma::DataEngine *m_engine;
    QGraphicsWidget *m_widget;
    QGraphicsLinearLayout *m_layout;
    KColorScheme *m_colorScheme;
    QHash<QString, AccountWidget*> m_accounts;
};

K_EXPORT_PLASMA_APPLET(presence, PresenceApplet)

#endif

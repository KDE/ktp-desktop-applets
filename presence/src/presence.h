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

#ifndef PLASMA_APPLET_PRESENCE_H
#define PLASMA_APPLET_PRESENCE_H

#include <plasma/applet.h>
#include <plasma/dataengine.h>

namespace Plasma
{
	class IconWidget;
}

#include <QtCore/QString>

class KColorScheme;

class QHBoxLayout;
class QLabel;
class QStandardItemModel;
class QString;
class QTreeView;
class QVBoxLayout;
class QWidget;

class PresenceApplet : public Plasma::Applet
{
    Q_OBJECT

public:
    PresenceApplet(QObject * parent, const QVariantList & args);
    ~PresenceApplet();

    QWidget * widget();

    void init();

private Q_SLOTS:
    void dataUpdated(const QString & source,
                     const Plasma::DataEngine::Data & data);
    void sourceAdded(const QString & source);
    void sourceRemoved(const QString & source);

private:
    void iconChanged();
    void masterStatusMessageChanged(const QString & message);
    void updateMasterPresence();

    Plasma::DataEngine * m_engine;
    Plasma::IconWidget *m_icon;

    KColorScheme * m_colorScheme;

    QHBoxLayout * m_masterStatusLayout;
    QLabel * m_masterIconLabel;
    QLabel * m_masterStatusMessageLabel;
    QStandardItemModel * m_accountsModel;
    QTreeView * m_accountsView;
    QVBoxLayout * m_layout;
    QWidget * m_widget;

    QString m_masterStatusMessage;

};

K_EXPORT_PLASMA_APPLET(presence, PresenceApplet)

#endif


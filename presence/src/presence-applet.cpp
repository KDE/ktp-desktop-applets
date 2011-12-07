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

#include "presenceapplet.h"

#include <KStandardDirs>
#include <KTelepathy/global-presence.h>
#include <KTelepathy/presence.h>

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

TelepathyPresenceApplet::TelepathyPresenceApplet(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_declarative(new Plasma::DeclarativeWidget(this))
    , m_qmlObject(0)
    , m_globalPresence(new KTp::GlobalPresence(this))
    , m_presenceAppletWrapper(new PresenceAppletWrapper(this))
{
    // setup plasmoid size
    resize(250, 250);
//     setBackgroundHints(NoBackground);

    setAspectRatioMode(Plasma::FixedSize);
}

TelepathyPresenceApplet::~TelepathyPresenceApplet()
{
    m_declarative->deleteLater();   // do i need to do this?
//     m_qmlObject ??
}

void TelepathyPresenceApplet::init()
{
    Plasma::Applet::init();

    if (m_declarative) {
        // sort this path out
        QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.telepathy-contact/contents/ui/main.qml");
        qDebug() << "LOADING: " << qmlFile;
        m_declarative->setQmlPath(qmlFile);
//         m_declarative->engine()->rootContext()->setContextProperty("TelepathyKDEPresenceApplet", /* make presenceWrapper object to interact with QML */);

        // setup qml object so that we can talk to the declarative part
        m_qmlObject = dynamic_cast<QObject*>(m_declarative->rootObject());

        // connect the qml object to recieve signals from KTP::GlobalPresece
//         connect(m_contact, SIGNAL(newContactSet()), m_qmlObject, SLOT(updateContact()));
//         connect(m_contact, SIGNAL(avatarChanged()), m_qmlObject, SLOT(updateContact()));
//         connect(m_contact, SIGNAL(presenceChanged()), m_qmlObject, SLOT(updateContact()));
//         connect(m_contact, SIGNAL(accountPresenceChanged()), m_qmlObject, SLOT(accountPresenceChanged()));
    }
}

void TelepathyPresenceApplet::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}




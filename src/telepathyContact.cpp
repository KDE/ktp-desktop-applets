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

#include "config.h"
#include "contactWrapper.h"
#include "telepathyContact.h"

#include <KStandardDirs>
#include <QObject>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
// #include <Plasma/PackageStructure>

#include <QtGui/QPainter>


TelepathyContact::TelepathyContact(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_config(new Config())
    , m_declarative(new Plasma::DeclarativeWidget(this))
    , m_contact(new ContactWrapper(parent))
    , m_qmlObject(0)
{
    // setup plasmoid
    resize(128, 128);
    setBackgroundHints(NoBackground);

    // user shouldn't be able to resize the plasmoid
    setAspectRatioMode(Plasma::FixedSize);

    connect(m_config, SIGNAL(setNewContact(Tp::ContactPtr)), this, SLOT(setContact(Tp::ContactPtr)));
}

TelepathyContact::~TelepathyContact()
{
    delete m_config;
    delete m_declarative;
//     delete m_contact;
//     delete m_qmlObject;
}

void TelepathyContact::init()
{
    Plasma::Applet::init();

    if (m_declarative) {
        QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.telepathy-contact/contents/ui/main.qml");
        qDebug() << "LOADING: " << qmlFile;
        m_declarative->setQmlPath(qmlFile);
        m_declarative->engine()->rootContext()->setContextProperty("TelepathyContact", m_contact);

        // setup qml object so that we can talk to the declarative part
        m_qmlObject = dynamic_cast<QObject*>(m_declarative->rootObject());

        // connect the qml object to recieve signals from C++ end
        connect(m_contact, SIGNAL(newContactSet()), m_qmlObject, SLOT(onNewContactSet()));
    }
}

void TelepathyContact::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}

void TelepathyContact::setContact(const Tp::ContactPtr& newContact)
{
    Q_ASSERT(newContact);

    if (!m_contact->contact() || m_contact->contact()->id() != newContact->id()) {
        m_contact->setContact(newContact);
    }
}

void TelepathyContact::showConfigurationInterface()
{
    m_config->show();
}

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(telepathy-contact, TelepathyContact)
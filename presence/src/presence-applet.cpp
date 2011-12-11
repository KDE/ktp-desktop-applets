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

#include "globalpresencewrapper.h"
#include "presenceapplet.h"

#include <KAction>
#include <KActionMenu>
#include <KStandardDirs>

#include <KTelepathy/global-presence.h>
#include <KTelepathy/presence.h>

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>

TelepathyPresenceApplet::TelepathyPresenceApplet(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_declarative(new Plasma::DeclarativeWidget(this))
    , m_qmlObject(0)
    , m_globalPresenceWrapper(new GlobalPresenceWrapper(this))
{
    // setup plasmoid size
    resize(250, 250);
    setAspectRatioMode(Plasma::FixedSize);
}

TelepathyPresenceApplet::~TelepathyPresenceApplet()
{
    m_declarative->deleteLater();   // do i need to do this?
//     m_qmlObject ??
}

int TelepathyPresenceApplet::appletHeight() const
{
    return geometry().height();
}

int TelepathyPresenceApplet::appletWidth() const
{
    return geometry().width();
}

QList< QAction* > TelepathyPresenceApplet::contextualActions()
{
    QList<QAction*>contextActions;

    KActionMenu *presenceMenu = new KActionMenu(i18n("Set presence"), this);
    KAction *goOnlineAction = new KAction(KIcon("user-online"), i18n("Online"), this);
    KAction *goBusyAction = new KAction(KIcon("user-busy"), i18n("Busy"), this);
    KAction *goAwayAction = new KAction(KIcon("user-away"), i18n("Away"), this);
    KAction *goExtendedAwayAction = new KAction(KIcon("user-away-extended"), i18n("Not Available"), this);
    KAction *goHiddenAction = new KAction(KIcon("user-invisible"), i18n("Invisible"), this);
    KAction *goOfflineAction = new KAction(KIcon("user-offline"), i18n("Offline"), this);

    // connect actions
    connect(goOnlineAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceOnline()));
    connect(goBusyAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceBusy()));
    connect(goAwayAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceAway()));
    connect(goExtendedAwayAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceXa()));
    connect(goHiddenAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceHidden()));
    connect(goOfflineAction, SIGNAL(triggered()), m_globalPresenceWrapper, SLOT(setPresenceOffline()));

    presenceMenu->addAction(goOnlineAction);
    presenceMenu->addAction(goBusyAction);
    presenceMenu->addAction(goAwayAction);
    presenceMenu->addAction(goExtendedAwayAction);
    presenceMenu->addAction(goHiddenAction);
    presenceMenu->addAction(goOfflineAction);
    presenceMenu->addSeparator();

    contextActions.append(presenceMenu);
    contextActions.append(presenceMenu->addSeparator());

    return contextActions;
}

void TelepathyPresenceApplet::init()
{
    Plasma::Applet::init();

    if (m_declarative) {
        /// TODO sort this path out with correct one
        QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.telepathy-kde-presence-applet/contents/ui/main.qml");
        qDebug() << "LOADING: " << qmlFile;
        m_declarative->setQmlPath(qmlFile);
        m_declarative->engine()->rootContext()->setContextProperty("TelepathyPresenceApplet", m_globalPresenceWrapper);

        // setup qml object so that we can talk to the declarative part
        m_qmlObject = dynamic_cast<QObject*>(m_declarative->rootObject());

        // connect the qml object to recieve signals from the globalpresencewrapper
//         connect(m_globalPresenceWrapper, SIGNAL(presenceChanged()), m_qmlObject, SLOT(/*updatePresence*/));

        // these two signals are for the plasmoid resize. QML can't determine the Plasma::DeclarativeWidget's boundaries
        connect(this, SIGNAL(widthChanged()), m_qmlObject, SLOT(onWidthChanged()));
        connect(this, SIGNAL(heightChanged()), m_qmlObject, SLOT(onHeightChanged()));
    }
}

void TelepathyPresenceApplet::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}


// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(telepathy-kde-presence-applet, TelepathyPresenceApplet)



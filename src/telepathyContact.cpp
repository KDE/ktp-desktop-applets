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

#include "telepathyContact.h"

#include <QtGui/QPainter>
#include <QDebug>

TelepathyContact::TelepathyContact(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_declarative(new Plasma::DeclarativeWidget(this))
{
    setBackgroundHints(NoBackground);
    m_declarative->setGeometry(geometry());

    // user shouldn't be able to resize the plasmoid
    setAspectRatioMode(Plasma::FixedSize);
}

TelepathyContact::~TelepathyContact()
{

}

void TelepathyContact::init()
{
    Plasma::Applet::init();

    if (m_declarative) {
        qDebug("setting declarative widget");
        m_declarative->setQmlPath("../src/declarative/main.qml");
    }
}

void TelepathyContact::paintInterface(QPainter* p, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    Plasma::Applet::paintInterface(p, option, contentsRect);
}


// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(telepathy-contact, TelepathyContact)
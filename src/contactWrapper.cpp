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

#include "contactWrapper.h"

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/Presence>

ContactWrapper::ContactWrapper(QObject* parent)
    : QObject(parent)
    , m_contact(0)
{
}

ContactWrapper::~ContactWrapper()
{
}

QString ContactWrapper::avatar() const
{
    if (m_contact) {
        if (!m_contact->avatarData().fileName.isEmpty()) {
            return m_contact->avatarData().fileName;
        } else {
            return QString("im-user");
        }
    } else {
        // return default icon
        return QString("im-user");
    }
}

Tp::ContactPtr ContactWrapper::contact() const
{
    return m_contact;
}

QString ContactWrapper::displayName() const
{
    if (m_contact) {
        return m_contact->alias();
    } else {
        return QString();
    }
}

QString ContactWrapper::presenceStatus() const
{
    if (m_contact) {
        return m_contact->presence().status();
    } else {
        return QString("");
    }
}

void ContactWrapper::setupConnects()
{
    connect(m_contact.data(), SIGNAL(avatarDataChanged(Tp::AvatarData)), this, SIGNAL(avatarChanged()));
    connect(m_contact.data(), SIGNAL(presenceChanged(Tp::Presence)), this, SIGNAL(presenceChanged()));
}

void ContactWrapper::setContact(const Tp::ContactPtr& newContact)
{
    qDebug() << "setting new contact to: " << newContact->id();

    // disconnect signals
    undoConnects();
    m_contact = newContact;

    // establish new signals
    setupConnects();

    // tell QML we have a new contact
    emit(newContactSet());
}

void ContactWrapper::undoConnects()
{
    disconnect(m_contact.data(), 0, 0, 0);
}


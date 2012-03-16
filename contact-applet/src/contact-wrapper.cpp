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

#include "contact-wrapper.h"

#include <KToolInvocation>
#include <KUrl>
#include <KDebug>

#include <TelepathyQt/AvatarData>
#include <TelepathyQt/ContactCapabilities>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/Presence>

#define PREFERRED_TEXTCHAT_HANDLER "org.freedesktop.Telepathy.Client.KDE.TextUi"

ContactWrapper::ContactWrapper(QObject* parent)
    : QObject(parent)
    , m_account(0)
    , m_contact(0)
    , m_tempAvatar(QString("im-user"))
{
}

ContactWrapper::~ContactWrapper()
{
}

QString ContactWrapper::accountId() const
{
    if (m_account) {
        return m_account.data()->uniqueIdentifier();
    } else {
        return QString();
    }
}

QString ContactWrapper::avatar() const
{
    if (m_contact) {
        if (!m_contact->avatarData().fileName.isEmpty()) {
            return m_contact->avatarData().fileName;
        }
    // check if temp avatar has been set
    } else if (!m_tempAvatar.isEmpty()) {
        return m_tempAvatar;
    }

    // return default icon
    return QString("im-user");
}

bool ContactWrapper::canSendFile() const
{
    if (m_contact && m_account) {
        return (m_contact->capabilities().fileTransfers() && m_account->capabilities().fileTransfers());
    } else {
        return false;
    }
}

bool ContactWrapper::canStartAudioCall() const
{
    if (m_contact && m_account) {
        return (m_contact->capabilities().streamedMediaAudioCalls() && m_account->capabilities().streamedMediaAudioCalls());
    } else {
        return false;
    }
}

bool ContactWrapper::canStartVideo() const
{
    if (m_contact && m_account) {
        return (m_contact->capabilities().streamedMediaVideoCalls() && m_account->capabilities().streamedMediaVideoCalls());
    } else {
        return false;
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

void ContactWrapper::genericOperationFinished(Tp::PendingOperation* op)
{
    /// TODO send notification on error. Why doesn't the static method work??
    if (op->isError()) {
        QString errorMsg(op->errorName() + ": " + op->errorMessage());

//         KNotification::event("telepathyError", errorMsg);
//
//         KNotification *notification = new KNotification("telepathyError", );
//         KAboutData aboutData("ktelepathy",0,KLocalizedString(),0);
//         notification->setComponentData(KComponentData(aboutData));
//         notification->setText(errorMsg);
//         notification->sendEvent();
    }
}

bool ContactWrapper::isAccountOnline() const
{
    if (m_account) {
        if (m_account->currentPresence().type() != Tp::Presence::offline().type()) {
            return true;
        }
    }

    return false;
}

void ContactWrapper::onConnectionChanged(const Tp::ConnectionPtr& newConn)
{
    // connection needs to be valid
    if (newConn) {
        connect(newConn->contactManager().data(), SIGNAL(stateChanged(Tp::ContactListState)), this, SLOT(onContactManagerStateChanged(Tp::ContactListState)));
    }
}

void ContactWrapper::onContactManagerStateChanged(Tp::ContactListState newState)
{
    if (newState == Tp::ContactListStateSuccess) {
        QList<Tp::ContactPtr> contactList = m_account->connection()->contactManager()->allKnownContacts().toList();

        for (int i = 0; i < contactList.count(); ++i) {
            if (contactList.at(i)->id() == m_tempContactId) {
                setContact(contactList.at(i));
                break;
            }
        }
    }
}

QString ContactWrapper::presenceStatus() const
{
    if (m_contact) {
        return m_contact->presence().status();
    } else {
        return QString();
    }
}

void ContactWrapper::setupAccountConnects()
{
    // keep track of presence (online/offline is all we need)
    connect(m_account.data(), SIGNAL(connectionChanged(Tp::ConnectionPtr)), this, SLOT(onConnectionChanged(Tp::ConnectionPtr)));
    connect(m_account.data(), SIGNAL(currentPresenceChanged(Tp::Presence)), this, SIGNAL(accountPresenceChanged()));
}

void ContactWrapper::setupContactConnects()
{
    connect(m_contact.data(), SIGNAL(avatarDataChanged(Tp::AvatarData)), this, SIGNAL(avatarChanged()));
    connect(m_contact.data(), SIGNAL(presenceChanged(Tp::Presence)), this, SIGNAL(presenceChanged()));
}

void ContactWrapper::sendMail()
{
    if (!m_account || !m_contact) {
        return;
    }

    KToolInvocation::invokeMailer(KUrl(m_contact->id()));
}

void ContactWrapper::startAudioCall()
{
    kDebug();
}

void ContactWrapper::startFileTransfer()
{
    kDebug();
}

void ContactWrapper::startTextChat()
{
    if (!m_account || !m_contact) {
        return;
    }

    Tp::PendingChannelRequest* channelRequest = m_account->ensureTextChat(m_contact,
                                                                        QDateTime::currentDateTime(),
                                                                        PREFERRED_TEXTCHAT_HANDLER);
    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
}

void ContactWrapper::startVideoCall()
{
    kDebug();
}

void ContactWrapper::setAccount(const Tp::AccountPtr& relatedAccount)
{
    kDebug() << "setting account to: " << relatedAccount->displayName();
    undoAccountConnects();
    m_account = relatedAccount;
    setupAccountConnects();
}

void ContactWrapper::setContact(const Tp::ContactPtr& newContact)
{
    kDebug() << "setting new contact to: " << newContact->id();

    // disconnect signals
    undoContactConnects();
    m_contact = newContact;

    // establish new signals
    setupContactConnects();

    // tell QML we have a new contact
    emit(newContactSet());
}

void ContactWrapper::setTempAvatar(const QString& path)
{
    m_tempAvatar = path;

    // tell QML to change avatar shown
    emit(avatarChanged());
}

void ContactWrapper::setTempContactId(const QString& tempId)
{
    m_tempContactId = tempId;
}

void ContactWrapper::undoAccountConnects()
{
    if (m_account) {
        disconnect(m_account.data(), 0, 0, 0);
    }
}

void ContactWrapper::undoContactConnects()
{
    if (m_contact) {
        disconnect(m_contact.data(), 0, 0, 0);
    }
}


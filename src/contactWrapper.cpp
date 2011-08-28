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

#include <KToolInvocation>
#include <KUrl>

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/ContactCapabilities>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/Presence>

#define PREFERRED_TEXTCHAT_HANDLER "org.freedesktop.Telepathy.Client.KDE.TextUi"

ContactWrapper::ContactWrapper(QObject* parent)
    : QObject(parent)
    , m_account(0)
    , m_contact(0)
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
        } else {
            return QString("im-user");
        }
    } else {
        // return default icon
        return QString("im-user");
    }
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

void ContactWrapper::sendMail()
{
    if (!m_account || !m_contact) {
        return;
    }

    KToolInvocation::invokeMailer(KUrl(m_contact->id()));
}

void ContactWrapper::startAudioCall()
{
    qDebug("ContactWrapper::startAudioCall");
}

void ContactWrapper::startFileTransfer()
{
    qDebug("ContactWrapper::startFileTransfer");
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
    qDebug("ContactWrapper::startVideoCall");
}

void ContactWrapper::setContact(const Tp::ContactPtr& newContact, const Tp::AccountPtr &relatedAccount)
{
    qDebug() << "setting new contact to: " << newContact->id();
    qDebug() << "with account " << relatedAccount->displayName();

    // disconnect signals
    undoConnects();
    m_contact = newContact;
    m_account = relatedAccount;

    // establish new signals
    setupConnects();

    // tell QML we have a new contact
    emit(newContactSet());
}

void ContactWrapper::undoConnects()
{
    if (m_contact) {
        disconnect(m_contact.data(), 0, 0, 0);
    }
}


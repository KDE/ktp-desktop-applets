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

#include <KFileDialog>
#include <KLocalizedString>
#include <KToolInvocation>
#include <KUrl>
#include <KDebug>

#include <KTp/actions.h>

#include <TelepathyQt/AvatarData>
#include <TelepathyQt/ContactCapabilities>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/Presence>

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
    return m_contact && m_contact->fileTransferCapability();
}

bool ContactWrapper::canStartTextChat() const
{
    return m_contact && m_contact->textChatCapability();
}

bool ContactWrapper::canStartAudioCall() const
{
    return m_contact && m_contact->audioCallCapability();
}

bool ContactWrapper::canStartVideoCall() const
{
    return m_contact && m_contact->videoCallCapability();
}

KTp::ContactPtr ContactWrapper::contact() const
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
    return m_account && m_account->currentPresence().type() != Tp::Presence::offline().type();
}

bool ContactWrapper::isContactOnline() const
{
    if (m_contact && isAccountOnline()) {
        return m_contact->presence().type() != Tp::Presence::offline().type();
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

void ContactWrapper::updateProperties()
{
    emit accountOnlineChanged();

    emit presenceStatusChanged();
    emit contactOnlineChanged();

    emit avatarChanged();

    emit canSendFileChanged();
    emit canStartTextChatChanged();
    emit canStartAudioCallChanged();
    emit canStartVideoCallChanged();

    emit displayNameChanged();
}

QString ContactWrapper::presenceStatus() const
{
    if (m_contact && isAccountOnline()) {
        return m_contact->presence().status();
    } else {
        return QString();
    }
}

void ContactWrapper::connectAccountSignals()
{
    // keep track of presence (online/offline is all we need)
    connect(m_account.data(), SIGNAL(connectionChanged(Tp::ConnectionPtr)), this, SLOT(onConnectionChanged(Tp::ConnectionPtr)));
    connect(m_account.data(), SIGNAL(currentPresenceChanged(Tp::Presence)), this, SLOT(updateProperties()));
}

void ContactWrapper::connectContactSignals()
{
    connect(m_contact.data(), SIGNAL(avatarDataChanged(Tp::AvatarData)), this, SIGNAL(avatarChanged()));
    connect(m_contact.data(), SIGNAL(presenceChanged(Tp::Presence)), this, SLOT(updateProperties()));
}

void ContactWrapper::disconnectAccountSignals()
{
    if (m_account) {
        disconnect(m_account.data(), SIGNAL(connectionChanged(Tp::ConnectionPtr)), this, SLOT(onConnectionChanged(Tp::ConnectionPtr)));
        disconnect(m_account.data(), SIGNAL(currentPresenceChanged(Tp::Presence)), this, SLOT(updateProperties()));
    }
}

void ContactWrapper::disconnectContactSignals()
{
    if (m_contact) {
        disconnect(m_contact.data(), SIGNAL(avatarDataChanged(Tp::AvatarData)), this, SIGNAL(avatarChanged()));
        disconnect(m_contact.data(), SIGNAL(presenceChanged(Tp::Presence)), this, SLOT(updateProperties()));
    }
}

void ContactWrapper::sendMail()
{
    if (!m_account || !m_contact) {
        return;
    }

    KToolInvocation::invokeMailer(KUrl(m_contact->id()));
}

void ContactWrapper::startTextChat()
{
    if (!m_account || !m_contact) {
        return;
    }

    Tp::PendingChannelRequest *channelRequest = KTp::Actions::startChat(m_account, m_contact);
    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
}

void ContactWrapper::startAudioCall()
{
    if (!m_account || !m_contact) {
        return;
    }

    Tp::PendingChannelRequest *channelRequest = KTp::Actions::startAudioCall(m_account, m_contact);
    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
}

void ContactWrapper::startVideoCall()
{
    if (!m_account || !m_contact) {
        return;
    }

    Tp::PendingChannelRequest *channelRequest = KTp::Actions::startAudioVideoCall(m_account, m_contact);
    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
}

void ContactWrapper::startFileTransfer()
{
    if (!canSendFile()) {
        return;
    }

    QStringList filenames = KFileDialog::getOpenFileNames(KUrl("kfiledialog:///FileTransferLastDirectory"),
                                                          QString(),
                                                          0,
                                                          i18n("Choose files to send to %1", m_contact->alias()));

    Q_FOREACH (const QString &filename, filenames) {
        Tp::PendingOperation *channelRequest = KTp::Actions::startFileTransfer(m_account, m_contact, filename);
        connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
    }
}

void ContactWrapper::startFileTransfer(const QVariantList &urls)
{
    if (!canSendFile()) {
        return;
    }

    Q_FOREACH (const QVariant &urlVariant, urls) {
        if (urlVariant.toUrl().isLocalFile()) {
            Tp::PendingOperation *channelRequest = KTp::Actions::startFileTransfer(m_account, m_contact, urlVariant.toUrl());
            connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(genericOperationFinished(Tp::PendingOperation*)));
        }
    }
}

void ContactWrapper::setAccount(const Tp::AccountPtr& relatedAccount)
{
    disconnectAccountSignals();
    m_account = relatedAccount;
    connectAccountSignals();

    updateProperties();
}

void ContactWrapper::setContact(const Tp::ContactPtr& newContact)
{
    kDebug() << "setting new contact to: " << newContact->id();

    // disconnect signals
    disconnectContactSignals();
    m_contact = KTp::ContactPtr::qObjectCast(newContact);

    // establish new signals
    connectContactSignals();

    // tell QML we have a new contact
    emit contactChanged();

    updateProperties();
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

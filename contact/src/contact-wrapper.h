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

#ifndef CONTACT_WRAPPER_H
#define CONTACT_WRAPPER_H

#include <QtCore/QObject>

#include <TelepathyQt/Account>
#include <KTp/contact.h>

class ContactWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAccountOnline READ isAccountOnline NOTIFY accountOnlineChanged)
    Q_PROPERTY(bool isContactOnline READ isContactOnline NOTIFY contactOnlineChanged)
    Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)

    Q_PROPERTY(bool canSendFile READ canSendFile NOTIFY canSendFileChanged)
    Q_PROPERTY(bool canStartTextChat READ canStartTextChat NOTIFY canStartTextChatChanged)
    Q_PROPERTY(bool canStartAudioCall READ canStartAudioCall NOTIFY canStartAudioCallChanged)
    Q_PROPERTY(bool canStartVideoCall READ canStartVideoCall NOTIFY canStartVideoCallChanged)

    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString presenceStatus READ presenceStatus NOTIFY presenceStatusChanged)

public:
    ContactWrapper(QObject *parent = 0);
    virtual ~ContactWrapper();

    /** returns the account id related to the contact chosen */
    QString accountId() const;

    /** returns the avatar location for the contact */
    QString avatar() const;

    /** returns whether the contact can send files */
    bool canSendFile() const;

    /** returns whether the contact support textual chat */
    bool canStartTextChat() const;

    /** returns whether the contact can start/receive audio calls */
    bool canStartAudioCall() const;

    /** returns whether the contact can start/receive video */
    bool canStartVideoCall() const;

    /** returns current contact being represented */
    KTp::ContactPtr contact() const;

    /** returns the display name of the contact */
    QString displayName() const;

    /** returns whether the account is online/offline */
    bool isAccountOnline() const;

    /** returns whether the contact is online/offline */
    bool isContactOnline() const;

    /** returns the contact presence status (online, offlince ... ) */
    QString presenceStatus() const;

    /** set account to which the contact is related
     * @param relatedAccount account pointer
     */
    void setAccount(const Tp::AccountPtr &relatedAccount);

    /** set new contact to represent
     * @param newContact the contact to represent
     */
    void setContact(const Tp::ContactPtr &newContact);

    /** set temporary avatar image path. This is set for when KDE-Telepathy goes offline so the
     * plasmoid can still show an image even without the pointer to the contact
     * @param path path to avatar image
     */
    void setTempAvatar(const QString &path);

    void setTempContactId(const QString &tempId);

public slots:
    void sendMail();
    void startTextChat();
    void startAudioCall();
    void startVideoCall();
    void startFileTransfer();
    void startFileTransfer(const QVariantList &urls);

signals:
    void accountOnlineChanged();
    void avatarChanged();

    void canSendFileChanged();
    void canStartTextChatChanged();
    void canStartAudioCallChanged();
    void canStartVideoCallChanged();

    void displayNameChanged();
    void presenceStatusChanged();
    void contactOnlineChanged();

    void contactChanged();

private slots:
    void genericOperationFinished(Tp::PendingOperation *op);
    void onConnectionChanged(const Tp::ConnectionPtr &newConn);
    void onContactManagerStateChanged(Tp::ContactListState state);
    void updateProperties();

private:
    void connectAccountSignals();
    void connectContactSignals();
    void disconnectAccountSignals();
    void disconnectContactSignals();

    Tp::AccountPtr m_account;
    KTp::ContactPtr m_contact;
    QString m_tempAvatar;           /** this is the path to the cached avatar for when kde-telepathy is offline */
    QString m_tempContactId;
};

#endif  // CONTACT_WRAPER_H

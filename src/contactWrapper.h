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

#ifndef CONTACT_WRAPER_H
#define CONTACT_WRAPER_H

#include <QtCore/QObject>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/Contact>

class ContactWrapper : public QObject
{
    Q_OBJECT
public:
    ContactWrapper(QObject *parent = 0);
    virtual ~ContactWrapper();

    Q_PROPERTY(bool accountOnline READ isAccountOnline);
    Q_PROPERTY(QString avatar READ avatar);
    Q_PROPERTY(bool canSendFile READ canSendFile);
    Q_PROPERTY(bool canStartAudioCall READ canStartAudioCall);
    Q_PROPERTY(bool canStartVideo READ canStartVideo);
    Q_PROPERTY(QString displayName READ displayName);
    Q_PROPERTY(QString presenceStatus READ presenceStatus);

    /** returns the account id related to the contact chosen */
    QString accountId() const;

    /** returns the avatar location for the contact */
    QString avatar() const;

    /** returns wether the contact can send files */
    bool canSendFile() const;

    /** returns wether the contact can start/recieve audio calls */
    bool canStartAudioCall() const;

    /** returns wether the contact can start/recieve video */
    bool canStartVideo() const;

    /** returns current contact being rappresented */
    Tp::ContactPtr contact() const;

    /** returns the display name of the contact */
    QString displayName() const;

    /** returns wether the account is online/offline */
    bool isAccountOnline() const;

    /** returns the contact presence status (online, offlince ... ) */
    QString presenceStatus() const;

    /** set account to which the contact is related
     * @param relatedAccount account pointer
     */
    void setAccount(const Tp::AccountPtr &relatedAccount);

    /** set new contact to rappresent
     * @param newContact the contact to rappresent
     */
    void setContact(const Tp::ContactPtr &newContact);

    /** set temporary avatar image path. This is set for when KDE-Telepathy goes offline so the
     * plasmoid can still show an image even without the pointer to the contact
     * @param path path to avatar image
     */
    void setTempAvatar(const QString &path);

public slots:
    void sendMail();
    void startAudioCall();
    void startFileTransfer();
    void startTextChat();
    void startVideoCall();

signals:
    void accountPresenceChanged();
    void avatarChanged();
    void newContactSet();
    void presenceChanged();

private slots:
    void genericOperationFinished(Tp::PendingOperation *op);

private:
    void setupAccountConnects();
    void setupContactConnects();
    void undoAccountConnects();
    void undoContactConnects();

    Tp::AccountPtr m_account;
    Tp::ContactPtr m_contact;
    QString m_tempAvatar;           /** this is the path to the cached avatar for when kde-telepathy is offline */
};

#endif  // CONTACT_WRAPER_H

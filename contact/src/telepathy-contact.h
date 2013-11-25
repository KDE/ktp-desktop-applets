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

#ifndef TELEPATHY_CONTACT_H
#define TELEPATHY_CONTACT_H

#include <KIcon>

#include <Plasma/Applet>
#include <Plasma/DeclarativeWidget>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/Contact>

class Config;
class ContactWrapper;

class TelepathyContact: public Plasma::Applet
{
    Q_OBJECT
public:
    TelepathyContact(QObject *parent, const QVariantList &args);
    ~TelepathyContact();

    void init();
    void paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);

    /** overide of config signal */
    void showConfigurationInterface();

public slots:
    /** called from config dialog to set new contact
     * @param newContact Tp::ContactPtr to the new contact to use
     * @param relatedAccount Tp:AccountPtr to the account related to the contact
     */
    void setContact(const Tp::ContactPtr &newContact, const Tp::AccountPtr &relatedAccount);

private slots:
    void loadConfig();
    void configurationAccepted();
    void onAccountManagerReady(Tp::PendingOperation *op);

private:
    void saveConfig();
    void setupAccountManager();

    QString m_fileToLoad;
    QString m_accountPath;
    Plasma::DeclarativeWidget *m_declarative;
    ContactWrapper *m_contactWrapper;
    Tp::AccountManagerPtr m_accountManager;
};

#endif  // TELEPATHY_CONTACT_H

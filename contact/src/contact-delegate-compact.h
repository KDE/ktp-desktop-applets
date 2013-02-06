/*
 * Contact Delegate - compact version
 *
 * Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CONTACTDELEGATECOMPACT_H
#define CONTACTDELEGATECOMPACT_H

#include "abstract-contact-delegate.h"

class ContactDelegateCompact : public AbstractContactDelegate
{
    Q_OBJECT

public:
    enum ListSize {
        Normal,
        Mini
    };
    ContactDelegateCompact(ContactDelegateCompact::ListSize size = Normal, QObject *parent = 0);
    ~ContactDelegateCompact();

    void paintContact(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    QSize sizeHintContact(const QStyleOptionViewItem & option, const QModelIndex & index) const;

    void setListMode(ContactDelegateCompact::ListSize size);

private:
    int m_spacing;
    int m_avatarSize;
    int m_presenceIconSize;
    int m_accountIconSize;
    int m_clientTypeIconSize;
    ContactDelegateCompact::ListSize m_listSize;
};

#endif // CONTACTDELEGATECOMPACT_H

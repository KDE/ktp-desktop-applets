/*
 * Abstract Contact Delegate - base class for other delegates
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


#ifndef ABSTRACT_CONTACT_DELEGATE_H
#define ABSTRACT_CONTACT_DELEGATE_H

#include <QtGui/QStyledItemDelegate>


class AbstractContactDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    AbstractContactDelegate(QObject* parent = 0);
    virtual ~AbstractContactDelegate();

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

public Q_SLOTS:
    /**
     * Reimplements the help tooltip for the contact delegate.
     *
     * When the user hovers over a contact it will display their information like Alias, which contact belongs to what account,
     * is this contact blocked, their status message if their is one, etc.
     */
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index);

Q_SIGNALS:
    void repaintItem(QModelIndex);

protected:
    /** Paint contact items. Pure virtual, items should be subclass this for painting*/
    virtual void paintContact(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;
    /** The size hint for painting contact items*/
    virtual QSize sizeHintContact(const QStyleOptionViewItem& option, const QModelIndex& index) const = 0;

private:
    /** Paints header items*/
    void paintHeader(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHintHeader(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // ABSTRACT_CONTACT_DELEGATE_H

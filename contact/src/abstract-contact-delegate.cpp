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

#include "abstract-contact-delegate.h"

#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtGui/QHelpEvent>
#include <QAbstractItemView>

#include <KDE/KGlobalSettings>
#include <KDE/KLocale>
#include <KDE/KIconLoader>
#include <KDE/KIcon>

#include <KTp/types.h>
#include <KDebug>

const int SPACING = 2;
const int ACCOUNT_ICON_SIZE = 16;

AbstractContactDelegate::AbstractContactDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
{
}

AbstractContactDelegate::~AbstractContactDelegate()
{
}

void AbstractContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data(KTp::RowTypeRole).toInt() == KTp::ContactRowType) {
        paintContact(painter, option, index);
    } else {
        paintHeader(painter, option, index);
    }
}

QSize AbstractContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);

    if (index.data(KTp::RowTypeRole).toInt() == KTp::ContactRowType) {
        return sizeHintContact(option, index);
    } else {
        return sizeHintHeader(option, index);
    }
}


void AbstractContactDelegate::paintHeader(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optV4 = option;
    initStyleOption(&optV4, index);

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter->setClipRect(optV4.rect);

    QStyle *style = QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    QRect groupRect = optV4.rect;

    QRect accountGroupRect = groupRect;
    accountGroupRect.setSize(QSize(ACCOUNT_ICON_SIZE, ACCOUNT_ICON_SIZE));
    accountGroupRect.moveTo(QPoint(groupRect.left() + 2, groupRect.top() + 2));

    QRect groupLabelRect = groupRect;
    groupLabelRect.setRight(groupLabelRect.right() - SPACING);

    QRect expandSignRect = groupLabelRect;
    expandSignRect.setLeft(ACCOUNT_ICON_SIZE + (SPACING*5));
    expandSignRect.setRight(groupLabelRect.left() + 20); //keep it by the left side

    QFont groupFont = KGlobalSettings::smallestReadableFont();

    QString counts = QString(" (%1/%2)").arg(index.data(KTp::HeaderOnlineUsersRole).toString(),
                     index.data(KTp::HeaderTotalUsersRole).toString());

    if (index.data(KTp::RowTypeRole).toInt() == KTp::AccountRowType) {
        painter->drawPixmap(accountGroupRect, KIcon(index.data(Qt::DecorationRole).value<QIcon>()).pixmap(32));
    } else {
        painter->drawPixmap(accountGroupRect, KIconLoader::global()->loadIcon(QString("system-users"),
                            KIconLoader::Desktop));
    }

    //create an area for text which does not overlap with the icons.
    QRect textRect = groupLabelRect.adjusted(ACCOUNT_ICON_SIZE + (SPACING*4),0,0,0);
    QString groupHeaderString =  index.data(Qt::DisplayRole).toString().append(counts);

    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.color(QPalette::Active, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(QPalette::Active, QPalette::Text));
    }

    painter->setFont(groupFont);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignRight,
                      optV4.fontMetrics.elidedText(groupHeaderString, Qt::ElideRight, textRect.width()));


    QPen thinLinePen;
    thinLinePen.setWidth(0);
    thinLinePen.setColor(option.palette.color(QPalette::Inactive, QPalette::Button));

    painter->setPen(thinLinePen);
    painter->setRenderHint(QPainter::Antialiasing, false);

    QFontMetrics fm = painter->fontMetrics();
    int groupNameWidth = fm.width(groupHeaderString);

    //show a horizontal line padding the header if there is any space left.
    if (groupNameWidth < textRect.width()) {
        painter->drawLine(expandSignRect.right() + SPACING * 4,
                          groupRect.y() + groupRect.height() / 2,
                          groupRect.width() - groupNameWidth - SPACING * 2,
                          groupRect.y() + groupRect.height() / 2);
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    QStyleOption expandSignOption = option;
    expandSignOption.rect = expandSignRect;

    if (option.state & QStyle::State_Open) {
        style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &expandSignOption, painter);
    } else {
        style->drawPrimitive(QStyle::PE_IndicatorArrowRight, &expandSignOption, painter);
    }

    painter->restore();
}

QSize AbstractContactDelegate::sizeHintHeader(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(0,20);
}

bool AbstractContactDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(event)
    Q_UNUSED(view)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return false;
}

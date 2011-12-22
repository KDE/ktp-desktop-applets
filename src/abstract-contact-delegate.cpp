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

#include <QApplication>
#include <QtGui/QStyle>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtGui/QHelpEvent>
#include <QAbstractItemView>

#include <KDE/KGlobalSettings>
#include <KDE/KLocale>
#include <KDE/KIconLoader>
#include <KDE/KIcon>

#include <KTp/Models/accounts-model.h>
#include <KTp/Models/accounts-model-item.h>
#include <KTp/Models/groups-model.h>
#include <KTp/Models/contact-model-item.h>

const int SPACING = 2;
const int ACCOUNT_ICON_SIZE = 13;

AbstractContactDelegate::AbstractContactDelegate(QObject* parent)
    : QStyledItemDelegate(parent), m_palette(0)
{
    m_palette = new QPalette(QApplication::palette());
}

AbstractContactDelegate::~AbstractContactDelegate()
{
    delete m_palette;
}

void AbstractContactDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
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
    expandSignRect.setLeft(ACCOUNT_ICON_SIZE + SPACING + SPACING);
    expandSignRect.setRight(groupLabelRect.left() + 20); //keep it by the left side

    QFont groupFont = KGlobalSettings::smallestReadableFont();

    QString counts = QString(" (%1/%2)").arg(index.data(AccountsModel::OnlineUsersCountRole).toString(),
                   index.data(AccountsModel::TotalUsersCountRole).toString());

    if (index.data(AccountsModel::ItemRole).userType() == qMetaTypeId<AccountsModelItem*>()) {
        painter->drawPixmap(accountGroupRect, KIcon(index.data(AccountsModel::IconRole).toString())
        .pixmap(ACCOUNT_ICON_SIZE, ACCOUNT_ICON_SIZE));
    } else {
        painter->drawPixmap(accountGroupRect, KIconLoader::global()->loadIcon(QString("system-users"),
                                                                                      KIconLoader::Desktop));
    }

    painter->setPen(m_palette->color(QPalette::WindowText));
    painter->setFont(groupFont);
    painter->drawText(groupLabelRect, Qt::AlignVCenter | Qt::AlignRight,
                      index.data(GroupsModel::GroupNameRole).toString().append(counts));

    QPen thinLinePen;
    thinLinePen.setWidth(0);
    thinLinePen.setColor(m_palette->color(QPalette::Disabled, QPalette::Button));

    painter->setPen(thinLinePen);
    painter->setRenderHint(QPainter::Antialiasing, false);

    QFontMetrics fm = painter->fontMetrics();
    int groupNameWidth = fm.width(index.data(GroupsModel::GroupNameRole).toString().append(counts));

    painter->drawLine(expandSignRect.right() + SPACING * 2,
                      groupRect.y() + groupRect.height() / 2,
                      groupRect.width() - groupNameWidth - SPACING * 2,
                      groupRect.y() + groupRect.height() / 2);

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

QSize AbstractContactDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(0, 20);
}


bool AbstractContactDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(option);

    // Check and make sure that we only want it to work on contacts and nothing else.
    if (index.data(AccountsModel::ItemRole).userType() != qMetaTypeId<ContactModelItem*>()) {
        return false;
    }

    if (event->type() != QEvent::ToolTip) {
        return false;
    }

    const QString contactAvatar = index.data(AccountsModel::AvatarRole).toString();
    const QString displayName = index.parent().data(AccountsModel::DisplayNameRole).toString();
    const QString cmIconPath = KIconLoader::global()->iconPath(index.parent().data(AccountsModel::IconRole).toString(), 1);
    const QString alias = index.data(AccountsModel::AliasRole).toString();
    const QString id = index.data(AccountsModel::IdRole).toString();
    const QString presenceStatus = index.data(AccountsModel::PresenceMessageRole).toString();
    QString presenceIconPath;
    QString presenceText;

    switch (index.data(AccountsModel::PresenceTypeRole).toUInt()) {
        case Tp::ConnectionPresenceTypeAvailable:
            presenceIconPath = KIconLoader::global()->iconPath("user-online", 1);
            presenceText = i18nc("This is an IM user status", "Online");
            break;
        case Tp::ConnectionPresenceTypeAway:
            presenceIconPath = KIconLoader::global()->iconPath("user-away", 1);
            presenceText = i18nc("This is an IM user status", "Away");
            break;
        case Tp::ConnectionPresenceTypeExtendedAway:
            presenceIconPath = KIconLoader::global()->iconPath("user-away-extended", 1);
            presenceText = i18nc("This is an IM user status", "Away");
            break;
        case Tp::ConnectionPresenceTypeBusy:
            presenceIconPath = KIconLoader::global()->iconPath("user-busy", 1);
            presenceText = i18nc("This is an IM user status", "Busy");
            break;
        case Tp::ConnectionPresenceTypeHidden:
            presenceIconPath = KIconLoader::global()->iconPath("user-invisible", 1);
            presenceText = i18nc("This is an IM user status", "Invisible");
            break;
        case Tp::ConnectionPresenceTypeOffline:
            presenceIconPath = KIconLoader::global()->iconPath("user-offline", 1);
            presenceText = i18nc("This is an IM user status", "Offline");
            break;
        default:
            presenceIconPath = KIconLoader::global()->iconPath("task-attention", 1);
            // What presence Text should be here??
            break;
    }

    /* The tooltip is composed of a HTML table to display the items in it of the contact.
     * ---------------------------
     * |          | Con's Alias  |
     * |  Avatar  | (Con's Id)   |
     * |          ----------------
     * |          | Con's Status*|
     * ---------------------------
     * |  Contact is blocked*    |
     * ---------------------------
     * * Display actual status name if contact has no custom status message.
     * * Contact is blocked will only show if the contact is blocked, else no display.
     */

    QString table;
    table += QString("<table><tr><td rowspan='2' width='96'>");
    if (contactAvatar.isEmpty() || QPixmap(contactAvatar).isNull()) {
        table += QString("<img src='%1' width='96' />").arg(KIconLoader::global()->iconPath("im-user", -1));
    } else {
        table += QString("<img src='%1' width='96' />").arg(contactAvatar);
    }

    table += QString("</td>");
    table += QString("<td rowspan='2'><img src='%1' height='16' width='16' />&nbsp;</td>").arg(presenceIconPath);
    table += QString("<td><b>%1</b><br>(%2)</td></tr>").arg(alias).arg(id);
    table += QString("<tr><td>");
    table += QString("%2").arg(presenceStatus.isEmpty() ? presenceText : presenceStatus);
    table += QString("</td></tr>");
    if (index.data(AccountsModel::BlockedRole).toBool()) {
        table += QString("<tr><td colspan='2'>%1</td></tr>").arg(i18n("User is blocked"));
    }
    table += QString("</table>");

    QToolTip::showText(QCursor::pos(), table, view);

    return true;
}

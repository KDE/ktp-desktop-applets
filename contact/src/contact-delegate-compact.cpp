/*
 * Contact Delegate - compact version
 *
 * Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>
 * Copyright (C) 2012 Dominik Cermak <d.cermak@arcor.de>
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

#include "contact-delegate-compact.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QToolTip>
#include <QApplication>
#include <QStyle>
#include <QHelpEvent>

#include <KIconLoader>
#include <KIcon>
#include <KDebug>
#include <KGlobalSettings>
#include <KDE/KLocale>

#include <KTp/types.h>

ContactDelegateCompact::ContactDelegateCompact(ContactDelegateCompact::ListSize size, QObject * parent)
    : AbstractContactDelegate(parent)
{
    setListMode(size);
}

ContactDelegateCompact::~ContactDelegateCompact()
{

}

void ContactDelegateCompact::paintContact(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItemV4 optV4 = option;
    initStyleOption(&optV4, index);

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter->setClipRect(optV4.rect);

    QStyle *style = QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    QRect iconRect = optV4.rect;
    iconRect.setSize(QSize(m_avatarSize, m_avatarSize));
    iconRect.moveTo(QPoint(iconRect.x() + m_spacing, iconRect.y() + m_spacing));

    QPixmap avatar;
    avatar.load(index.data(KTp::ContactAvatarPathRole).toString());

    bool noContactAvatar = avatar.isNull();

    if (noContactAvatar) {
        avatar = SmallIcon("im-user", KIconLoader::SizeMedium);
    }

    style->drawItemPixmap(painter, iconRect, Qt::AlignCenter, avatar.scaled(iconRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // This value is used to set the correct width for the username and the presence message.
    int rightIconsWidth = m_presenceIconSize + m_spacing;

    QPixmap icon = KIcon(index.data(KTp::ContactPresenceIconRole).toString()).pixmap(KIconLoader::SizeSmallMedium);

    QRect statusIconRect = optV4.rect;

    statusIconRect.setSize(QSize(m_presenceIconSize, m_presenceIconSize));
    statusIconRect.moveTo(QPoint(optV4.rect.right() - rightIconsWidth,
                                 optV4.rect.top() + (optV4.rect.height() - m_presenceIconSize) / 2));

    painter->drawPixmap(statusIconRect, icon);

    // Right now we only check for 'phone', as that's the most interesting type.
    if (index.data(KTp::ContactClientTypesRole).toStringList().contains(QLatin1String("phone"))) {
        // Additional space is needed for the icons, don't add too much spacing between the two icons
        rightIconsWidth += m_clientTypeIconSize + (m_spacing / 2);

        QPixmap phone = QIcon::fromTheme("phone").pixmap(m_clientTypeIconSize);
        QRect phoneIconRect = optV4.rect;
        phoneIconRect.setSize(QSize(m_clientTypeIconSize, m_clientTypeIconSize));
        phoneIconRect.moveTo(QPoint(optV4.rect.right() - rightIconsWidth,
                                    optV4.rect.top() + (optV4.rect.height() - m_clientTypeIconSize) / 2));
        painter->drawPixmap(phoneIconRect, phone);
    }

    QFont nameFont;

    if (m_listSize == ContactDelegateCompact::Mini) {
        nameFont = KGlobalSettings::smallestReadableFont();
    } else {
        nameFont = KGlobalSettings::generalFont();
    }

    const QFontMetrics nameFontMetrics(nameFont);

    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.color(QPalette::Active, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(QPalette::Active, QPalette::Text));
    }

    painter->setFont(nameFont);

    QRect userNameRect = optV4.rect;
    userNameRect.setX(iconRect.x() + iconRect.width() + m_spacing * 2);
    userNameRect.setY(userNameRect.y() + (userNameRect.height()/2 - nameFontMetrics.height()/2));
    userNameRect.setWidth(userNameRect.width() - rightIconsWidth);

    painter->drawText(userNameRect,
                      nameFontMetrics.elidedText(optV4.text, Qt::ElideRight, userNameRect.width()));

    QRect presenceMessageRect = optV4.rect;
    presenceMessageRect.setX(userNameRect.x() + nameFontMetrics.boundingRect(optV4.text).width() + m_spacing * 2);
    presenceMessageRect.setWidth(optV4.rect.width() - presenceMessageRect.x() - rightIconsWidth);
    presenceMessageRect.setY(presenceMessageRect.y() + (presenceMessageRect.height()/2 - nameFontMetrics.height()/2));

    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(QPalette::Disabled, QPalette::Text));
    }

    painter->drawText(presenceMessageRect,
                      nameFontMetrics.elidedText(index.data(KTp::ContactPresenceMessageRole).toString().trimmed(),
                                                 Qt::ElideRight, presenceMessageRect.width()));

    painter->restore();
}

QSize ContactDelegateCompact::sizeHintContact(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(0, qMax(m_avatarSize + 2 * m_spacing, KGlobalSettings::smallestReadableFont().pixelSize() + m_spacing));
}

void ContactDelegateCompact::setListMode(ContactDelegateCompact::ListSize size)
{
    if (size == ContactDelegateCompact::Mini) {
        m_spacing = 2;
        m_avatarSize = IconSize(KIconLoader::Toolbar);
        m_presenceIconSize = qMax(12, KGlobalSettings::smallestReadableFont().pixelSize() + m_spacing);
        m_clientTypeIconSize = qMax(12, KGlobalSettings::smallestReadableFont().pixelSize() + m_spacing);
    } else if (size == ContactDelegateCompact::Normal) {
        m_spacing = 4;
        m_avatarSize = IconSize(KIconLoader::Toolbar);
        m_presenceIconSize = IconSize(KIconLoader::Small);
        m_clientTypeIconSize = IconSize(KIconLoader::Small);
    }

    m_listSize = size;
}


#include "contact-delegate-compact.moc"

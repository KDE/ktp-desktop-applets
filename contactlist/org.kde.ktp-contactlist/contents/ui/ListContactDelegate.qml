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

import QtQuick 2.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0 as ExtraComponents

PlasmaComponents.ListItem {
    id: delegate
    enabled: true
    property bool isCurrent: delegate.ListView.view.currentIndex==index

    ExtraComponents.QPixmapItem {
        id: avatarLabel
        width: height
        height: parent.height
        pixmap: model.avatarPixmap
        fillMode: ExtraComponents.QPixmapItem.PreserveAspectFit
        smooth: true

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
    }

    PlasmaComponents.Label {
        id: nickLabel
        text: model.display
        elide: Text.ElideRight

        //consider this a workaround, because the breeze's listitem doesn't
        //display very clearly what's the current item
        styleColor: theme.viewFocusColor
        style: delegate.isCurrent ? Text.Raised : Text.Normal

        anchors {
            left: avatarLabel.right
            right: presenceLabel.left
            leftMargin: 4
            verticalCenter: parent.verticalCenter
        }
    }

    ExtraComponents.QIconItem {
        id: presenceLabel
        width: height
        height: parent.height
        icon: presenceIcon
        anchors {
            right:parent.right
            leftMargin: 4
            verticalCenter: parent.verticalCenter
        }
    }
}

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

import Qt 4.7
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents

PlasmaComponents.ListItem {
    id: delegate;
    enabled: true
    
    height: 30;
//     width: parent.width;

    PlasmaWidgets.IconWidget {
        id: avatar;
        width: 22;
        height: parent.height;
        icon: QIcon("im-user");

        anchors {
            left: parent.left;
            leftMargin: 2;
            verticalCenter: parent.verticalCenter;
        }
    }

    PlasmaComponents.Label {
        id: nickLabel;
        text: aliasName;
        font.bold: true
        elide: Text.ElideRight

        anchors {
            left: avatar.right;
            right: presenceIcon.left
            leftMargin: 4;
            verticalCenter: parent.verticalCenter;
        }
    }

//     PlasmaComponents.Label {
//         id: presenceMessageLabel;
//         text: presenceMessage;
//         elide: Text.ElideRight;
//         font.italic: true;
// 
//         anchors {
//             left: nickLabel.right;
//             leftMargin: 4;
//             right: presenceIcon.left;
//             verticalCenter: parent.verticalCenter;
//         }
//     }
    
    PlasmaWidgets.IconWidget {
            id: presenceIcon
            width: 22
            height: parent.height
            icon: QIcon("user-online")
            anchors {
                right:parent.right
                leftMargin: 4
                verticalCenter: parent.verticalCenter
            }
    }

    
//     Rectangle {
//         color: "transparent";
//         border.color: "red";
//         anchors.fill: parent;
//     }

  
//     MouseArea {
//         anchors.fill: parent
//         onDoubleClicked: {
//             contactList.startChat(item);
//         }
//     }
}

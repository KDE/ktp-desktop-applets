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

Item {
    id: delegate;

    height: 30;
    width: parent.width;

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

    BorderImage {
        id: avatarFrame;
        width: 26;
        height: 26;

        border {
            left: 2
            right: 2
            top: 2
            bottom: 2
        }

        anchors {
            left: parent.left;
            verticalCenter: parent.verticalCenter;
        }
    }

    PlasmaComponents.Label {
        id: nickLabel;
        text: aliasName;
        font.bold: true;

        anchors {
            left: avatar.right;
            leftMargin: 4;
            verticalCenter: parent.verticalCenter;
        }
    }

    PlasmaComponents.Label {
        id: presenceMessageLabel;
        text: presenceMessage;
        elide: Text.ElideRight;
        font.italic: true;

        anchors {
            left: nickLabel.right;
            leftMargin: 4;
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
    }

//     Rectangle {
//         color: "transparent";
//         border.color: "red";
//         anchors.fill: parent;
//     }

    function setAvatarPresenceStatus(presenceStatus)
    {
        switch (presenceStatus) {
            case "available":
                avatarFrame.source = "../frames/online.png";
                if (!avatar.enabled) {
                    avatar.enabled = true;
                }
                break;
            case "dnd":
                avatarFrame.source = "../frames/busy.png";
                if (!avatar.enabled) {
                    avatar.enabled = true;
                }
                break;
            case "away":
                avatarFrame.source = "../frames/away.png";
                if (!avatar.enabled) {
                    avatar.enabled = true;
                }
                break;
            case "offline":
                avatarFrame.source = "../frames/offline.png";
                if (avatar.enabled) {
                    avatar.enabled = false;
                }
                break;
            default:
                avatarFrame.source = "../frames/offline.png";
                if (avatar.enabled) {
                    avatar.enabled = false;
                }
                break;
        }
    }

    MouseArea {
        anchors.fill: parent
        onDoubleClicked: {
            contactList.startChat(item);
        }
    }
}

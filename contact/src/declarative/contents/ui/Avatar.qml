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

import QtQuick 1.1
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

Item {
    id: container;

    // eliminate once finished. This is only a test image
    property string avatarPath: "";
    property string avatarPresenceStatus;

    signal clicked();

    anchors.fill: parent;

    Component.onCompleted: {
        setAvatarPresenceStatus(avatarPresenceStatus);
    }

    // TODO: use image instead of iconwidget?
    PlasmaWidgets.IconWidget {
        id: avatar;
        anchors.fill: parent;
        anchors.margins: 10;

        onClicked: {
            // toggleMenu
            container.clicked();
        }

        Component.onCompleted: {
            if (avatarPath == "") {
                avatar.icon = QIcon("im-user");
            } else {
                avatar.icon = QIcon(avatarPath);
            }
        }
    }

    BorderImage {
        id: avatarFrame;
        width: 128;
        height: 128;
    }

    // show drop-down action menu
    function showMenu()
    {
        console.log("SHOW MENU");
    }

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

    // updates avatar info with info from contact currently set
    function update()
    {
        avatar.icon = QIcon(TelepathyContact.avatar);
    }

    function accountPresenceChanged()
    {
        // check if user account is online
        if (!TelepathyContact.accountOnline && avatar.enabled) {
            avatar.enabled = false;
            setAvatarPresenceStatus("offline");
        } else if (TelepathyContact.accountOnline && !avatar.enabled) {
            avatar.enabled = true;
            // set back to normal
            avatarPresenceStatus = TelepathyContact.presenceStatus;
            setAvatarPresenceStatus(avatarPresenceStatus);
        }
    }
}

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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

/// TODO
// - add status border (colour like on the presence plasmoid)
// - bottom status bar with displayName on hover
// - rounded borders for image

Item {
    id: gridDelegate;

    property string delegateDisplayName;
    property string delegateAvatar;
//    property QIcon delegatePresenceIcon;
    property string delegatePresenceMessage;
//    property string delegatePresenceType;
    property string delegatePresenceName;

    // emitted when mouse hovers over contact
    signal setGridContactDisplayName(variant gridContactDisplayName, variant gridPresenceMessage);

    width: 40;
    height: 40;

    PlasmaWidgets.IconWidget {
        id: contactIcon;
        icon: QIcon("im-user");         // temp icon until it's sorted out
        anchors.fill: parent;
    }

    BorderImage {
        id: avatarFrame;
        width: parent.width;
        height: parent.height;

        border {
            left: 2
            right: 2
            top: 2
            bottom: 2
        }

        anchors {
            centerIn: gridDelegate;
        }
    }

    MouseArea {
        anchors.fill: parent;
        hoverEnabled: true;

        // set contact name in view
        onEntered: {
            gridDelegate.setGridContactDisplayName(delegateDisplayName, delegatePresenceMessage);
        }

        // unset contact name in view
        onExited: {
            gridDelegate.setGridContactDisplayName("", "");
        }
    }

    onDelegatePresenceNameChanged: {
        console.log("CHANGING BORDER to_ : " + delegatePresenceName);
        setAvatarPresenceStatus(delegatePresenceName);
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
}

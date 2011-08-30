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

Item {
    id: wrapper;

    // contact/person presence status
    property string status: "offline";

    anchors.fill: parent;

    Avatar {
        id: avatar;
        avatarPresenceStatus: wrapper.status;

        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        onClicked: {
            dropDownMenu.toggleMenu();
        }
    }

    DropDownMenu {
        id: dropDownMenu;
        width: wrapper.width;

        anchors {
            top: avatar.bottom;
            left: avatar.left;
            right: avatar.right;
        }
    }

    function update()
    {
        if (wrapper.status != TelepathyContact.presenceStatus) {
            wrapper.status = TelepathyContact.presenceStatus;
            avatar.setAvatarPresenceStatus(wrapper.status);
        }

        // update avatar
        avatar.update();
        dropDownMenu.update();
    }

    function accountPresenceChanged()
    {
        avatar.accountPresenceChanged();
    }
}

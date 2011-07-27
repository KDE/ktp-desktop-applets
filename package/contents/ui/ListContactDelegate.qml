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

Item {
    id: delegate;

    height: 30;
    width: parent.width;

    property string delegateDisplayName;
    property string delegateAvatar;

    Text {
        text: delegateDisplayName + delegateAvatar;
        anchors {
            right: parent.right;
        }
    }

    Rectangle {
        color: "transparent";
        border.color: "red";
        anchors.fill: parent;
    }

    PlasmaWidgets.IconWidget {
        width: 30;
        height: parent.height;
        icon: QIcon("im-user");

        anchors {
            left: parent.left;
            top: parent.top;        // not needed
            bottom: parent.bottom;  // not needed
        }

        Component.onCompleted: {
            console.log("LOADED: " + delegate.delegateAvatar);
        }
    }
}

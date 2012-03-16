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

/*
  This component is used ONLY by the contactlist in GRID view mode. It
  displays basic contact info when a contact is hovered over.
*/

import Qt 4.7
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

Item {
    property string contactNickToShow;
    property string contactPresenceMessage;

    // get color from current plasma theme
    property QtObject theme: PlasmaCore.Theme {}

    PlasmaWidgets.Label {
        id: idLabel;
        text: contactNickToShow;
        font.bold: true;
        width: parent.width/2;

        anchors {
            left: parent.left;
            verticalCenter: parent.verticalCenter;
        }
    }

    Text {
        id: presenceMessage;
        text: contactPresenceMessage;
        color: theme.textColor;

        font.italic: true;
        elide: Text.ElideRight;

        anchors {
            left: idLabel.right;
            leftMargin: 2;
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
    }
}

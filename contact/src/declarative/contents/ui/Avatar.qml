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
import org.kde.draganddrop 1.0 as DnD

Item {
    id: container;

    signal clicked();

    // TODO: use image instead of iconwidget?
    PlasmaWidgets.IconWidget {
        id: avatar;
        anchors.fill: parent;
        anchors.margins: 10;
        icon: QIcon(TelepathyContact.avatar);

        onClicked: {
            // toggleMenu
            container.clicked();
        }
    }

    BorderImage {
        id: avatarFrame;
        width: 128;
        height: 128;
        source: getFrameForAvatarPresence(TelepathyContact.presenceStatus)
    }

    DnD.DropArea {
        id: dropArea
        anchors.fill: parent
        enabled: TelepathyContact.canSendFile
        onDrop: {
            if (event.mimeData.url!="") {
                TelepathyContact.startFileTransfer(event.mimeData.urls)
            }
        }
    }

    function getFrameForAvatarPresence(presenceStatus)
    {
        switch (presenceStatus) {
            case "available":
                return "../frames/online.png";
            case "dnd":
                return "../frames/busy.png";
            case "away":
                return "../frames/away.png";
            default:
            case "offline":
                return "../frames/offline.png";
        }
    }
}

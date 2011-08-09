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
    // eliminate once finished. This is only a test image
    property string avatarPath: "/home/z3r0/Blog/hackergotchi_new.jpg";
    anchors.fill: parent;

    // make this a border image. Giulia will give me the frames soon
    Rectangle {
        id: borderStatus;

        color: "green";
        radius: 8;
        anchors.fill: parent;
    }

    // TODO: use image instead of iconwidget?
    PlasmaWidgets.IconWidget {
        id: avatar;
        icon: QIcon(avatarPath);
        anchors.fill: parent;
        anchors.margins: 10;
    }
//    Image {
//        id: avatar;
//        source: avatarPath;

//        anchors.fill: parent;
//        anchors.margins: 10;
//    }
}

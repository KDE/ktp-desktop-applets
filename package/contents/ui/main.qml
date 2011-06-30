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
// not yet ready. Waiting for grundelborg's fix and to accept my patch
//import org.kde.telepathy.declarativeplugins 0.1 as TelepathyDeclarative

Item {
    id: main;
    width: 200;
    height: 500;
    anchors.fill: parent;



    // set Label on the left
    ExternalLabel {
        id: label;
        width: 150;
        height: 30;

        Component.onCompleted: {
            setLabelOrientation();
        }

        onOrientationChanged: {
            setLabelOrientation();
        }
    }

    // set correct orientation for components according to the
    // contactlist orientation
    function setLabelOrientation()
    {
        switch (label.orientation) {
            case "left": {
                label.anchors.bottom = main.bottom;
                label.anchors.left = main.left;
                label.transformOrigin = Item.BottomLeft;
                label.rotation = 270;
            }
        }
    }
}

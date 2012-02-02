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
    id: main;

    // these settings are made to stay in bounds of the Plasma::DeclarativeWidget
    x: 10;
    y:10;

    // plasmoid starting size
    /// TODO load this from KConfig and read from C++
    width: 230;
    height: 380;


    // contact listview
    ContactList {
        id: contactList;
        anchors.fill: parent;
        state: "listView";   // start with list view
    }

    Rectangle {
        anchors.fill: parent;
        color: "transparent";
        border.color: "green"
    }

    function onWidthChanged()
    {
        main.width = TelepathyContactList.width - 20;
    }

    function onHeightChanged()
    {
        main.height = TelepathyContactList.height - 20;
    }


    // THIS WILL NOT BE NEEDED.
//    // set Label on the left
//    ExternalLabel {
//        id: label;
//        width: 150;
//        height: 30;

//        Component.onCompleted: {
//            setLabelOrientation();
//        }

//        onOrientationChanged: {
//            setLabelOrientation();
//        }
//    }

//    // set correct orientation for components according to the
//    // contactlist orientation
//    function setLabelOrientation()
//    {
//        switch (label.orientation) {
//            case "left": {
//                label.anchors.bottom = main.bottom;
//                label.anchors.left = main.left;
//                label.transformOrigin = Item.BottomLeft;
//                label.rotation = 270;
//            }
//        }
//    }
}

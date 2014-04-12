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

import QtQuick 2.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

Item {
    id: label;

    // this property holds which side the label is to be viewed. Default is left
    // and changes when user changes screen edge from settings ui.
    property string orientation: "left";
    signal orientationChanged();
    property QtObject theme: PlasmaCore.Theme {}

    PlasmaWidgets.IconWidget {
        id: labelIcon;
        icon: QIcon("kde-telepathy");
        drawBackground: false;
        text: i18n("ContactList");
        orientation: QtHorizontal;
        textBackgroundColor: theme.backgroundColor;
        anchors.fill: parent;
    }

    function setOrientation(newOrientation)
    {
        if (newOrientation == "left"
                || newOrientation == "right"
                || newOrientation == "bottom"
                || newOrientation == "top") {
            label.orientation = newOrientation;
            label.orientationChanged();
        } else {
            console.log("CAN'T SET FAILED ORIENTATION!");
        }
    }
}

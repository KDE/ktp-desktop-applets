/*
    Copyright (C) 2012  Lasath Fernando <kde@lasath.org>
    Copyright (C) 2012 David Edmundson <kde@davidedmundson.co.uk>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

import QtQuick 2.1
import org.kde.plasma.components 2.0 as PlasmaComponents

PlasmaComponents.Label {
    id: body

    signal contextMenuRequested(int x, int y)

    wrapMode: Text.Wrap
    width: view.width

    text: model.text
    textFormat: Text.RichText
    height: undefined

    onLinkActivated: {
        console.log("opening link: " + link);
        Qt.openUrlExternally(link);
    }

    //Hover to display the time when hovering a message
    PlasmaComponents.Label {
        height: undefined
        text: Qt.formatTime(model.time)
        anchors {
            top: parent.top
            right: parent.right
        }
        Rectangle {
            color: theme.backgroundColor
            anchors.fill: parent
            z: parent.z-1
            opacity: 0.8
            radius: 5
        }
        visible: mouseArea.containsMouse
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton | Qt.RightButton
        onClicked: body.contextMenuRequested(mouse.x, mouse.y)
    }
}

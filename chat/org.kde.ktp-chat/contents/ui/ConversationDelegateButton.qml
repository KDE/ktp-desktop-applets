/*
    Copyright (C) 2012  Lasath Fernando <kde@lasath.org>
    Copyright (C) 2012 David Edmundson <kde@davidedmundson.co.uk>
    Copyright (C) 2012 Aleix Pol <aleixpol@kde.org>

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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.qtextracomponents 0.1 as ExtraComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.draganddrop 1.0 as DnD
import org.kde.telepathy 0.1

PlasmaComponents.ToolButton
{
    property variant account
    property variant contact
    property alias avatar: icon.icon
    property alias nick: tooltip.mainText
    property alias presenceIconName: tooltip.image
    property alias overlay: overlayLoader.sourceComponent
    checked: base.currentIndex==index

    ExtraComponents.QIconItem {
        id: icon
        opacity: dropArea.dragging ? 0.5 : 1
        anchors {
            fill: parent
            margins: 5
        }

        Behavior on opacity { SmoothedAnimation { duration: 250; velocity: 0.01 } }
    }
    
    PlasmaCore.ToolTip {
      id: tooltip
      target: icon
    }
    
    DnD.DropArea {
        id: dropArea
        property bool dragging: false

        anchors.fill: parent
        DeclarativeKTpActions { id: actions }
        onDrop: if (event.mimeData.url!="") {
            actions.startFileTransfer(parent.account, parent.contact, event.mimeData.url)
            dragging=false
        }
        onDragEnter: dragging=true
        onDragLeave: dragging=false
    }
    
    Loader {
        id: overlayLoader
        anchors {
            right: parent.right
            top: parent.top
        }
        width: parent.width / 3
        height: parent.height / 3
    }
}

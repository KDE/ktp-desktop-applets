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

import QtQuick 2.1
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0 as ExtraComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.telepathy 0.1

MouseArea
{
    id: mouseArea

    property variant account
    property variant contact
    property alias avatar: icon.source
    property alias title: toolTip.mainText
    property alias presenceIcon: toolTip.icon
    property alias overlay: overlayLoader.sourceComponent
    property bool needsAttention: false
    property bool focused: base.currentIndex==index

    PlasmaCore.FrameSvgItem {
        id: frame

        anchors.fill: parent

        imagePath: "widgets/tasks"
        prefix: taskPrefix(mouseArea.focused        ? "focus" :
                           toolTip.containsMouse    ? "hover" :
                           mouseArea.needsAttention ? "attention"
                                                    : "normal")

        function taskPrefix(prefix) {
            var effectivePrefix;
            switch (plasmoid.location) {
                case PlasmaCore.Types.LeftEdge:
                    effectivePrefix = "west-" + prefix;
                    break;
                case PlasmaCore.Types.TopEdge:
                    effectivePrefix = "north-" + prefix;
                    break;
                case PlasmaCore.Types.RightEdge:
                    effectivePrefix = "east-" + prefix;
                    break;
                case PlasmaCore.Types.BottomEdge:
                    effectivePrefix = "south-" + prefix;
                    break;
            }
            if (!frame.hasElementPrefix(effectivePrefix)) {
                effectivePrefix = prefix;
            }
            return effectivePrefix;
        }

        PlasmaCore.IconItem {
            id: icon
            opacity: dropArea.containsDrag ? 0.5 : 1
            anchors.fill: parent

            Behavior on opacity { SmoothedAnimation { duration: 250; velocity: 0.01 } }
        }
    }

    PlasmaCore.ToolTipArea {
        id: toolTip

        active: !mouseArea.focused
        anchors.fill: parent
    }
    
    DropArea {
        id: dropArea

        anchors.fill: parent
        onDropped: {
            for(var url in drop.urls)
                telepathyManager.startFileTransfer(parent.account, parent.contact, url);
        }
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

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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.telepathy 0.1

ConversationDelegateButton {
    id: convButton
    property alias isCurrentConversation: dialog.visible
    property variant popupSide
    
    avatar: model.conversation.avatar
    title: model.conversation.title
    presenceIconName: model.conversation.presenceIcon.name
    account: model.conversation.account
    contact: model.conversation.targetContact
    onClicked: toggleVisibility()
    
    function toggleVisibility() { setVisible(!isCurrentConversation) }
    function openConversation() { base.currentIndex = index }
    function setVisible(v) { v ? openConversation() : closeConversation(); }
    function closeConversation() {
        if(base.currentIndex == index) {
            base.currentIndex = -1
        }
    }
    
    Component.onCompleted: setVisible(model.conversation.messages.shouldStartOpened)
    
    //FIXME: put in a loader to not slow down the model
    PlasmaCore.Dialog {
        id: dialog
        windowFlags: Qt.WindowStaysOnTopHint
        visible: base.currentIndex==index

        mainItem: ChatWidget {
            id: chatWidget
            width: 250
            height: 350
            conv: model.conversation

            onCloseRequested: closeConversation()
        }

        onVisibleChanged: {
            if(visible) {
                windowHide.hideWindowFromTaskbar(dialog.windowId)

                //if we are opening the dialog right away (e.g. started chat from pinned)
                //when we open the dialog by the button plasma will collapse because the
                //item is not positioned yet. Use the plasmoid root instead, in those cases
                var item = convButton.state==Component.Ready ? convButton : base;
                var point = dialog.popupPosition(item, convButton.popupSide);
                console.log("Showing dialog at (" + point.x + "," + point.y + ")");

                dialog.x = point.x;
                dialog.y = point.y;
                dialog.activateWindow();
            } else if(base.currentIndex == index) {
                closeConversation();
            }
            dialog.mainItem.focus = visible;
        }
    }

    // needed to let MessageModel know when messages are visible
    // so that it can acknowledge them properly
    Binding {
        target: model.conversation.messages
        property: "visibleToUser"
        value: dialog.visible
    }
    
    Connections {
        target: model.conversation.messages
        onVisibleToUserChanged: {
            if(!dialog.visible && model.conversation.messages.visibleToUser) {
                openConversation();
            }
        }
    }
    
    overlay: Rectangle {
        color: "red"
        radius: 3

        Text {
            id: text
            anchors.fill: parent

            font.pixelSize: parent.height
            text: model.conversation.messages.unreadCount
            color: "white"

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        visible: model.conversation.messages.unreadCount !== 0
    }
}

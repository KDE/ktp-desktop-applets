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
import QtQuick.Layouts 1.1
import org.kde.telepathy 0.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

FocusScope {
    id: chatWidget
    property Conversation conv

    signal closeRequested

    RowLayout {
        id: titleArea
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        spacing: 3
        height: 30

        PlasmaCore.IconItem {
            Layout.fillHeight: true

            source: conv.presenceIcon
        }

        PlasmaComponents.Label {
            Layout.fillHeight: true
            Layout.fillWidth: true

            text: conv.title
            elide: Text.ElideRight
        }

        PlasmaComponents.ToolButton {
            Layout.fillHeight: true
            checkable: true
            checked: pin.pinned

            iconSource: "rating"
            onClicked: pin.toggle()
            tooltip: i18n("Pin contact")
            ContactPin {
                id: pin
                model: pinnedModel
                contact: chatWidget.conv.targetContact
                account: chatWidget.conv.account
            }
        }

        PlasmaComponents.ToolButton {
            Layout.fillHeight: true
            iconSource: "view-conversation-balloon"
            tooltip: i18n("Move conversation into a window")
            onClicked: conv.delegateToProperClient()
        }

        PlasmaComponents.ToolButton {
            Layout.fillHeight: true
            iconSource: "dialog-close"
            tooltip: i18n("Close conversation")
            onClicked: conv.requestClose()
        }
    }

    ListView {
        id: view
        property bool followConversation: true

        anchors {
            top: titleArea.bottom
            left: parent.left
            right: parent.right
            bottom: disconnectedLabel.top
            rightMargin: 5
            leftMargin: 5
        }
        boundsBehavior: Flickable.StopAtBounds
        section.property: "senderAlias"
        section.delegate: PlasmaComponents.Label { text: section; font.bold: true; anchors.right: parent.right}
        clip: true

        //we need this so that scrolling down to the last element works properly
        //this means that all the list is in memory
        cacheBuffer: Math.max(0, contentHeight)

        delegate: Loader {
            Component.onCompleted: {
                switch(model.type) {
                    case MessagesModel.MessageTypeOutgoing:
//                             console.log("Type: MessagesModel::MessageTypeOutgoing");
                        source = "OutgoingDelegate.qml"
                        break;
                    case MessagesModel.MessageTypeAction:
//                             console.log("Type: MessagesModel::MessageTypeAction");
                        source = "ActionDelegate.qml";
                        break;
                    case MessagesModel.MessageTypeIncoming:
                    default:
                        source = "TextDelegate.qml";
                }
            }
        }

        model: conv.messages
        onMovementEnded: followConversation = atYEnd //we only follow the conversation if moved to the end

        onContentHeightChanged: {
            if(followConversation && contentHeight>height) {
                view.positionViewAtEnd()
            }
        }
    }

    PlasmaComponents.Label {
        id: disconnectedLabel
        visible: !conv.valid
        height: visible ? contentHeight : 0

        anchors {
            left: parent.left
            right: parent.right
            bottom: input.top
        }

        text: i18n("Chat is not connected. You cannot send messages at this time")
        wrapMode: Text.Wrap
    }

    PlasmaComponents.TextField {
        id: input
        focus: true
        enabled: conv.valid

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onAccepted: {
            view.model.sendNewMessage(text);
            text = "";
        }

        Keys.onEscapePressed: {
            if (length > 0) {
                text = ""
            } else {
                chatWidget.closeRequested()
            }
        }
    }
}

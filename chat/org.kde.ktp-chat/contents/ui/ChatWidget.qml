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
import org.kde.telepathy 0.1
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.qtextracomponents 0.1 as ExtraComponents
import org.kde.plasma.components 0.1 as PlasmaComponents

FocusScope {
    id: chatWidget
    property Conversation conv

    signal closeRequested

    Item {
        id: titleArea
        anchors {
            margins: 5
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: 24

        ExtraComponents.QIconItem {
            id: contactIcon
            anchors {
                top: parent.top
                left: parent.left
                bottom: parent.bottom
                margins: 3
            }
            width: height

            icon: conv.presenceIcon
        }

        PlasmaComponents.Label {
            id: contactName
            anchors {
                left: contactIcon.right
                right: pinButton.left
                top: parent.top
                bottom: parent.bottom
                leftMargin: 5
            }
            text: conv.title
            elide: Text.ElideRight
        }

        PlasmaComponents.ToolButton {
            id: pinButton

            anchors {
                top: parent.top
                right: minimizeButton.left
                bottom: parent.bottom
            }
            checkable: true
            checked: pin.pinned

            iconSource: "rating"
            onClicked: pin.toggle()
            ContactPin {
                id: pin
                model: pinnedModel
                contact: chatWidget.conv.targetContact
                account: chatWidget.conv.account
            }
        }

        PlasmaComponents.ToolButton {
            id: minimizeButton

            anchors {
                top: parent.top
                right: popoutButton.left
                bottom: parent.bottom
            }

            iconSource: "arrow-down"
            onClicked: closeRequested()
        }

        PlasmaComponents.ToolButton {
            id: popoutButton

            anchors {
                top: parent.top
                right: closeButton.left
                bottom: parent.bottom
            }

            iconSource: "view-conversation-balloon"

            onClicked: conv.delegateToProperClient()
        }

        PlasmaComponents.ToolButton {
            id: closeButton

            anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
            }

            iconSource: "dialog-close"

            onClicked: conv.requestClose()
        }
    }

    PlasmaWidgets.Separator {
        id: space
        anchors {
            top: titleArea.bottom
            left: parent.left
            right: parent.right
        }
        orientation: Qt.Horizontal
    }

    ListView {
        id: view
        property bool followConversation: true

        anchors {
            top: space.bottom
            left: parent.left
            right: parent.right
            bottom: disconnectedLabel.top
            rightMargin: viewScrollBar.width+5
            leftMargin: 5
        }
        boundsBehavior: Flickable.StopAtBounds
        section.property: "senderAlias"
        section.delegate: PlasmaComponents.Label { text: section; font.bold: true; anchors.right: parent.right}
        clip: true

        //we need this so that scrolling down to the last element works properly
        //this means that all the list is in memory
        cacheBuffer: contentHeight

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

    PlasmaComponents.ScrollBar {
        id: viewScrollBar
        anchors {
            top: view.top
            bottom: view.bottom
            right: parent.right
        }

        flickableItem: view
        orientation: Qt.Vertical
        opacity: view.atYEnd ? 0.3 : 1
        stepSize: 0

        Behavior on width { NumberAnimation { duration: 250 } }
        Behavior on opacity { NumberAnimation { duration: 250 } }
    }


    PlasmaComponents.Label {
        id: disconnectedLabel
        visible: !conv.valid

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

        Keys.onReturnPressed: {
            view.model.sendNewMessage(text);
            text = "";
        }
    }
}

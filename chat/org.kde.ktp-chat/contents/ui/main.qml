/*
    Copyright (C) 2012 Lasath Fernando <kde@lasath.org>
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

import QtQuick 1.0
import org.kde.telepathy 0.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as ExtraComponents

Grid {
    id: base
    property real minimumItemSize: 4
    property real minimumHeight: 1+(flow===Flow.LeftToRight ? minimumItemSize : base.childrenRect.height)
    property real minimumWidth: 1+(flow===Flow.LeftToRight ? base.childrenRect.width : minimumItemSize)
    property real itemSize: Math.min(base.width, base.height)
    property int currentIndex: -1

    clip: true
    spacing: 2
    flow: (plasmoid.formFactor === Vertical || width<height) ? Flow.TopToBottom : Flow.LeftToRight
    rows:    flow===Flow.LeftToRight ?  1 : -1
    columns: flow===Flow.LeftToRight ? -1 :  1

    TelepathyTextObserver { id: handler }
    HideWindowComponent { id: windowHide }

    Component.onCompleted: {
        plasmoid.aspectRatioMode = plasmoid.IgnoreAspectRatio
        plasmoid.addEventListener('activate', function() {
            base.currentIndex = handler.conversations.nextActiveConversation(base.currentIndex+1 % handler.conversations)
        });
    }

    Repeater {
        delegate: ConversationDelegateButton {
            width: base.itemSize
            height: width
            visible: available && !alreadyChatting
            onClicked: handler.conversations.startChat(account, contact)
            avatar: decoration
            overlay: ExtraComponents.QIconItem {
                    anchors.fill: parent
                    anchors.margins: parent.width/3
                    icon: presenceIcon
                }
        }
        model: PinnedContactsModel {
            id: pinnedModel
            conversations: handler.conversations
            accountManager: handler.accountManager

            Component.onCompleted: plasmoid.addEventListener('ConfigChanged', 
                                    function() {
                                        var v = plasmoid.readConfig("pinnedContacts");
                                        console.log("loading state", v)
                                        if(v!="")
                                            pinnedModel.state = v
                                    });
            onCountChanged: {
                plasmoid.writeConfig("pinnedContacts", pinnedModel.state)
                console.log("saving state", pinnedModel.state)
            }
        }
    }

    Repeater {
        delegate: ConversationDelegate {
            width: base.itemSize
            height: width
        }
        model: handler.conversations
    }
}

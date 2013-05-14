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
import org.kde.telepathy 0.1 as KTp
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1 as ExtraComponents

Grid {
    id: base
    property real minimumItemSize: 12
    property real implicitItemSize: Math.max(0, Math.min(width, height))
    property real implicitHeight: flow===Flow.TopToBottom ? itemsCount*implicitItemSize : itemsCount==0 ? 0 : implicitItemSize
    property real implicitWidth: flow===Flow.LeftToRight ? itemsCount*implicitItemSize : itemsCount==0 ? 0 : implicitItemSize
    property real minimumHeight: flow===Flow.TopToBottom ? itemsCount*minimumItemSize : itemsCount==0 ? 0 : minimumItemSize
    property real minimumWidth: flow===Flow.LeftToRight ? itemsCount*minimumItemSize : itemsCount==0 ? 0 : minimumItemSize
    property int currentIndex: -1
    property int itemsCount: pinnedView.count + conversationsView.count
    property real itemWidth: Math.max(0, flow===Flow.LeftToRight ? Math.min(height, width/itemsCount) : width)
    property real itemHeight:Math.max(0, flow===Flow.TopToBottom ? Math.min(width, height/itemsCount) : height)

    clip: true
    spacing: 2
    flow: (plasmoid.formFactor === Vertical   ? Flow.TopToBottom :
           plasmoid.formFactor === Horizontal ? Flow.LeftToRight :
                                 width<height ? Flow.TopToBottom : Flow.LeftToRight)
    rows:    flow===Flow.LeftToRight ?  1 : -1
    columns: flow===Flow.LeftToRight ? -1 :  1


    KTp.ConversationsModel {
        id: conversationsModel
    }

    KTp.HideWindowComponent {
        id: windowHide
    }

    Binding {
        target: plasmoid
        property: "status"
        value: (base.currentIndex >= 0                  ? AcceptingInputStatus
                : conversationsModel.totalUnreadCount>0 ? NeedsAttentionStatus
                : conversationsView.count>0             ? ActiveStatus
                                                        : PassiveStatus)
    }

    Component.onCompleted: {
        telepathyManager.addTextChatFeatures();
        telepathyManager.registerClient(conversationsModel, "KTp.ChatPlasmoid");
        plasmoid.aspectRatioMode = plasmoid.IgnoreAspectRatio
        plasmoid.addEventListener('activate', function() {
            base.currentIndex = conversationsModel.nextActiveConversation(base.currentIndex+1 % conversationsModel.count)
        });
    }
    Component.onDestruction: {
        telepathyManager.unregisterClient(conversationsModel);
    }

    Repeater {
        id: pinnedView
        delegate: ConversationDelegateButton {
            width: base.itemWidth
            height: base.itemHeight
            onClicked: telepathyManager.startChat(account, contact)
            avatar: decoration
            overlay: ExtraComponents.QIconItem {
                    anchors.fill: parent
                    anchors.margins: parent.width/3
                    icon: presenceIcon
                }
            account: model.account
            contact: model.contact
        }
        model: KTp.FilteredPinnedContactsProxyModel {
            sourceModel: KTp.PinnedContactsModel {
                id: pinnedModel
                conversations: conversationsModel
                accountManager: telepathyManager.accountManager

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
    }
    property variant popupSide:   plasmoid.location === TopEdge ?Qt.AlignBottom
                                : plasmoid.location === BottomEdge ? Qt.AlignTop
                                : plasmoid.location === LeftEdge ? Qt.AlignRight
                                : plasmoid.location === RightEdge ? Qt.AlignLeft
                                : base.flow === Flow.LeftToRight ? Qt.AlignBottom : Qt.AlignRight

    Repeater {
        id: conversationsView
        delegate: ConversationDelegate {
            width: base.itemWidth
            height: base.itemHeight
            popupSide: base.popupSide
        }
        model: conversationsModel
        onCountChanged: if(base.currentIndex>=conversationsView.count) {
            base.currentIndex = -1;
        }
    }
}


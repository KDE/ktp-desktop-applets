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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import org.kde.telepathy 0.1 as KTp
import org.kde.ktpchat 0.1 as KTpChat
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0 as ExtraComponents

Grid {
    id: base
    property real minimumItemSize: 12
    property real preferredItemSize: Math.max(0, Math.min(width, height))
    property real preferredLength: itemsCount*preferredItemSize+(spacing*itemsCount-1)
    property real preferredThickness: itemsCount==0 ? 0 : preferredItemSize
    property real minimumLength: itemsCount*minimumItemSize+(spacing*itemsCount-1)
    property real minimumThickness: itemsCount==0 ? 0 : minimumItemSize

    Layout.preferredHeight: flow===Flow.TopToBottom ? preferredLength : preferredThickness
    Layout.preferredWidth: flow===Flow.LeftToRight ? preferredLength : preferredThickness
    Layout.minimumHeight: flow===Flow.TopToBottom ? minimumLength : minimumThickness
    Layout.minimumWidth: flow===Flow.LeftToRight ? minimumLength : minimumThickness

    property int currentIndex: -1
    property int itemsCount: pinnedView.count + conversationsView.count
    property real itemWidth: Math.max(0, flow===Flow.LeftToRight ? Math.min(height, width/itemsCount) : width)
    property real itemHeight:Math.max(0, flow===Flow.TopToBottom ? Math.min(width, height/itemsCount) : height)

    clip: true
    spacing: 2
    flow: (plasmoid.formFactor === PlasmaCore.Types.Vertical   ? Flow.TopToBottom :
           plasmoid.formFactor === PlasmaCore.Types.Horizontal ? Flow.LeftToRight :
                                 width<height ? Flow.TopToBottom : Flow.LeftToRight)
    rows:    flow===Flow.LeftToRight ?  1 : -1
    columns: flow===Flow.LeftToRight ? -1 :  1

    function activate() {
        base.currentIndex = conversationsModel.nextActiveConversation(base.currentIndex+1 % conversationsModel.count)
    }

    function closeAllConversations() {
        base.currentIndex = -1
        conversationsModel.closeAllConversations()
    }

    KTp.ConversationsModel {
        id: conversationsModel
    }

    KTpChat.HideWindowComponent {
        id: windowHide
    }

    Binding {
        target: plasmoid
        property: "status"
        value: (base.currentIndex >= 0                  ? PlasmaCore.Types.AcceptingInputStatus
                : conversationsModel.totalUnreadCount>0 ? PlasmaCore.Types.NeedsAttentionStatus
                : conversationsView.count>0             ? PlasmaCore.Types.ActiveStatus
                                                        : PlasmaCore.Types.HiddenStatus)
    }

    Component.onCompleted: {
        telepathyManager.addTextChatFeatures();
        telepathyManager.registerClient(conversationsModel, "KTp.ChatPlasmoid");
    }
    Component.onDestruction: {
        telepathyManager.unregisterClient(conversationsModel);
    }

    Repeater {
        id: pinnedView
        delegate: ConversationDelegateButton {
            width: base.itemWidth
            height: base.itemHeight
            onClicked: {
                telepathyManager.startChat(account, contact, "org.freedesktop.Telepathy.Client.KTp.ChatPlasmoid")
            }
            avatar: decoration
            title: display
            overlay: PlasmaCore.IconItem { source: "rating" }
            presenceIcon: model.presenceIcon
            account: model.account
            contact: model.contact
        }
        model: KTp.FilteredPinnedContactsProxyModel {
            sourceModel: KTp.PinnedContactsModel {
                id: pinnedModel
                conversations: conversationsModel

                Component.onCompleted: {
                    state = plasmoid.configuration.pinnedContacts
                }

                onStateChanged: {
                    console.log("saving state", plasmoid.configuration.pinnedContacts)
                    plasmoid.configuration.pinnedContacts = pinnedModel.state
                }
            }
        }
    }

    property variant popupSide:   plasmoid.location === PlasmaCore.Types.TopEdge ?Qt.AlignBottom
                                : plasmoid.location === PlasmaCore.Types.BottomEdge ? Qt.AlignTop
                                : plasmoid.location === PlasmaCore.Types.LeftEdge ? Qt.AlignRight
                                : plasmoid.location === PlasmaCore.Types.RightEdge ? Qt.AlignLeft
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


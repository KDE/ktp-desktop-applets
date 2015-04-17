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
import QtQuick.Layouts 1.1
import org.kde.telepathy 0.1 as KTp
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.plasmoid 2.0

Item {
    id: root
    Layout.minimumHeight: delegateHeight*3
    Layout.minimumWidth: 100
    height: delegateHeight*10
    width: 300
    property real delegateHeight: Math.ceil(theme.mSize(theme.defaultFont).height*2)

    onVisibleChanged: {
        if (visible)
            filterLineEdit.forceActiveFocus();
    }

    Column {
        id: addAccountItem
        anchors.centerIn: parent
        width: parent.width
        visible: !ktpPresence.hasEnabledAccounts

        PlasmaComponents.Label {
            anchors.left: parent.left
            anchors.right: parent.right

            text: i18n("It appears that you do not have any accounts configured");

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        PlasmaComponents.Button {
            id: addAccountItemButton
            anchors.horizontalCenter: parent.horizontalCenter

            text: i18n("Configure Now...")
            onClicked: telepathyManager.showSettingsKCM();
        }
    }

    Column {
        id: goOnlineItem
        anchors.centerIn: parent
        visible: ktpPresence.presenceType == KTp.GlobalPresence.Offline && ktpPresence.hasEnabledAccounts

        PlasmaCore.IconItem {
            anchors.horizontalCenter: parent.horizontalCenter
            width: goOnlineButton.width
            height: width
            source: "user-online"
        }

        PlasmaComponents.Button {
            id: goOnlineButton

            text: i18n("Go Online")
            onClicked: ktpPresence.setPresence(KTp.GlobalPresence.Available, "")
        }
    }

    PlasmaComponents.TextField {
        id: filterLineEdit
        anchors {
            left:parent.left
            right:parent.right
            top:parent.top
        }

        visible: !goOnlineItem.visible && !addAccountItem.visible
        focus: true
        clearButtonShown: true

        placeholderText: i18n("Search Contacts...")

        Keys.onDownPressed: contactsList.incrementCurrentIndex();
        Keys.onUpPressed: contactsList.decrementCurrentIndex();
        Keys.onReturnPressed: contactsList.currentItem.clicked();

        onActiveFocusChanged: filterLineEdit.selectAll();
    }

    PlasmaExtras.ScrollArea {
        visible: !goOnlineItem.visible && !addAccountItem.visible
        anchors {
            top:filterLineEdit.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }

        contentItem: ListView {
            id: contactsList

            clip: true
            model: KTp.ContactsModel {
                id: contactsModel
                accountManager: telepathyManager.accountManager
                presenceTypeFilterFlags: KTp.ContactsModel.HideAllOffline
                globalFilterString: filterLineEdit.text
                sortRoleString: "presenceType"
            }


            boundsBehavior: Flickable.StopAtBounds

            delegate: ListContactDelegate {
                height: root.delegateHeight
                onClicked: {
                    plasmoid.expanded = false;
                    telepathyManager.startChat(model.account, model.contact, "org.freedesktop.Telepathy.Client.KTp.ChatPlasmoid");
                    filterLineEdit.text = "";
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    propagateComposedEvents: true
                    onEntered: contactsList.currentIndex = index
                }
            }

            highlight: PlasmaComponents.Highlight {
                hover: contactsList.focus
            }
            highlightMoveDuration: 0
        }
    }
}

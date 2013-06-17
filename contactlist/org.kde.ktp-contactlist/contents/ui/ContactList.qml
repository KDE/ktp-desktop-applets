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

import QtQuick 1.1
import org.kde.telepathy 0.1 as KTp
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras


Item {
    id: contactListContainer
    anchors.fill: parent

    PlasmaComponents.TextField {
        id: filterLineEdit
        anchors {
            left:parent.left
            right:parent.right
            top:parent.top
        }

        focus: true
        clearButtonShown: true

        placeholderText: i18n("Search contacts...")

        Keys.onDownPressed: contactsList.incrementCurrentIndex();
        Keys.onUpPressed: contactsList.decrementCurrentIndex();
        Keys.onReturnPressed: contactsList.currentItem.clicked();

        onActiveFocusChanged: filterLineEdit.selectAll();
    }

    PlasmaExtras.ScrollArea {
        anchors {
            top:filterLineEdit.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }

        flickableItem: ListView {
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

            delegate: ListContactDelegate {}

            highlight: PlasmaComponents.Highlight {
                hover: contactList.focus
            }
        }
    }

    function popupEventSlot(shown) {
        if (shown)
            filterLineEdit.forceActiveFocus();
    }

    Component.onCompleted: {
        plasmoid.popupEvent.connect(popupEventSlot);
    }
}

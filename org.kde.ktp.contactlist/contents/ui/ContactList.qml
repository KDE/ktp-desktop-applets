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

import Qt 4.7

import org.kde.telepathy.contactlist 0.1 as KtpContactList
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore


Item {
    id: contactListContainer;
    anchors.fill: parent;    
    
    KtpContactList.ContactList {
        id: contactList   
    }
    
    Component {
        id: highlightBar
        PlasmaCore.FrameSvgItem {
                imagePath: "widgets/viewitem"
                prefix: "hover"
                width: contactsList.width
                opacity: 0
                Behavior on opacity {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.OutQuad
                    }
                }
            }
    }
    
    ListView {
        id: contactsList;

        anchors {
            top: parent.top;
            topMargin: 5;
            left: parent.left;
            right: viewScrollBar.left;
            rightMargin: 1
            bottom: parent.bottom;
        }

        clip: true;
        model: contactList.model;
        boundsBehavior: Flickable.StopAtBounds

        delegate: ListContactDelegate {}
        highlight: highlightBar
        highlightMoveDuration: 250
        highlightMoveSpeed: 1
        highlightFollowsCurrentItem: true
        
        focus: true
        
        

    }
    
    PlasmaComponents.ScrollBar {
        id: viewScrollBar
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        flickableItem: contactsList
        interactive: true
        opacity: 1
        orientation: Qt.Vertical
    }
    
    Component.onCompleted : {
         contactList.filter.sortMode = KtpContactList.AccountsFilterModel.SortByPresence;
         contactList.filter.presenceTypeFilterFlags = KtpContactList.AccountsFilterModel.HideAllOffline
    }

}

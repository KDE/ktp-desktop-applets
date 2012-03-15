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



/// TODO
// - connect buttons in toolbar to switch view
// - (after above) animation from list to grid
// - add bar @ the bottom for gridview (hidden in list view)
// - get contact list to load nepomuk:/bla/foo/bar images as contact avatars
// - add side panel with common contact actions
// - highlight contacts on click
// - Use properties properly and get rid of all the stupid onSomethingChagned methods.


Item {
    id: contactListContainer;
    anchors.fill: parent;    
    
    KtpContactList.ContactList {
        id: contactList
        
    }
    
    ListView {
        id: contactsList;

        anchors {
            top: parent.top;
            topMargin: 5;
            left: parent.left;
            right: viewScrollBar.left;
            bottom: parent.bottom;
        }

        clip: true;
        model: contactList.model;
        boundsBehavior: Flickable.StopAtBounds

        delegate: ListContactDelegate {}
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
         contactList.filter.sortMode = SortByPresence;
         contactList.filter.presenceTypeFilterFlags = HideAllOffline;
    }

}

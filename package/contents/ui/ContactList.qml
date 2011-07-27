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

// import nepomuk models
import org.kde.telepathy.declarativeplugins 0.1 as TelepathyDeclarative

/// TODO
// - button to switch views
// - (after above) animation from list to grid
// - add bar @ the bottom for gridview (hidden in list view)
// - get contact list to load nepomuk:/bla/foo/bar images as contact avatars
// - add side panel with common contact actions
// - highlight contacts on click

Item {
    anchors.fill: parent;

    ListView {
        id: contactsList;
        anchors.fill: parent;
        model: TelepathyDeclarative.ContactListModel{}

        delegate:
            ListContactDelegate {
                delegateDisplayName: displayName;
            }
    }

    GridView {
        id: contactsGrid;
        anchors.fill: parent;
        model: TelepathyDeclarative.ContactListModel{}

        cellWidth: 48;
        cellHeight: 48;

        delegate:
            GridContactDelegate {
                delegateDisplayName: displayName;
                delegateAvatar: avatar;
            }
    }

    states: [
        State {
            name: "listView";
            PropertyChanges {
                target: contactsList;
                opacity: 1;
            }
            PropertyChanges {
                target: contactsGrid;
                opacity: 0;
            }
        },
        State {
            name: "gridView";
            PropertyChanges {
                target: contactsList;
                opacity: 0;
            }
            PropertyChanges {
                target: contactsGrid;
                opacity: 1;
            }
        }
    ]
}

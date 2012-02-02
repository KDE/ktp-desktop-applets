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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets


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


    
    
    // TOOLBAR
    ToolBar {
        id: toolbar;
        height: 20;

        anchors {
//             top: parent;
            left: parent.left;
            right: parent.right;
        }
    }

    // TOP SEPARATOR
    PlasmaWidgets.Separator {
        id: topSeparator;
        anchors {
            top: toolbar.bottom;
            topMargin: 5;
            left: parent.left;
            right:parent.right;
        }
    }

    // LISTVIEW
    ListView {
        id: contactsList;

        anchors {
            top: topSeparator.bottom;
            topMargin: 5;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        clip: true;
        model: contactListModel;

        delegate:
            ListContactDelegate {
                delegateDisplayName: aliasName;
                delegateAvatar: avatar;
//                delegatePresenceIcon: presenceIcon;
                delegatePresenceMessage: presenceMessage;
                delegatePresenceName: presenceName;
//                delegatePresenceType: presenceType;
            }
    }

    // GRIDVIEW
    GridView {
        id: contactsGrid;
        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        anchors {
            top: topSeparator.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            bottomMargin: 20;
        }

        model: contactListModel;

        cellWidth: 48;
        cellHeight: 48;

        delegate:
            GridContactDelegate {
                id: gridDelegate;
                
                //Dave - why does any of this exist just use the correct fucking names in the delegate.???
                delegateDisplayName: aliasName;
                delegateAvatar: avatar;
//                delegatePresenceIcon: presenceIcon;
                delegatePresenceName: presenceType;
                delegatePresenceMessage: presenceMessage;

                //Dave - this is a stupid name for the method. Be careful when using words like "set"
                onSetGridContactDisplayName: {
                    console.log("SETTING NAME TO: " + gridContactDisplayName + " with presence msg: " + gridPresenceMessage);
                    contactDisplay.contactNickToShow = gridContactDisplayName;
                    contactDisplay.contactPresenceMessage = gridPresenceMessage;
                }
            }
    }

    PlasmaWidgets.Separator {
        id: separator;
        anchors {
            top: contactsGrid.bottom;
            left: parent.left;
            right:parent.right;
            bottom: contactDisplay.top;
        }
    }

    ContactDisplayName {
        id: contactDisplay;

        anchors {
            top: contactsGrid.bottom;
            left: contactListContainer.left;
            right: contactListContainer.right;
            bottom: contactListContainer.bottom;
            margins: 5;
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
            PropertyChanges {
                target: contactDisplay;
                opacity: 0;
            }
            PropertyChanges {
                target: separator;
                opacity: 0
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
            PropertyChanges {
                target: contactDisplay;
                opacity: 1;
            }
            PropertyChanges {
                target: separator;
                opacity: 1;
            }
        }
    ]
}

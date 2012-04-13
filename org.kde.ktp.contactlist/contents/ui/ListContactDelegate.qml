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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.qtextracomponents 0.1 as ExtraComponents

Item {
    id: delegate;
    enabled: true
    
    height: 36;
    width: parent.width
    
    PlasmaCore.FrameSvgItem {
        id: padding
        imagePath: "widgets/viewitem"
        prefix: "hover"
        opacity: 0
        anchors.fill: parent
    }
    
    Item {
        anchors.fill: parent
        anchors.topMargin: padding.margins.top
        anchors.bottomMargin: padding.margins.bottom
        anchors.leftMargin: padding.margins.left
        anchors.rightMargin: padding.margins.right
        

        ExtraComponents.QIconItem {
            id: avatarLabel;
            width: 30;
            height: parent.height;
            
            icon : (avatar ? avatar : "im-user")
            anchors {
                left: parent.left;
                verticalCenter: parent.verticalCenter;
            }       
        }

        PlasmaComponents.Label {
            id: nickLabel;
            text: aliasName;
            font.bold: true
            elide: Text.ElideRight

            anchors {
                left: avatarLabel.right;
                right: presenceLabel.left
                leftMargin: 4;
                verticalCenter: parent.verticalCenter;
            }
        }
    
        ExtraComponents.QIconItem {
                id: presenceLabel
                width: 22
                height: parent.height
                icon: presenceIcon
                anchors {
                    right:parent.right
                    leftMargin: 4
                    verticalCenter: parent.verticalCenter
                }
        }
    }
      
    MouseArea {
        anchors.fill: parent
        
        hoverEnabled: true

        onDoubleClicked: {
            contactList.startChat(item);
        }
        
        onEntered: {
            contactsList.currentIndex = index;
            contactsList.highlightItem.opacity = 1;
        }
        onExited: {
            contactsList.highlightItem.opacity = 0;
        }
    }
}

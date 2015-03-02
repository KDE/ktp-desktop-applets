/***************************************************************************
 *   Copyright (C) 2014 by Aleix Pol Gonzalez <aleixpol@blue-systems.com>  *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

import QtQuick 2.4
import QtQuick.Layouts 1.1

import org.kde.people 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0

PlasmaCore.IconItem {
    id: personPhoto
    source: personData.person.photo

    property int minActionWidth

    function actionTriggered(id)
    {
        actions.triggerAction(id);
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            actionTriggered(0);
        }
    }

    PersonData {
        id: personData
        personUri: plasmoid.configuration.personUri
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        mainText: personData.person.name
        icon: personData.person.presenceIconName
    }

    FontMetrics {
        id: fontMetrics
    }

    Flow {
        id: actionsGrid
        anchors {
            left: parent.left
            right: parent.right
            bottom: flow == Flow.LeftToRight ? parent.bottom : undefined
            top: flow == Flow.TopToBottom ? parent.bottom : undefined
        }
        visible: height < parent.height && plasmoid.expanded
        flow: actionsRepeater.width > personPhoto.minActionWidth ? Flow.TopToBottom : Flow.LeftToRight

        move: Transition {
            NumberAnimation { properties: "x,y"; duration: 150 }
        }

        Repeater {
            id: actionsRepeater

            model: PersonActions {
                id: actions
                personUri: plasmoid.configuration.personUri
            }
            delegate: PlasmaComponents.Button {
                // Text width + a bit for the icon
                text: fontMetrics.advanceWidth(display) + units.gridUnit * 2 > actionsGrid.width ? "" : display
                iconName: model.iconName
                onClicked: model.action.trigger()
                tooltip: text == "" ? display : ""

                Component.onCompleted: {
                    var actionTextWidth = fontMetrics.advanceWidth(display);
                    if (personPhoto.minActionWidth < actionTextWidth) {
                        personPhoto.minActionWidth = actionTextWidth;
                    }
                    plasmoid.setAction(index, model.action.text, model.iconName)
                }
            }
        }
    }
}

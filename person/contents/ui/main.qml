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

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.people 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0

PlasmaCore.IconItem {
    source: p.person.photo

    PersonData {
        id: p
        personId: plasmoid.configuration.personId
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        mainText: p.person.name
        icon: p.person.presenceIconName
    }

    function actionTriggered(id)
    {
        actions.triggerAction(id);
    }

    GridLayout {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: height<parent.height

        Repeater {
            model: PersonActions {
                id: actions
                personId: plasmoid.configuration.personId
            }
            delegate: PlasmaComponents.Button {
                text: display
                iconName: model.iconName
                onClicked: model.action.trigger()

                Component.onCompleted: {
                    plasmoid.setAction(index, model.action.text, model.iconName)
                }
            }
        }
    }
}

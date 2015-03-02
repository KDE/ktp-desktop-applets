/***************************************************************************
 *   Copyright (C) 2014 by Aleix Pol Gonzalez <aleixpol@blue-systems.com>  *
 *   Copyright (C) 2015 by Martin Klapetek <mklapetek@kde.org>             *
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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0 as ExtraComponents
import org.kde.people 1.0

ColumnLayout
{
    id: configRoot
    property string cfg_personUri

    onCfg_personUriChanged: {
        if (cfg_personUri != "" && configItemSelector.filterString == "") {
            configItemSelector.filterString = cfg_personUri;
        }
    }

    SystemPalette { id: sysPalette; colorGroup: SystemPalette.Active }

    PersonsModel {
        id: personsModel

        onModelInitialized: {
            contactsList.enabled = true
        }
    }

    PlasmaCore.SortFilterModel {
        id: configItemSelector
        sourceModel: personsModel
        filterRole: "personUri"

        onFilterStringChanged: {
            if (filterString != "") {
                contactsList.currentIndex = sortProxy.mapRowFromSource(configItemSelector.mapRowToSource(0));
                contactsList.positionViewAtIndex(contactsList.currentIndex, GridView.Center);
            }
        }
    }

    Label {
        text: i18n("Select a person from the list below")
    }

    TextField {
        id: filterLineEdit

        Layout.fillWidth: true

        placeholderText: i18n("Search contacts...")
        onActiveFocusChanged: filterLineEdit.selectAll();
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        frameVisible: true
        highlightOnFocus: true


        GridView {
            id: contactsList

            width: parent.width
            activeFocusOnTab: true
            enabled: false
            focus: true
            currentIndex: -1

            cellWidth: units.gridUnit * 6
            cellHeight: units.gridUnit * 7

            model: PlasmaCore.SortFilterModel {
                id: sortProxy
                sourceModel: personsModel
                filterRole: "display"
                sortRole: "display"
                filterRegExp: filterLineEdit.text
            }

            boundsBehavior: Flickable.StopAtBounds
            highlightFollowsCurrentItem: true
            highlight: Rectangle {
                width: contactsList.cellWidth
                height: contactsList.cellHeight
                color: sysPalette.highlight
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Left) {
                    contactsList.moveCurrentIndexLeft()
                } else if (event.key == Qt.Key_Right) {
                    contactsList.moveCurrentIndexRight()
                } else if (event.key == Qt.Key_Up) {
                    contactsList.moveCurrentIndexUp()
                } else if (event.key == Qt.Key_Down) {
                    contactsList.moveCurrentIndexDown()
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0) {
                    configRoot.cfg_personUri = personsModel.get(model.mapRowToSource(currentIndex), PersonsModel.PersonUriRole);
                }
            }


            delegate: MouseArea {
                id: mouseArea

                width: contactsList.cellWidth
                height: contactsList.cellHeight

                onClicked: {
                    GridView.view.currentIndex = index
                }

                Column {
                    spacing: units.smallSpacing
                    anchors.fill: parent

                    // This is a padding item
                    Item {
                        height: units.smallSpacing
                        width: units.smallSpacing

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }

                    ExtraComponents.QPixmapItem {
                        id: avatarLabel

                        width: height
                        height: units.gridUnit * 4

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        pixmap: decoration
                        fillMode: ExtraComponents.QPixmapItem.PreserveAspectFit
                        smooth: true
                    }

                    Label {
                        id: nickLabel

                        width: mouseArea.width - (2 * units.smallSpacing)

                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        text: model.display
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        horizontalAlignment: TextInput.AlignHCenter
                        maximumLineCount: 2
                    }
                }
            }

            Component.onCompleted: {
                forceActiveFocus();
            }
        }
    }
}

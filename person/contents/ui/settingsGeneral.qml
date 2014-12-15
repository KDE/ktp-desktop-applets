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

import QtQuick 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.people 1.0

ColumnLayout
{
    id: configRoot
    property string cfg_personId

    Label {
        text: i18n("Selected Person:")
    }

    ComboBox {
        id: combo
        Layout.fillWidth: true
        model: PersonsModel {}
        textRole: "display"
        onActivated: {
            configRoot.cfg_personId = model.get(index, PersonsModel.PersonIdRole);
        }
    }

    Item {
        Layout.fillHeight: true
    }

    onCfg_personIdChanged: {
        restore();
    }
    function restore() {
        if (configRoot.cfg_personId == "")
            return;

        for(var i=0; i<combo.count; ++i) {
            var f = combo.model.get(i, PersonsModel.PersonIdRole);
            if (f == configRoot.cfg_personId) {
                combo.currentIndex = combo.find(combo.model.get(i, Qt.DisplayRole));
            }
        }
    }
}

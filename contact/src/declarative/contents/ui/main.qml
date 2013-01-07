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

Item {
    id: mainWidget;

    // default contact size (also in metadata file) just to be sure
    property int minimumWidth: 128;
    property int minimumHeight: 128;

    Contact {
        id: contact;
        anchors.centerIn: parent;
    }

    function updateContact()
    {
        contact.update();
    }

    function toggleDropDownMenu()
    {
        contact.toggleMenu();
    }

    function accountPresenceChanged()
    {
        contact.accountPresenceChanged();
    }
}

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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

Item {
    id: container;
    state: "hidden";

    Row {
        spacing: 2;
        anchors.centerIn: parent;

        PlasmaWidgets.IconWidget {
            id: chatButton;
            icon: QIcon("text-x-generic");
            width: 22;
            height: 22;

            enabled: TelepathyContact.isContactOnline && TelepathyContact.canStartTextChat;

            onClicked: {
                TelepathyContact.startTextChat();
                state = "hidden";
            }
        }

        PlasmaWidgets.IconWidget {
            id: callButton;
            icon: QIcon("audio-headset");
            width: 22;
            height: 22

            enabled: TelepathyContact.isContactOnline && TelepathyContact.canStartAudioCall;

            onClicked: {
                TelepathyContact.startAudioCall();
                state = "hidden";
            }
        }

        PlasmaWidgets.IconWidget {
            id: videoButton;
            icon: QIcon("camera-web");
            width: 22;
            height: 22;

            enabled: TelepathyContact.isContactOnline && TelepathyContact.canStartVideoCall;

            onClicked: {
                TelepathyContact.startVideoCall();
                state = "hidden";
            }
        }

        PlasmaWidgets.IconWidget {
            id: fileTransferButton;
            icon: QIcon("mail-attachment");
            width: 22;
            height: 22;

            enabled: TelepathyContact.isContactOnline && TelepathyContact.canSendFile;

            onClicked: {
                TelepathyContact.startFileTransfer();
                state = "hidden";
            }
        }

        PlasmaWidgets.IconWidget {
            id: mailButton;
            icon: QIcon("mail-message-new");
            width: 22;
            height: 22

            // mail button should be enabled even when the contact is offline
            enabled: TelepathyContact.isContactOnline;

            onClicked: {
                TelepathyContact.sendMail();
                state = "hidden";
            }
        }
    }

    states: [
        State {
            name: "visible";
            PropertyChanges {
                target: container;
                height: 30;
                opacity: 1;
            }
        },
        State {
            name: "hidden";
            PropertyChanges {
                target: container;
                height: 0;
                opacity: 0;
            }
        }
    ]

    transitions:
        Transition {
            NumberAnimation {
                properties: "height,opacity";
                easing.type: Easing.Linear;
            }
        }

    function toggleMenu()
    {
        if (state == "hidden") {
            state = "visible";
        } else {
            state = "hidden";
        }
    }
}

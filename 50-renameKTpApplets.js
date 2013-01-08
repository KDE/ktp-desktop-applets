/*
 * KTp Applets Migration Script
 *
 * Copyright (C) 2013 Dan Vrátil <dvratil@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 *
 * After migrating all Plasma applets to ktp-desktop-applets repository,
 * we renamed the to conform our naming policy. All applets are now called
 * org.kde.ktp-%appletname%.
 * This scripts replaces the old applets by new ones in order to make this
 * transition seemless for users.
 */
                  

var template = loadTemplate('org.kde.plasma-desktop.findWidgets')

function createWidget(widget, containment, newWidgetType)
{
  var wasLocked = containment.locked;
  containment.locked = false;
  
  var newWidget = containment.addWidget(newWidgetType);
  newWidget.geometry = widget.geometry;
  newWidget.index = widget.index;
  newWidget.globalShortcut = widget.shortcut;

  for (i = 0; i < widget.configKeys.length; i++) {
    var key = widget.configKeys[i];
    newWidget.writeConfig(key, widget.readConfig(key));
  }

  newWidget.locked = widget.locked;

  widget.locked = false;
  widget.remove();

  containment.locked = wasLocked;
}

/* Unlock widgets */
var wasLocked = locked;
locked = false;

template.findWidgets("org.kde.ktp-chatplasmoid", replaceChatPlasmoid);
template.findWidgets("org.kde.ktp.contactlist", replaceContactlist);
template.findWidgets("ktp_presence", replacePresence);
template.findWidgets("ktp_contact", replaceContact);


function replaceChatPlasmoid(widget, containment)
{
  createWidget(widget, containment, "org.kde.ktp-chat");
}

function replaceContactlist(widget, containment)
{
  createWidget(widget, containment, "org.kde.ktp-contactlist");
}

function replacePresence(widget, containment)
{
  createWidget(widget, containment, "org.kde.ktp-presence");
}

function replaceContact(widget, containment)
{
  createWidget(widget, containment, "org.kde.ktp-contact");
}

locked = wasLocked;

/*
    Copyright (C) 2016 James D. Smith <smithjd15@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef REGISTERCONTACTAPPLET_H
#define REGISTERCONTACTAPPLET_H

#include <QObject>

class RegisterContactApplet : public QObject
{
    Q_OBJECT
  public:
    explicit RegisterContactApplet(QObject *parent = 0);
    ~RegisterContactApplet();

};

#endif // REGISTERCONTACTAPPLET_H

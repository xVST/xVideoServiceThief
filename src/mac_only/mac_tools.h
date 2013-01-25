/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2012 Xesc & Technology
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with xVideoServiceThief. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: Xesc <xeskuu.xvst@gmail.com>
* Program URL   : http://xviservicethief.sourceforge.net/
*
*/

#ifndef MAC_TOOLS_H
#define MAC_TOOLS_H

class QString;

/*! Get if we are running Mac OS X 10.5.x or 10.6.x */
int getCurrentMacOSXVersion();
/*! Get if we are running Leopard (10.5.x) */
bool isRunningLeopard();
/*! Get if we are running Snow Leopard (10.6.x) */
bool isRunningSnowLeopard();
/*! Display a native Mac OS X native dialog */
int alert(int icon, QString messageText, QString informativeText, QString defaultButton, QString alternateButton, QString otherButton);

#endif //MAC_TOOLS_H

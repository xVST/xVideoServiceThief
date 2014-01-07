/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

#include "mac_tools.h"

#import <Cocoa/Cocoa.h>
#include <QMessageBox>

int getCurrentMacOSXVersion()
{
	// get system version
	static SInt32 systemVersion;
	Gestalt(gestaltSystemVersion, &systemVersion);
	// detect if we are running a MacOS Snow Leopard
	return systemVersion;
}

bool isRunningLeopard()
{
	return !isRunningSnowLeopard();
}

bool isRunningSnowLeopard()
{
	return getCurrentMacOSXVersion() >= 0x1060;
}

CFStringRef qstringToCFStringRef(const QString &string)
{
	return CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar *>(string.unicode()), string.length());
}

NSString *qstringToNSString(const QString &qstr)
{
    return [NSString stringWithUTF8String:qstr.toUtf8().data()];
}

int alert(int icon, QString messageText, QString informativeText, QString defaultButton, QString alternateButton, QString otherButton)
{
	// create native alert
	NSAlert *alert = [NSAlert alertWithMessageText:qstringToNSString(messageText)
									  defaultButton:qstringToNSString(defaultButton)
								   alternateButton:qstringToNSString(alternateButton)
									   otherButton:qstringToNSString(otherButton)
                         informativeTextWithFormat:qstringToNSString(informativeText), nil];
	// configure alert style
	switch (icon)
	{
		case QMessageBox::Warning		: [alert setAlertStyle:NSWarningAlertStyle]; break;
		case QMessageBox::Information	: [alert setAlertStyle:NSInformationalAlertStyle]; break;
		case QMessageBox::Critical		: [alert setAlertStyle:NSCriticalAlertStyle]; break;
	}
	// display alert
	NSInteger userSelection = [alert runModal];
	// convert result to Qt style
	if (otherButton != NULL && !otherButton.isEmpty()) userSelection = 2 - userSelection;
	else if (alternateButton != NULL && !alternateButton.isEmpty()) userSelection = 1 - userSelection;
	else userSelection = 0;
	// return user selection
	return userSelection;
}

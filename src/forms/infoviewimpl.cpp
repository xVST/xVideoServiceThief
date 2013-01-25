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

#include "infoviewimpl.h"

#include "../programversion.h"
#include "../videoitem.h"
#include "../videoinformation.h"

InfoViewImpl::InfoViewImpl(VideoItem *videoItem, QString pluginInfo,
						   QString userName, QString email, QString comments,
						   QWidget *parent, Qt::WFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
#ifdef Q_WS_MAC
	resize(500, 300);
#endif
	// fill text
	QStringList info;
	info 	<< "<p><b>Video Information:</b>\n"
			<< "Error Code: " + QString("%1").arg(videoItem->getErrorCode()) + "\n"
			<< "Video URL: " + videoItem->getURL() + "\n"
			<< "xVST Version: " + PROGRAM_VERSION + " (" + CURRENT_OS + ")" + "\n"
			<< "Plugin Version: " + pluginInfo + "\n"
			<< "FLV URL: " + videoItem->getVideoInformation().URL + "\n"
			<< "FLV Title: " + videoItem->getVideoInformation().title + "</p>"
			<< "<p><b>Sender Information:</b>\n"
			<< "User name: " + userName + "\n"
			<< "User eMail: " + email + "\n"
			<< "User Comments: " + comments + "</p>";
			
	foreach (QString line, info)
		rchInformation->setHtml(rchInformation->toHtml() + line);
}
//


/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2017 Xesc & Technology
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
* Program URL   : http://www.xvideothief.com/
*
*/

#include "multiurlsimpl.h"

#include "../videoinformation.h"

MultiURLsImpl::MultiURLsImpl(VideoInformation *videoInformation, QStringList selectedURLs, QStringList originalURLs,
							 QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// inits
	this->videoInformation = videoInformation;
	this->selectedURLs = selectedURLs;
	this->originalURLs = originalURLs;
	// build list
	foreach (QString url, originalURLs)
	{
		QListWidgetItem *item = new QListWidgetItem(QPixmap(videoInformation->getHostImage(url, true)), url, lstURLs);
		// set as checked?
		item->setCheckState(selectedURLs.contains(url) ? Qt::Checked : Qt::Unchecked);
	}
}

QStringList MultiURLsImpl::getSelectedURLs()
{
	selectedURLs.clear();
	// add only the checked urls
	for (int n = 0; n < lstURLs->count(); n++)
	{
		QListWidgetItem *item = lstURLs->item(n);
		// is it checked?
		if (item->checkState() == Qt::Checked)
			selectedURLs.append(item->text());
	}
	// return the selected urls
	return selectedURLs;
}

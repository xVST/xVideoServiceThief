/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2018 Xesc & Technology
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
* Program URL   : https://www.xvideothief.com/
*
*/

#include "downloadlogimpl.h"

#include "../sessionmanager.h"
#include "../videoinformation.h"
#include "../videolistcontroller.h"
#include "../tools.h"

DownloadLogImpl::DownloadLogImpl(VideoListController *videoList, QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
#ifdef Q_OS_MACX
	resize(900, 450);
#endif
	this->videoList = videoList;
	// add the headers
	QStringList headers;
	headers << tr("Date/Time") << tr("Video Title") << tr("URL");
	lsvLog->setHeaderLabels(headers);
	// resize sections (header)
	QHeaderView *header = lsvLog->header();
	header->resizeSection(0, 150);
	header->resizeSection(1, 250);
	// connect actions
	connect(actViewVideo, SIGNAL(triggered()), this, SLOT(viewVideoClicked())); // actViewVideo (clicked)
	connect(actDownloadAgain, SIGNAL(triggered()), this, SLOT(downloadAgainClicked())); // actDownloadAgain (clicked)
	// connect signals
	connect(lsvLog, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
	connect(lsvLog, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(lsvLogContextMenu(const QPoint &)));
}

void DownloadLogImpl::buildLog(QList<LogEntry> logEntries, VideoInformation *videoInformation)
{
	for (int n = 0; n < logEntries.count(); n++)
	{
		QTreeWidgetItem *item = new LogTreeWidgetItem(lsvLog);

		item->setText(0, logEntries.at(n).dateTime.toString());
		item->setText(1, logEntries.at(n).title);
		item->setText(2, logEntries.at(n).URL);

		item->setIcon(0, QIcon(videoInformation->getHostImage(logEntries.at(n).URL)));

		item->setSizeHint(0, QSize(18,18));

		item->setData(0, Qt::UserRole, logEntries.at(n).dateTime);
	}
	// sort list
	lsvLog->sortItems(0, Qt::AscendingOrder);
}

void DownloadLogImpl::itemDoubleClicked(QTreeWidgetItem *item, int)
{
	QDesktopServices::openUrl(item->text(2));
}

void DownloadLogImpl::lsvLogContextMenu(const QPoint &)
{
	if (lsvLog->selectedItems().count() > 0)
	{
		QMenu *menu = new QMenu(this);
		// Add actions
		menu->addAction(actViewVideo);
		menu->addAction(actDownloadAgain);
		// display popup
		menu->exec(QCursor::pos());
		// destrueix el submenu
		delete menu;
	}
}

void DownloadLogImpl::viewVideoClicked()
{
	foreach (QTreeWidgetItem *treeItem, lsvLog->selectedItems())
	{
		QDesktopServices::openUrl(treeItem->text(2));
	}
}

void DownloadLogImpl::downloadAgainClicked()
{
	foreach (QTreeWidgetItem *treeItem, lsvLog->selectedItems())
	{
		videoList->addVideo(treeItem->text(2));
	}
}
//


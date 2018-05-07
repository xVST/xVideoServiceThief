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
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef DOWNLOADLOGIMPL_H
#define DOWNLOADLOGIMPL_H

#include <QtWidgets>

#include "ui_downloadlog.h"

class VideoInformation;
class VideoListController;

struct LogEntry;

class LogTreeWidgetItem : public QTreeWidgetItem
{
	public:
		// constructors
		LogTreeWidgetItem(QTreeWidget *tree) : QTreeWidgetItem(tree) {}
		// custom comparation method
		bool operator< ( const QTreeWidgetItem & other ) const
		{
			QDateTime myDate = data(0, Qt::UserRole).toDateTime();
			QDateTime otherDate = other.data(0, Qt::UserRole).toDateTime();
			// compare
			return myDate < otherDate;
		}
};

class DownloadLogImpl : public QDialog, public Ui::DownloadLog
{
Q_OBJECT
	private:
		VideoListController *videoList;	//!< Video list controller reference
	public:
		DownloadLogImpl(VideoListController *videoList, QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowMaximizeButtonHint );
		/*! Display the downloads log */
		void buildLog(QList<LogEntry> logEntries, VideoInformation *videoInformation);
	private slots:
		void itemDoubleClicked(QTreeWidgetItem *item, int column);
		void lsvLogContextMenu(const QPoint &pos);
		// actions
		void viewVideoClicked();
		void downloadAgainClicked();

};
#endif






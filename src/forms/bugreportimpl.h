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
* Program URL   : http://xviservicethief.sourceforge.net/
*
*/

#ifndef BUGREPORTIMPL_H
#define BUGREPORTIMPL_H

#include <QDialog>

#include "ui_bugreport.h"

class ProgramOptions;
class VideoItem;
class VideoInformation;
class TrackerReport;

class BugReportImpl : public QDialog, public Ui::BugReport
{
Q_OBJECT
	private:
		ProgramOptions *programOptions;		//!< Program options reference
		VideoItem *videoItem; 				//!< VideoItem with the error
		TrackerReport *trackerReport;		//!< Tracker report class
		VideoInformation *videoInformation;	//!< Video service info
		/*! Get used plugin information */
		QString getPluginInformation(QString URL);
	public:
		/*! Class constructor */
		BugReportImpl(ProgramOptions *programOptions, QWidget * parent = 0, Qt::WindowFlags f = 0);
		/*! Class destructor */
		~BugReportImpl();
		/*! Fill the error information */
		void fillErrorInfo(VideoItem *videoItem, VideoInformation *videoInformation);
	private slots:
		void viewInfoClicked();
		void sendReportClicked();
		void cancelClicked();
		void trackerReportSent(QString result);
};
#endif

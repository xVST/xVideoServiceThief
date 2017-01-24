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

#ifndef TRACKERREPORT_H
#define TRACKERREPORT_H

#include <QtWidgets>

const QString SF_TRACKER_URL = "http://xviservicethief.sourceforge.net/tracker/bug.php";

class Http;

/*! SourceForge.net Tracker wrapper */
class TrackerReport : public QObject
{
Q_OBJECT
	protected:
		Http *http;				//!< Http class
		//! Get an unique id
		QString getUniqueID();
		/*! Send the tracker report */
		void sendTrackerReport();
	public:
		/*! Class constructor */
		TrackerReport();
		/*! Class destructor */
		~TrackerReport();
		/*! Send the tracker report */
		void sendTrackerReport(QString infoToSend);
	signals:
		/*! Abstract tracker report sent, this function is implemented in sub-classes */
		void trackerReportSent(QString result);
};
#endif

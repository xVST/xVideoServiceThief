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
#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QtWidgets>

#ifdef Q_OS_MACX
static const QString SESSION_FILE = "/../Application Support/xVideoServiceThief/session.plist";
static const QString LOG_FILE = "/../Application Support/xVideoServiceThief/historic.plist";
#else
static const QString SESSION_FILE = "/session.conf";
static const QString LOG_FILE = "/historic.log";
#endif

struct LogEntry
{
	QDateTime dateTime;	//!< date & time
	QString title;		//!< video title
	QString URL;		//!< video URL
};

class ProgramOptions;
class VideoListController;
class VideoItem;

class SessionManager : public QObject
{
Q_OBJECT
	private:
		ProgramOptions *programOptions;	//!< program options
		bool loading;	//!< Flag for know if we are already loading
	public:
		/*! Class constructor */
		SessionManager(ProgramOptions *programOptions);
		/*! Save the current session */
		void saveSession(VideoListController *videoListController);
		/*! Restore the last session */
		void loadSession(VideoListController *videoListController);
		/*! Add new entry to the log */
		void addToLog(VideoItem *videoItem);
		/*! Clear the current historic log */
		void clearLog();
		/*! Get all log entries */
		QList<LogEntry> getLogEntries();
};
#endif

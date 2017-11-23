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

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QtCore>

#ifdef Q_OS_MACX
static const QString SCHEDULE_FILE = "/../Application Support/xVideoServiceThief/schedules.plist";
#else
static const QString SCHEDULE_FILE = "/schedules.conf";
#endif

/*! Schedule item */
class ScheduleItem : public QObject
{
	private:
		QString comment;
		QTime startTime;
		QTime endTime;
		bool enabled;
	public:
		/*! Class constructor */
		ScheduleItem(bool enabled, QTime startTime, QTime endTime, QString comment = QString());
		/*! Set the start time */
		void setStartTime(QTime value);
		/*! Get the start time */
		QTime getStartTime();
		/*! Set the end time */
		void setEndTime(QTime value);
		/*! Get the end time */
		QTime getEndTime();
		/*! Set if this item is enabled */
		void setEnabled(bool value);
		/*! Get comment */
		QString getComment();
		/*! Set comment */
		void setComment(QString value);
		/*! Get if this item is enabled */
		bool isEnabled();
		/*! Get if this item is active (current time is between start and end times and is enabled) */
		bool isActive();
};

/*! Schedule controller */
class ScheduleController : public QObject
{
	private:
		QSettings::Format schedulesFileFormat;	//! Schedule file format
		QString schedulesFile;					//! Schedule file path
		QList<ScheduleItem *> *schedules;		//! List of schedules items
		bool enabled;							//! Flag for know if this controller is enabled
	public:
		/*! Class constructor */
		ScheduleController(QString schedulesBasePath, QSettings::Format format);
		/*! Class destructor */
		~ScheduleController();
		/*! Detrmine if we are into some of the configured ranges */
		bool canStart();
		/*! Save the current Schedule configuration */
		void save();
		/*! Open a saved Schedule configuration */
		void load();
		/*! Get if is enabled or disabled */
		bool isEnabled();
		/*! Set as enabled or disabled */
		void setEnabled(bool value);
		/*! Set as enabled */
		void enable();
		/*! Set as disabled */
		void disable();
		/*! Add a new schedule item */
		void addNewSchedule(bool enabled, QTime startTime, QTime endTime, QString comment = QString());
		/*! Remove a schedule item */
		void removeSchedule(int index);
		/*! Remove a schedule item */
		void removeSchedule(ScheduleItem *schedule);
		/*! Remove all schedules */
		void removeAllSchedules();
		/*! Get a schedule item */
		ScheduleItem *getScheduleItem(int index);
		/*! Get schedule count */
		int getSchedulesCount();
};

#endif // SCHEDULE_H

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

#include "schedule.h"

#include "options.h"

// ScheduleController class

ScheduleController::ScheduleController(QString schedulesBasePath, QSettings::Format schedulesFileFormat)
{
	schedulesFile = schedulesBasePath + SCHEDULE_FILE;
	this->schedulesFileFormat = schedulesFileFormat;
	schedules = new QList<ScheduleItem *>();
}

ScheduleController::~ScheduleController()
{
	removeAllSchedules();
	// remove all schedules
	delete schedules;
}

bool ScheduleController::canStart()
{
	// when the schedule controller is OFF then always can download items
	if (!isEnabled())
		return true;
	// find if we are into some of the active ranges
	for (int n = 0; n < getSchedulesCount(); n++)
		if (getScheduleItem(n)->isActive())
			return true;
	// nope, can't start
	return false;
}

void ScheduleController::save()
{
	if (QFile::exists(schedulesFile))
		QFile::remove(schedulesFile);

	QSettings settings(schedulesFile, schedulesFileFormat);

	for (int n = 0; n < schedules->count(); n++)
	{
		ScheduleItem *item = schedules->at(n);

		settings.beginGroup(QString("SCHEDULE_ITEM_%1").arg(n));
		settings.setValue("enabled", item->isEnabled());
		settings.setValue("startTime", item->getStartTime());
		settings.setValue("endTime", item->getEndTime());
		settings.setValue("comment", item->getComment());
		settings.endGroup();
	}
}

void ScheduleController::load()
{
	removeAllSchedules();

	QSettings settings(schedulesFile, schedulesFileFormat);

	QStringList items = settings.childGroups();

	for (int n = 0; n < items.count(); n++)
	{
		addNewSchedule(settings.value(items.at(n) + "/enabled", "").toBool(),
					   settings.value(items.at(n) + "/startTime", "").toTime(),
					   settings.value(items.at(n) + "/endTime", "").toTime(),
					   settings.value(items.at(n) + "/comment", "").toString());
	}
}

bool ScheduleController::isEnabled()
{
	return enabled;
}

void ScheduleController::setEnabled(bool value)
{
	enabled = value;
}

void ScheduleController::enable()
{
	setEnabled(true);
}

void ScheduleController::disable()
{
	setEnabled(false);
}

void ScheduleController::addNewSchedule(bool enabled, QTime startTime, QTime endTime, QString comment)
{
	schedules->append(new ScheduleItem(enabled, startTime, endTime, comment));
}

void ScheduleController::removeSchedule(int index)
{
	delete schedules->takeAt(index);
}

void ScheduleController::removeSchedule(ScheduleItem *schedule)
{
	removeSchedule(schedules->indexOf(schedule));
}

void ScheduleController::removeAllSchedules()
{
	while (!schedules->isEmpty())
		delete schedules->takeFirst();
}

ScheduleItem* ScheduleController::getScheduleItem(int index)
{
	return schedules->at(index);
}

int ScheduleController::getSchedulesCount()
{
	return schedules->count();
}

// ScheduleItem class

ScheduleItem::ScheduleItem(bool enabled, QTime startTime, QTime endTime, QString comment)
{
	setEnabled(enabled);
	setStartTime(startTime);
	setEndTime(endTime);
	setComment(comment);
}

void ScheduleItem::setStartTime(QTime value)
{
	startTime = value;
}

QTime ScheduleItem::getStartTime()
{
	return startTime;
}

void ScheduleItem::setEndTime(QTime value)
{
	endTime = value;
}

QTime ScheduleItem::getEndTime()
{
	return endTime;
}

void ScheduleItem::setEnabled(bool value)
{
	enabled = value;
}

bool ScheduleItem::isEnabled()
{
	return enabled;
}

QString ScheduleItem::getComment()
{
	return comment;
}

void ScheduleItem::setComment(QString value)
{
	comment = value;
}

bool ScheduleItem::isActive()
{
	return enabled && (QTime::currentTime() >= startTime) && (QTime::currentTime() <= endTime);
}

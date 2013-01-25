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

#include "trackerreport.h"

#include "http.h"
#include "tools.h"

TrackerReport::TrackerReport(QString groupID, QString atid)
{
	this->groupID = groupID;
	this->atid = atid;
	// http class
	http = new Http();
}

TrackerReport::~TrackerReport()
{
	delete http;
}

QString TrackerReport::getUniqueID()
{
	QString dateKey = QDateTime::currentDateTime().toString(QString("ddMMyyyyhhmmsszzz"));
	QString uniqueKey = "";

	qsrand(QDateTime::currentDateTime().toTime_t());
	int offset = qrand()%dateKey.length() - 1;
	int pos = offset + 1;
	
	while (offset != pos)
	{
		pos = pos < dateKey.length() - 1 ? pos + 1 : 0;
		uniqueKey += dateKey.at(pos);
	}
	
	return uniqueKey;
}

void TrackerReport::addCategory(QString caption, QString id)
{
	category[caption] = id;
}

void TrackerReport::addGroup(QString caption, QString id)
{
	groups[caption] = id;
}

void TrackerReport::sendTrackerReport(QString categoryCaption, QString groupCaption, 
	QString summary, QString description)
{
	QString parameters = "group_id=%1&atid=%2&func=postadd&category_id=%3&artifact_group_id=%4&summary=%5 (%6)&details=%7&submit=SUBMIT";
	
	QString html = http->downloadWebpagePost(SF_TRACKER_URL, 
						QString(parameters).arg(groupID).arg(atid)
											.arg(category[categoryCaption]).arg(groups[groupCaption])
											.arg(summary).arg(getUniqueID()).arg(description));

	emit trackerReportSent(copyBetween(html, "<h4 class=\"message\" id=\"top_feedback\">", "</h4>"));
}
//

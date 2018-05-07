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

#include "sessionmanager.h"

#include "options.h"
#include "videoitem.h"
#include "videolistcontroller.h"

SessionManager::SessionManager(ProgramOptions *programOptions)
{
	this->programOptions = programOptions;
	// init others
	loading = false;
}

void SessionManager::saveSession(VideoListController *videoListController)
{
	if (loading) return;

	QString sessionFile = programOptions->getOptionsPath() + SESSION_FILE;

	if (videoListController == NULL || !programOptions->getSaveRestoreSessions()) return;

	if (QFile::exists(sessionFile))
		QFile::remove(sessionFile);

	QSettings settings(sessionFile, programOptions->getOptionsFormat());

	for (int n = 0; n < videoListController->getVideoItemCount(); n++)
	{
		VideoItem *videoItem = videoListController->getVideoItem(n);
		bool canAdd = !videoItem->isDeleted();

		if (canAdd && programOptions->getDontRememberDownloadedVideos() && videoItem->isCompleted())
			canAdd = false;

		if (canAdd || !videoItem->isCompleted())
		{
			settings.beginGroup(QString("SESSION_VIDEO_%1").arg(n));

			settings.setValue("URL", videoItem->getURL());
			settings.setValue("videoFile", videoItem->getVideoFile());
			if (videoItem->isGettingURL())
				settings.setValue("videoState", vsNULL);
			else if (videoItem->isDownloading())
				settings.setValue("videoState", vsGettedURL);
			else if (videoItem->isConverting())
				settings.setValue("videoState", vsDownloaded);
			else if (videoItem->isResuming())
				settings.setValue("videoState", vsPaused);
			else
				settings.setValue("videoState", videoItem->getVideoState());
			settings.setValue("errorCode", videoItem->getErrorCode());
			settings.setValue("videoSize", videoItem->getVideoSize());
			settings.setValue("videoProgress", videoItem->getProgress());
			settings.setValue("customDownload", videoItem->isCustomDownload());
			settings.setValue("VIDEO_URL", videoItem->getVideoInformation().URL);
			settings.setValue("VIDEO_title", videoItem->getVideoInformation().title);
			settings.setValue("VIDEO_extension", videoItem->getVideoInformation().extension);
			settings.setValue("VIDEO_audioFile", videoItem->getVideoInformation().isAudioFile);
			settings.endGroup();
		}
	}
}

void SessionManager::loadSession(VideoListController *videoListController)
{
	QString sessionFile = programOptions->getOptionsPath() + SESSION_FILE;

	if (videoListController == NULL ||
	    !programOptions->getSaveRestoreSessions() || !QFile::exists(sessionFile)) return;

	loading = true;

	QSettings settings(sessionFile, programOptions->getOptionsFormat());

	QStringList videos = settings.childGroups();

	for (int n = 0; n < videos.count(); n++)
	{
		VideoItem *videoItem = new VideoItem(settings.value(videos.at(n) + "/URL", "").toString());
		videoItem->setVideoFile(settings.value(videos.at(n) + "/videoFile", "").toString());
		videoItem->setState(static_cast<VideoState>(settings.value(videos.at(n) + "/videoState", 0).toInt()));
		videoItem->setVideoSize(settings.value(videos.at(n) + "/videoSize", 0).toInt());
		videoItem->setProgress(settings.value(videos.at(n) + "/videoProgress", 0).toDouble());
		videoItem->setErrorCode(settings.value(videos.at(n) + "/errorCode", 0).toInt());
		VideoDefinition videoInformation;
		videoInformation.URL = settings.value(videos.at(n) + "/VIDEO_URL", "").toString();
		videoInformation.title = settings.value(videos.at(n) + "/VIDEO_title", "").toString();
		videoInformation.extension = settings.value(videos.at(n) + "/VIDEO_extension", ".flv").toString();
		videoItem->setVideoInformation(videoInformation);
		if (videoItem->hasErrors()) videoItem->setAsReported();
		if (settings.value(videos.at(n) + "/customDownload", false).toBool()) videoItem->setAsCustomDownload();
		if (!videoItem->isReady() && !videoItem->isReadyAndPaused() && !videoItem->isNULL() && !videoItem->isCanceled()
			&& !QFile::exists(videoItem->getVideoFile())) continue;
		videoListController->addVideo(videoItem);
	}
	// unlock
	loading = false;
}

void SessionManager::addToLog(VideoItem *videoItem)
{
	if (loading) return;

	if (videoItem == NULL || !programOptions->getSaveLogDownloadedVideos()) return;

	QSettings settings(programOptions->getOptionsPath() + LOG_FILE, programOptions->getOptionsFormat());

	int lastID = settings.value("information/lastID", 0).toInt() + 1;

	settings.beginGroup("information");
	settings.setValue("lastID", lastID);
	settings.endGroup();

	settings.beginGroup(QString("historic%1").arg(lastID));
	settings.setValue("date", QDateTime::currentDateTime());
	settings.setValue("title", videoItem->getVideoInformation().title);
	settings.setValue("URL", videoItem->getURL());
	settings.endGroup();
}

void SessionManager::clearLog()
{
	if (QFile::exists(programOptions->getOptionsPath() + LOG_FILE))
		QFile::remove(programOptions->getOptionsPath() + LOG_FILE);
}

QList<LogEntry> SessionManager::getLogEntries()
{
	QList<LogEntry> results;

	QSettings settings(programOptions->getOptionsPath() + LOG_FILE, programOptions->getOptionsFormat());

	QStringList historics = settings.childGroups();

	for (int n = 0; n < historics.count(); n++)
		if (historics.at(n).toLower() != "information")
		{
			LogEntry item;
			// set info
			item.dateTime = settings.value(historics.at(n) + "/date", item.dateTime).toDateTime();
			item.title = settings.value(historics.at(n) + "/title", item.title).toString();
			item.URL = settings.value(historics.at(n) + "/URL", item.URL).toString();
			// add it
			results << item;
		}

	return results;
}
//

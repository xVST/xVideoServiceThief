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

#include "videoitem.h"

#include "tools.h"

int VideoItem::internalID = 0;

VideoItem::VideoItem()
{
	locker = nullptr;
	assignID();
	// init data
	initData();
}

VideoItem::VideoItem(const QString URL)
{
	locker = nullptr;
	assignID();
	// init data
	initData();
	// set initial data
	this->URL = URL;
}

VideoItem::VideoItem(const QString URL, const OverridedVideoConversionConfig overridedConversionConfig)
{
	locker = nullptr;
	assignID();
	// init data
	initData();
	// set initial data
	this->URL = URL;
	overrideConversionConfig = true;
	overridedConvConf = overridedConversionConfig;
}

void VideoItem::assign(VideoItem *videoItem)
{
	URL = videoItem->getURL();
	videoFile = videoItem->getVideoFile();
	videoFileSavedTo = videoItem->getVideoFileSavedTo();
	progress = videoItem->getProgress();
	videoSize = videoItem->getVideoSize();
	downloadSpeed = videoItem->getDownloadSpeed();
	timeRemaining = videoItem->getTimeRemaining();
	videoState = videoItem->getVideoState();
	videoInfo.URL = videoItem->getVideoInformation().URL;
	videoInfo.title = videoItem->getVideoInformation().title;
	videoInfo.extension = videoItem->getVideoInformation().extension;
	videoInfo.needLogin = videoItem->getVideoInformation().needLogin;
	videoInfo.isAudioFile = videoItem->getVideoInformation().isAudioFile;
	reported = videoItem->isReported();
	errorCode = videoItem->getErrorCode();
    errorMessage = videoItem->errorMessage;
	overrideConversionConfig = videoItem->overrideConversionConfig;
	overridedConvConf = videoItem->overridedConvConf;
	customItemDownload = videoItem->isCustomDownload();
}

void VideoItem::initData()
{
	URL = "";
	videoFile = "";
	videoFileSavedTo = "";
	progress = 0;
	videoSize = 0;
	downloadSpeed = 0;
	timeRemaining = 0;
	videoState = vsNULL;
	videoInfo.URL = "";
	videoInfo.title = "";
	reported = false;
	errorCode = 0;
    errorMessage = "";
	audioFile = false;
	overrideConversionConfig = false;
	customItemDownload = false;
	updateSate = vusNothing;
	videoPreState = vpsNothing;
}

void VideoItem::assignID()
{
	internalID += 1;
	ID = internalID;
}

bool VideoItem::lock(QObject *locker)
{
	if (!isLocked())
	{
		this->locker = locker;
		return true;
	}
	else
		return false;
}

bool VideoItem::unlock(QObject *locker)
{
	if (isLocked())
		if (this->locker == locker)
		{
			this->locker = nullptr;
			return true;
		}
		else
			return false;
	else
		return false;
}

bool VideoItem::isLocked()
{
	return locker != nullptr;
}

VideoState VideoItem::getVideoState()
{
	return videoState;
}

QString VideoItem::getVideoStateAsString()
{
	switch (videoState)
	{
		case vsNULL:
			return tr("-");
		case vsGettingURL:
			return videoPreState == vpsNothing ? tr("Getting info...") : tr("Updating info...");
		case vsGettedURL:
			return videoPreState == vpsNothing ? tr("Ready") : tr("Queued...");
		case vsDownloading:
			return tr("Downloading...");
		case vsDownloaded:
			return tr("Downloaded");
		case vsConverting:
			return tr("Converting...");
		case vsConverted:
			return tr("Converted");
		case vsCompleted:
			return tr("Completed");
		case vsCanceled:
			return tr("Canceled");
		case vsBlocked:
			return tr("Blocked");
		case vsError:
			return tr("Error");
		case vsDeleted:
			return tr("Deleted");
		case vsPaused:
			return tr("Paused");
		case vsResuming:
			return tr("Resuming...");
		case vsNeedLogin:
			return tr("Need login...");
		case vsReadyAndPaused:
			return tr("Paused");
		case vsIsPlaylist:
			return tr("Playlist");
	}
	// default value
	return "-";
}

bool VideoItem::isNULL()
{
	return videoState == vsNULL;
}

bool VideoItem::isReady()
{
	return videoState == vsGettedURL;
}

bool VideoItem::isReadyAndPaused()
{
	return videoState == vsReadyAndPaused;
}

bool VideoItem::isGettingURL()
{
	return videoState == vsGettingURL;
}

bool VideoItem::isGettedURL()
{
	return videoState == vsGettedURL;
}

bool VideoItem::isDownloadable()
{
	return isReady() || isCanceled();
}

bool VideoItem::isDownloading()
{
	return videoState == vsDownloading;
}

bool VideoItem::isDownloaded()
{
	return videoState == vsDownloaded;
}

bool VideoItem::isConverted()
{
	return videoState == vsConverted;
}

bool VideoItem::isConverting()
{
	return videoState == vsConverting;
}

bool VideoItem::isCompleted()
{
	return videoState == vsCompleted;
}

bool VideoItem::isBlocked()
{
	return videoState == vsBlocked;
}

bool VideoItem::hasErrors()
{
	return videoState == vsError;
}

bool VideoItem::hasAnImportantError()
{
    return  (errorCode < 0) || (errorCode == 22) || (errorCode >= 28 && errorCode <= 33) || (errorCode == 102);
}

bool VideoItem::isBussy()
{
	return isGettingURL() || isDownloading() || isConverting();
}

bool VideoItem::isRemovable()
{
	return !isBussy();
}

bool VideoItem::isRenameable()
{
	return !isBussy();
}

bool VideoItem::isCanceled()
{
	return videoState == vsCanceled;
}

bool VideoItem::isDeleted()
{
	return videoState == vsDeleted;
}

bool VideoItem::isPauseable()
{
	return	isDownloading() || isResuming() || isReady();
}

bool VideoItem::isPaused()
{
	return videoState == vsPaused;
}

bool VideoItem::isResetable()
{
	return isCanceled() || isBlocked() || hasErrors();
}

bool VideoItem::isAnyKindOfPaused()
{
	return isReadyAndPaused() || isPaused();
}

bool VideoItem::isResuming()
{
	return videoState == vsResuming;
}

bool VideoItem::isReported()
{
	return reported;
}

bool VideoItem::isAudioFile()
{
	return videoInfo.isAudioFile;
}

bool VideoItem::isPreStateNothing()
{
	return videoPreState == vpsNothing;
}

bool VideoItem::isPreDownloading()
{
	return videoPreState == vpsPreDownloading;
}

bool VideoItem::isPreResuming()
{
	return videoPreState == vpsPreResuming;
}

bool VideoItem::isPreResumingReadyPaused()
{
	return videoPreState == vpsPreResumingReadyPaused;
}

bool VideoItem::isCustomDownload()
{
	return customItemDownload;
}

bool VideoItem::isUrlExpired()
{
	return lastUpdateDateTime.secsTo(QDateTime::currentDateTime()) >= TIMEOUT_MINS*60 && !customItemDownload;
}

bool VideoItem::isUpdatingUrl()
{
	return updateSate == vusUpdatingURL;
}

bool VideoItem::isPlaylist()
{
    return videoState == vsIsPlaylist;
}

bool VideoItem::needUpdateUrl()
{
	return updateSate == vusNeedUpdateURL;
}

bool VideoItem::needLogin()
{
	return videoState == vsNeedLogin;
}

int VideoItem::getID()
{
	return ID;
}

bool VideoItem::hasOverridedConversion()
{
	return overrideConversionConfig;
}

OverridedVideoConversionConfig VideoItem::getOverridedVideoConversionConfig()
{
	return overridedConvConf;
}

QString VideoItem::getDisplayLabel()
{
	if (videoInfo.title.isEmpty())
		return URL;
	else
		return videoInfo.title;
}

QString VideoItem::getDisplaySize()
{
	if (videoSize == 0)
		return "-";
	else
		return fileSizeToString(videoSize);
}

QString VideoItem::getDisplayProgress()
{
	if (isDownloading() || isConverting())
		return floatToStr(progress) + "%";
	else
		return getVideoStateAsString();
}

QString VideoItem::getDisplayTimeRemaining()
{
	if (isDownloading() || isConverting())
		return secondsToHMS(timeRemaining, false);
	else
		return "-";
}

QString VideoItem::getDisplayDownloadSpeed()
{
	if (isDownloading())
		return fileSizeToString(downloadSpeed) + tr("/sec");
	else
		return "-";
}

VideoDefinition VideoItem::getVideoInformation()
{
	return videoInfo;
}

QString VideoItem::getURL()
{
	return URL;
}

QString VideoItem::getVideoFile()
{
	return videoFile;
}

QString VideoItem::getVideoFileSavedTo()
{
	return videoFileSavedTo;
}

float VideoItem::getProgress()
{
	return progress;
}

int VideoItem::getVideoSize()
{
	return videoSize;
}

int VideoItem::getDownloadSpeed()
{
	return downloadSpeed;
}

int VideoItem::getTimeRemaining()
{
	return timeRemaining;
}

int VideoItem::getErrorCode()
{
	return errorCode;
}

QString VideoItem::getErrorMessage()
{
	switch (errorCode)
	{
        //-1 CUSTOM_ERROR
        case -1: return errorMessage;
		//0 NO_HTTP_ERROR
		case 0: return "";
		//20 UNABLE_CREATE_DIR
		case 20: return tr("Unable to create the destination dir");
		//21 UNABLE_CREATE_FILE
		case 21: return tr("Unable to create the destination file");
		//22 INVALID_URL
		case 22: return tr("Invalid URL");
		//23 ALREADY_DOWNLOADING
		case 23: return tr("An another download is already active");
		//24 INVALID_FILE_SIZE
		case 24: return tr("Invalid file size");
		//25 MISSING_RESUME_FILE
		case 25: return tr("The video to resume is missing");
		//26 UNABLE_RESUME_DOWNLOAD
		case 26: return tr("Unable resume the download");
		//27 UNABLE_APPEND_FILE
		case 27: return tr("Unable append data to file");
		//28 TOO_MUCH_REDIRECTIONS
		case 28: return tr("The maximum allowed http redirections has been reached");
		//29 INTERNAL_NETWEORK_TIME_OUT
		case 29: return tr("The connection to the remote server timed out");
		//30 CONNECTION_REFUSED
		case 30: return tr("The remote server refused the connection (the server is not accepting requests)");
		//31 HOST_NOT_FOUND
		case 31: return tr("The remote host name was not found (invalid hostname)");
		// 32 CONTENT_ACCESS_DENIED
		case 32: return tr("The access to the remote content was denied");
		// 33 CONTENT_NOT_FOUND
		case 33: return tr("The remote content was not found at the server ");
		// FLVSTREAMER_MISSING
		case 100: return tr("flvstreamer is missing (impossible download from RTMP server)");
		// 101 FAILED_TO_OPEN_FILE
		case 101: return tr("Unable to open the destination file");
		// 102 FAILED_TO_CONNECT
		case 102: return tr("Failed to connect with RTMP server");
		// 103 COULDNT_RESUME_FLV
		case 103: return tr("Unable to resume the FLV file");
		// 104 DOWNLOAD_INCOMPLETE
		case 104: return tr("Download incomplete (due to download errors)");
		// 105 PTHREAD_CREATE_FAILED
		case 105: return tr("Failed on initialize the pthread_create");
		// 106 FILE_NOT_FOUND
		case 106: return tr("File to resume not found");
        // 300 YOUTUBE_DL_APP_MISSING
        case 300: return tr("The Youtube-DL application is missing");
        // 301 INVALID_JSON_RESPONSE
        case 301: return tr("Impossible get video information or information is not well formed");
		// other errors, are "download errors"
		default: return tr("Connection error: Unable to download the video");
	}
}

QDateTime VideoItem::getLastUpdateDateTime()
{
	return lastUpdateDateTime;
}

void VideoItem::setVideoInformation(VideoDefinition videoInformation, QObject *who)
{
	if (isLocked() && who != locker) return;
	// set value
	this->videoInfo = videoInformation;
	// convert the html title to human title
	this->videoInfo.title = htmlToStr(this->videoInfo.title);
	// set the extension
	this->videoInfo.extension = videoInfo.extension;
	// set if this video need login
	this->videoInfo.needLogin = videoInformation.needLogin;
	// set if is an audio file
	this->videoInfo.isAudioFile = videoInformation.isAudioFile;
	// set the stored cookies for this video
	this->videoInfo.cookies = videoInformation.cookies;
	// update updating date
	setLastUpdateDateTime(QDateTime::currentDateTime());
	// no need to update information
	removeUpdatingURLStatus();
}

void VideoItem::setVideoFile(QString videoFile, QObject *who)
{
	if (isLocked() && who != locker) return;
	// set value
	this->videoFile = htmlToStr(videoFile);
}

void VideoItem::setVideoFileSavedTo(QString videoFileSavedTo, QObject *who)
{
	if (isLocked() && who != locker) return;
	// set value
	this->videoFileSavedTo = videoFileSavedTo;
}

void VideoItem::setProgress(float progress, QObject *who)
{
	if (isLocked() && who != locker) return;
	// check value
	if (progress < 0)
		progress = 0;
	else if (progress > 100)
		progress = 100;
	// set value
	this->progress = progress;
}

void VideoItem::setVideoSize(int videoSize, QObject *who)
{
	if (isLocked() && who != locker) return;
	// check value
	if (videoSize < 0)
		videoSize = 0;
	// set value
	this->videoSize = videoSize;
}

void VideoItem::setDownloadSpeed(int downloadSpeed, QObject *who)
{
	if (isLocked() && who != locker) return;
	// check value
	if (downloadSpeed < 0)
		downloadSpeed = 0;
	// set value
	this->downloadSpeed = downloadSpeed;
}

void VideoItem::setTimeRemaining(int timeRemaining, QObject *who)
{
	if (isLocked() && who != locker) return;
	// check value
	if (timeRemaining < 0)
		timeRemaining = 0;
	// set value
	this->timeRemaining = timeRemaining;
}

void VideoItem::setErrorCode(int errorCode, QObject *who)
{
	if (isLocked() && who != locker) return;
	this->errorCode = errorCode;
}

void VideoItem::setErrorMessage(QString message, QObject *who)
{
    if (isLocked() && who != locker) return;
    this->errorCode = -1;
    this->errorMessage = message;
}

void VideoItem::setState(VideoState videoState, QObject *who)
{
	if (isLocked() && who != locker) return;
	this->videoState = videoState;
}

void VideoItem::setAsNULL(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsNULL;
}

void VideoItem::setAsReady(QObject *who)
{
	setAsGettedURL(who);
}

void VideoItem::setAsReadyAndPaused(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsReadyAndPaused;
}

void VideoItem::setAsGettingURL(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsGettingURL;
}

void VideoItem::setAsGettedURL(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsGettedURL;
}

void VideoItem::setAsDownloading(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsDownloading;
}

void VideoItem::setAsDownloaded(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsDownloaded;
}

void VideoItem::setAsConverting(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsConverting;
}

void VideoItem::setAsConverted(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsConverted;
}

void VideoItem::setAsCompleted(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsCompleted;
}

void VideoItem::setAsCanceled(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsCanceled;
}

void VideoItem::setAsBlocked(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsBlocked;
}

void VideoItem::setAsError(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsError;
}

void VideoItem::setAsDeleted(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsDeleted;
}

void VideoItem::setAsPaused(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsPaused;
	videoPreState = vpsNothing;
}

void VideoItem::setAsResuming(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsResuming;
}

void VideoItem::setAsReported(QObject *who)
{
	if (isLocked() && who != locker) return;
	reported = true;
}

void VideoItem::setAsAudioFile(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoInfo.isAudioFile = true;
}

void VideoItem::setAsNeedLogin(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsNeedLogin;
}

void VideoItem::setAsPlaylist(QObject *who)
{
	if (isLocked() && who != locker) return;
	videoState = vsIsPlaylist;
}

void VideoItem::setAsNothingPreState()
{
	videoPreState = vpsNothing;
}

void VideoItem::setCustomPreState(VideoPreState preState)
{
	videoPreState = preState;
}

void VideoItem::setAsCustomDownload()
{
	customItemDownload = true;
}

void VideoItem::setAsNeedUpdateURL(VideoPreState preState)
{
	videoPreState = preState;
	updateSate = vusNeedUpdateURL;
}

void VideoItem::setAsUpdatingURL()
{
	updateSate = vusUpdatingURL;
}

void VideoItem::removeUpdatingURLStatus()
{
	updateSate = vusNothing;
}

void VideoItem::setLastUpdateDateTime(QDateTime dateTime)
{
	lastUpdateDateTime = dateTime;
}

void VideoItem::initVideoDefinition(VideoDefinition &videoDef)
{
	videoDef.URL = "";
	videoDef.title = "";
	videoDef.extension = ".flv";
	videoDef.needLogin = false;
	videoDef.isAudioFile = false;
}

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

#include "videodownload.h"

#include "http.h"
#include "rtmp.h"
#include "youtubedl.h"
#include "tools.h"
#include "videoitem.h"
#include "options.h"

/* DownloadItem Class */

VideoItem* DownloadItem::getVideoItem()
{
	return videoItem;
}

void DownloadItem::downloadStarted()
{
	workStarted();
}

void DownloadItem::downloadPaused(const QFileInfo destFile)
{
	videoItem->setVideoFile(destFile.absoluteFilePath(), this);
	videoItem->setAsPaused(this);
	workFinished();
}

void DownloadItem::downloadResumed()
{
	workStarted();
}

void DownloadItem::downloadFinished(const QFileInfo destFile)
{
	videoItem->setVideoFile(destFile.absoluteFilePath(), this);
	videoItem->setAsDownloaded(this);
	workFinished();
}

void DownloadItem::downloadCanceled()
{
	videoItem->setAsCanceled(this);
	workFinished();
}

void DownloadItem::downloadError(int error)
{
	videoItem->setErrorCode(error, this);
	videoItem->setAsError(this);
	workFinished();
}

void DownloadItem::downloadEvent(int pos, int max)
{
	videoItem->setProgress(calculePercent(pos, max), this);
	videoItem->setVideoSize(getFileSize(), this);
	videoItem->setDownloadSpeed(getDownloadSpeed(), this);
	videoItem->setTimeRemaining(getTimeRemaining(), this);
	// emit signal
	emit videoItemUpdated_child(videoItem);
}

void DownloadItem::workStarted()
{
	emit downloadStarted_child(videoItem);
}

void DownloadItem::workFinished()
{
	videoItem->unlock(this);
	// emit signal
	emit downloadFinished_child(videoItem);
	// emit the destroy signal (bye bye cruel world!)
	emit downloadDestroyable();
}

/* DownloadItem_HTTP */

DownloadItem_HTTP::DownloadItem_HTTP(VideoDownload *parent, VideoItem *videoItem)
{
	setObjectName("DownloadItem_HTTP");
	// assign main info
	this->parent = parent;
	this->videoItem = videoItem;
	this->videoItem->setAsNothingPreState();
	// create the http object
	http = new Http();
	// configure the http object
	updateConfiguration();
	// connect signals
	connect(http, SIGNAL(downloadStarted()), this, SLOT(downloadStarted()));
	connect(http, SIGNAL(downloadPaused(const QFileInfo)), this, SLOT(downloadPaused(const QFileInfo)));
	connect(http, SIGNAL(downloadResumed()), this, SLOT(downloadResumed()));
	connect(http, SIGNAL(downloadFinished(const QFileInfo)), this, SLOT(downloadFinished(const QFileInfo)));
	connect(http, SIGNAL(downloadCanceled()), this, SLOT(downloadCanceled()));
	connect(http, SIGNAL(downloadError(int)), this, SLOT(downloadError(int)));
	connect(http, SIGNAL(downloadEvent(int, int)), this, SLOT(downloadEvent(int, int)));
}

DownloadItem_HTTP::~DownloadItem_HTTP()
{
	cancelDownload();

	delete http;
}

void DownloadItem_HTTP::startDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	videoItem->setProgress(0, this);
	// if this video has special cookies, then assing them
	if ( ! videoItem->getVideoInformation().cookies.isEmpty())
		http->addCookies(videoItem->getVideoInformation().cookies);
	// if this video has special headers, then add them
	if ( ! videoItem->getVideoInformation().headers.isEmpty())
		http->addHeaderParameters(videoItem->getVideoInformation().headers);
	// if this video has a special "user agent", then override it
	if ( ! videoItem->getVideoInformation().userAgent.isEmpty())
		http->setUserAgent(videoItem->getVideoInformation().userAgent);
	// start download
	int er = http->download(QUrl(videoItem->getVideoInformation().URL),	parent->getDownloadDir(), videoItem->getVideoFile());
	if (er != EnumHTTP::NO_HTTP_ERROR) downloadError(er);
}

void DownloadItem_HTTP::pauseDownload()
{
	http->pause();
}

void DownloadItem_HTTP::resumeDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	// resume download
	int er = http->resume(QUrl(videoItem->getVideoInformation().URL), videoItem->getVideoFile());
	if (er != EnumHTTP::NO_HTTP_ERROR) downloadError(er);
}

void DownloadItem_HTTP::cancelDownload()
{
	http->cancel();
}

void DownloadItem_HTTP::updateConfiguration()
{
	http->setDeleteFileOnError(ProgramOptions::instance()->getDeleteFailedDownloads());
	http->setTimeOutOption(ProgramOptions::instance()->getEnableTimeOut());
	http->setTimeOut(ProgramOptions::instance()->getTimeOut());
	http->setUsePercentageForTimeRemaining(ProgramOptions::instance()->getTimeRemainingCalculation() == 0);
}

int DownloadItem_HTTP::getDownloadSpeed()
{
	return http->getDownloadSpeed();
}

int DownloadItem_HTTP::getTimeRemaining()
{
	return http->getTimeRemaining();
}

int DownloadItem_HTTP::getFileSize()
{
	return http->getFileSize();
}

/* DownloadItem_RTMP */

DownloadItem_RTMP::DownloadItem_RTMP(VideoDownload *parent, VideoItem *videoItem)
{
	setObjectName("DownloadItem_RTMP");
	// assign main info
	this->parent = parent;
	this->videoItem = videoItem;
	this->videoItem->setAsNothingPreState();
	// create the rtmp object
	rtmp = new RTMP(ProgramOptions::instance()->getToolsPath(), ProgramOptions::instance()->getDownloadDir());
	// connect signals
	connect(rtmp, SIGNAL(downloadStarted()), this, SLOT(downloadStarted()));
	connect(rtmp, SIGNAL(downloadPaused(const QFileInfo)), this, SLOT(downloadPaused(const QFileInfo)));
	connect(rtmp, SIGNAL(downloadResumed()), this, SLOT(downloadResumed()));
	connect(rtmp, SIGNAL(downloadFinished(const QFileInfo)), this, SLOT(downloadFinished(const QFileInfo)));
	connect(rtmp, SIGNAL(downloadCanceled()), this, SLOT(downloadCanceled()));
	connect(rtmp, SIGNAL(downloadError(int)), this, SLOT(downloadError(int)));
	connect(rtmp, SIGNAL(downloadEvent(int, int)), this, SLOT(downloadEvent(int, int)));
}

DownloadItem_RTMP::~DownloadItem_RTMP()
{
	cancelDownload();

	delete rtmp;
}

void DownloadItem_RTMP::startDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	videoItem->setProgress(0, this);
	// start download
	int er = rtmp->download(videoItem->getVideoInformation().URL, parent->getDownloadDir(),
							videoItem->getVideoFile(), videoItem->getVideoInformation().rtmpParams);
	if (er != EnumRTMP::NO_RTMP_ERROR) downloadError(er);
}

void DownloadItem_RTMP::pauseDownload()
{
	rtmp->pause();
}

void DownloadItem_RTMP::resumeDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	// resume download
	int er = rtmp->resume(videoItem->getVideoInformation().URL, videoItem->getVideoFile());
	if (er != EnumRTMP::NO_RTMP_ERROR) downloadError(er);
}

void DownloadItem_RTMP::cancelDownload()
{
	rtmp->cancel();
}

int DownloadItem_RTMP::getDownloadSpeed()
{
	return rtmp->getDownloadSpeed();
}

int DownloadItem_RTMP::getTimeRemaining()
{
	return rtmp->getTimeRemaining();
}

int DownloadItem_RTMP::getFileSize()
{
	return rtmp->getFileSize();
}

/* DownloadItem_YoutubeDL */

DownloadItem_YoutubeDL::DownloadItem_YoutubeDL(VideoDownload *parent, VideoItem *videoItem)
{
	setObjectName("DownloadItem_YoutubeDL");
	// assign main info
	this->parent = parent;
	this->videoItem = videoItem;
	this->videoItem->setAsNothingPreState();
	// create the rtmp object
	youtubeDL = new YoutubeDL(ProgramOptions::instance()->getToolsPath(), ProgramOptions::instance()->getDownloadDir());
	// connect signals
	connect(youtubeDL, SIGNAL(downloadStarted()), this, SLOT(downloadStarted()));
	connect(youtubeDL, SIGNAL(downloadPaused(const QFileInfo)), this, SLOT(downloadPaused(const QFileInfo)));
	connect(youtubeDL, SIGNAL(downloadResumed()), this, SLOT(downloadResumed()));
	connect(youtubeDL, SIGNAL(downloadFinished(const QFileInfo)), this, SLOT(downloadFinished(const QFileInfo)));
	connect(youtubeDL, SIGNAL(downloadCanceled()), this, SLOT(downloadCanceled()));
	connect(youtubeDL, SIGNAL(downloadError(int)), this, SLOT(downloadError(int)));
	connect(youtubeDL, SIGNAL(downloadEvent(int, int)), this, SLOT(downloadEvent(int, int)));
}

DownloadItem_YoutubeDL::~DownloadItem_YoutubeDL()
{
	cancelDownload();

	delete youtubeDL;
}

void DownloadItem_YoutubeDL::startDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	videoItem->setProgress(0, this);
	// start download
	int er = youtubeDL->download(videoItem->getVideoInformation().URL, parent->getDownloadDir(),
								 videoItem->getVideoFile(), videoItem->getVideoInformation().rtmpParams);
	if (er != EnumYoutubeDL::NO_YOUTUBE_DL_ERROR) downloadError(er);
}

void DownloadItem_YoutubeDL::pauseDownload()
{
	youtubeDL->pause();
}

void DownloadItem_YoutubeDL::resumeDownload()
{
	// assign data
	videoItem->lock(this);
	videoItem->setAsDownloading(this);
	// resume download
	int er = youtubeDL->resume(videoItem->getVideoInformation().URL, videoItem->getVideoFile());
	if (er != EnumYoutubeDL::NO_YOUTUBE_DL_ERROR) downloadError(er);
}

void DownloadItem_YoutubeDL::cancelDownload()
{
	youtubeDL->cancel();
}

int DownloadItem_YoutubeDL::getDownloadSpeed()
{
	return youtubeDL->getDownloadSpeed();
}

int DownloadItem_YoutubeDL::getTimeRemaining()
{
	return youtubeDL->getTimeRemaining();
}

int DownloadItem_YoutubeDL::getFileSize()
{
	return youtubeDL->getFileSize();
}

/* VideoDownload Class */

VideoDownload::VideoDownload(QString saveTo, int maxActiveDownloads)
{
	setObjectName("VideoDownload");
	// init vars
	setDownloadDir(saveTo);
	setMaxActiveDownloads(maxActiveDownloads);
	// init downloads list
	downloads = new QList<DownloadItem *>;
	// init garbage collector cue
	garbageCollecotrQueue = new QList<DownloadItem *>;
}

VideoDownload::~VideoDownload()
{
	cancelAllDownloads();

	// force to execute the garbage collector
	destroyDownload();

	delete garbageCollecotrQueue;
	delete downloads;
}

DownloadItem* VideoDownload::findDownloadItemByVideoItem(VideoItem *videoItem)
{
	if (videoItem == nullptr) return nullptr;

	for (int n = 0; n < downloads->size(); n++)
		if (downloads->at(n)->getVideoItem() == videoItem)
			return downloads->at(n);

	return nullptr;
}

void VideoDownload::stopAllDownloads(bool doCancel)
{
	for (int n = downloads->size() - 1; n >= 0; n--)
	{
		if (doCancel) downloads->at(n)->cancelDownload();
		else downloads->at(n)->pauseDownload();
		// process events
		qApp->processEvents();
	}
}

DownloadItem* VideoDownload::createDownloadItem(VideoItem *videoItem)
{
	if (videoItem == nullptr) return nullptr;

	// if cannot start then set this item as "preDownloading"
	if ( ! canStartDownload())
	{
		videoItem->setCustomPreState(vpsPreDownloading);
		emit videoItemUpdated(videoItem);
		// ops
		return nullptr;
	}
	// the downloader is "youtube-dl"?
	if (videoItem->getVideoInformation().downloader == "youtube-dl")
	{
		downloads->append(new DownloadItem_YoutubeDL(this, videoItem));
	}
	// check if is an HTTP or RTMP download and add it inot downloads queue
	else if (isHttpURL(videoItem->getVideoInformation().URL))
	{
		downloads->append(new DownloadItem_HTTP(this, videoItem));
	}
	// is a RTMP url?
	else if (isRtmpURL(videoItem->getVideoInformation().URL))
	{
		downloads->append(new DownloadItem_RTMP(this, videoItem));
	}
	else // invalid URL
	{
		videoItem->setErrorCode(EnumHTTP::INVALID_URL);
		videoItem->setAsError();
		emit videoItemUpdated(videoItem);
		// ops...
		return nullptr;
	}
	// get the new item added
	DownloadItem *downloadItem = downloads->last();
	// connect signals of this new child
	connect(downloadItem, SIGNAL(videoItemUpdated_child(VideoItem*)), this, SLOT(videoItemUpdated_child(VideoItem*)));
	connect(downloadItem, SIGNAL(downloadStarted_child(VideoItem*)), this, SLOT(downloadStarted_child(VideoItem*)));
	connect(downloadItem, SIGNAL(downloadFinished_child(VideoItem*)), this, SLOT(downloadFinished_child(VideoItem*)));
	connect(downloadItem, SIGNAL(downloadDestroyable()), this, SLOT(downloadDestroyable()));
	// return this new one
	return downloadItem;
}

void VideoDownload::downloadVideo(VideoItem *videoItem)
{
	DownloadItem *downloadItem = createDownloadItem(videoItem);
	// start to download the video
	if (downloadItem)
	{
		downloadItem->startDownload();
	}
}

void VideoDownload::pauseDownload(VideoItem *videoItem)
{
	DownloadItem *downloadItem = findDownloadItemByVideoItem(videoItem);

	if (downloadItem != nullptr)
	{
		downloadItem->pauseDownload();
	}
}

void VideoDownload::resumeDownload(VideoItem *videoItem)
{
	if (videoItem)
	{
		// if is Ready and paused then it should start instead of resume
		if (videoItem->isReadyAndPaused())
		{
			downloadVideo(videoItem);
			return;
		}
		// create a new download
		DownloadItem *downloadItem = createDownloadItem(videoItem);
		// start to download the video
		if (downloadItem)
		{
			downloadItem->resumeDownload();
		}
	}
}

void VideoDownload::cancelDownload(VideoItem *videoItem)
{
	DownloadItem *downloadItem = findDownloadItemByVideoItem(videoItem);

	if (downloadItem != nullptr)
		downloadItem->cancelDownload();
}

void VideoDownload::pauseAllDownloads()
{
	stopAllDownloads();
}

void VideoDownload::cancelAllDownloads()
{
	stopAllDownloads(true);
}

bool VideoDownload::canStartDownload()
{
	return downloads->size() < maxActiveDownloads;
}

bool VideoDownload::isDownloading()
{
	return !downloads->isEmpty();
}

QString VideoDownload::getDownloadDir()
{
	return downloadDir;
}

void VideoDownload::setDownloadDir(QString downloadDir)
{
	this->downloadDir = downloadDir;
}

int VideoDownload::getMaxActiveDownloads()
{
	return maxActiveDownloads;
}

void VideoDownload::setMaxActiveDownloads(int maxActiveDownloads)
{
	maxActiveDownloads = maxActiveDownloads < 1 ? 1 : maxActiveDownloads;

	this->maxActiveDownloads = maxActiveDownloads;
}

void VideoDownload::updateHttpConfiguration()
{
	for (int n = 0; n < downloads->size(); n++)
		if (downloads->at(n)->objectName() == "DownloadItem_HTTP")
			static_cast<DownloadItem_HTTP *>(downloads->at(n))->updateConfiguration();
}

void VideoDownload::videoItemUpdated_child(VideoItem *videoItem)
{
	emit videoItemUpdated(videoItem);
}

void VideoDownload::downloadStarted_child(VideoItem *videoItem)
{
	emit downloadStarted(videoItem);
}

void VideoDownload::downloadFinished_child(VideoItem *videoItem)
{
	emit downloadFinished(videoItem);
}

void VideoDownload::downloadDestroyable()
{
	// add this object to garbage collector cue (to be destroyed and removed from list safely)
	garbageCollecotrQueue->append(static_cast<DownloadItem *>(sender()));
	// execute garbage collector
	QTimer::singleShot(0, this, SLOT(destroyDownload()));
}

void VideoDownload::destroyDownload()
{
	// remove all items which are in garbage collector cue
	while (!garbageCollecotrQueue->isEmpty())
	{
		DownloadItem *toDestroy = garbageCollecotrQueue->first();
		// delete from garbage collector cue
		garbageCollecotrQueue->removeFirst();
		// disconnect signals
		toDestroy->disconnect();
		// delete DownloadItem from list of active downloads
		downloads->removeAt(downloads->indexOf(toDestroy));
		// destroy the DownloadItem
		delete toDestroy;
	}
}

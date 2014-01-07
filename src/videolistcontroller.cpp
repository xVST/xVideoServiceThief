/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

#include "videolistcontroller.h"

#include "tools.h"
#include "options.h"
#include "videoitem.h"
#include "videoinformation.h"
#include "videoconvert.h"
#include "videodownload.h"
#include "schedule.h"

// VideoListController class

static VideoListController *lastInstance = NULL;

VideoListController::VideoListController(ProgramOptions *programOptions)
{
	setObjectName("VideoListController");

	this->programOptions = programOptions;

	internalTimer = 0;

	videoList = new QList<VideoItem *>;

	schedule = new ScheduleController(programOptions->getOptionsPath(), programOptions->getOptionsFormat());
	schedule->load();

	videoInformation = new VideoInformation(programOptions->getPluginsPath());
	videoDownload = new VideoDownload(programOptions->getDownloadDir(), programOptions->getMaxActiveDownloads());
	videoConverter = new VideoConverter(programOptions->getFfmpegLibLocation(),
	                                    programOptions->getDownloadDir(),
	                                    programOptions->getConversionConf(),
	                                    programOptions->getDeleteVideosOnConvert());

	// video information
	connect(videoInformation, SIGNAL(informationStarted(VideoItem *)), this, SLOT(actionStarted(VideoItem *)));
	connect(videoInformation, SIGNAL(informationFinished(VideoItem *)), this, SLOT(actionFinished(VideoItem *)));
	// video download
	connect(videoDownload, SIGNAL(videoItemUpdated(VideoItem *)), this, SLOT(videoItemUpdated(VideoItem *)));
	connect(videoDownload, SIGNAL(downloadStarted(VideoItem *)), this, SLOT(actionStarted(VideoItem *)));
	connect(videoDownload, SIGNAL(downloadFinished(VideoItem *)), this, SLOT(actionFinished(VideoItem *)));
	// video converter
	connect(videoConverter, SIGNAL(videoItemUpdated(VideoItem *)), this, SLOT(videoItemUpdated(VideoItem *)));
	connect(videoConverter, SIGNAL(conversionStarted(VideoItem *)), this, SLOT(actionStarted(VideoItem *)));
	connect(videoConverter, SIGNAL(conversionFinished(VideoItem *)), this, SLOT(actionFinished(VideoItem *)));
	//
	lastInstance = this;
}

VideoListController::~VideoListController()
{
	stop();

	videoInformation->abortExecution();

	clear();

	delete videoList;

	delete videoConverter;
	delete videoInformation;
	delete videoDownload;
	delete schedule;
}

bool VideoListController::validItemIndex(const int index)
{
	return (index >= 0 && index < getVideoItemCount());
}

VideoItem* VideoListController::getFirstByState(VideoState videoState)
{
	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->getVideoState() == videoState)
			return videoItem;
	// no items are ready to be downloaded
	return NULL;
}

void VideoListController::swapVideoItems(VideoItem *from, VideoItem *to)
{
	if (from != NULL && to != NULL)
	{
		int i = getVideoItemIndexByVideoItem(from);
		int j = getVideoItemIndexByVideoItem(to);

		if (validItemIndex(i) && validItemIndex(j))
		{
			videoList->swap(i, j);
			emit videoMoved(i, j);
		}
	}
}

void VideoListController::timerEvent(QTimerEvent* /*event*/)
{
	// get first null item to, to start get info
	if (videoInformation->canGetInformation())
	{
		// get first item which need to update the url
		VideoItem *item = getFirstWhichNeedUpdateUrl();
		// if no items where found, then get the first null item
		if (item == NULL) item = getFirstNULL();
		// get video information
		startGetInformation(item);
	}

	// get the first error item, to display the bug report (if is necessary)
	displayError(getFirstError());

	// get first item which has a pre-state assigned
	VideoItem *videoItem = getFirstWithPreState();

	// if we found an item with a pre-state, then it has a high priority (no one can start before it)
	if (videoItem != NULL && !videoItem->hasErrors())
	{
		// the item don't need to update the url
		if (!videoItem->needUpdateUrl())
		{
			if (videoItem->isPreDownloading() || videoItem->isPreResumingReadyPaused()) startDownload(videoItem);
			else if (videoItem->isPreResuming()) resumeDownload(videoItem);
		}
	}
	else // no special items were found
	{
		// get the first item with "resuming" state
		videoItem = getFirstByState(vsResuming);
		// if we found an item resuming... we start it (if shedule and download list let us)
		if (videoItem != NULL && schedule->canStart() && videoDownload->canStartDownload())
			resumeDownload(videoItem);
		// get the first ready item, to auto-start the download
		else if (programOptions->getDownloadAutomatically() && schedule->canStart() && videoDownload->canStartDownload())
		{
			videoItem = getFirstReady();
			if (videoItem != NULL && !videoItem->hasErrors())
			{
				if (videoItem->isResuming()) resumeDownload(videoItem);
				else startDownload(videoItem);
			}
		}
	}

	// get the first downloaded item, to start the conversion
	if (videoConverter->canStartConversion())
	{
		if (programOptions->getConvertVideos())
			startConversion(getFirstDownloaded());
		else // video conversion is disabled, but maybe a custom download is ready to convert
			startConversion(getFirstDownloadedWithOverridedConversion());
	}
}

void VideoListController::start()
{
	stop();

	internalTimer = this->startTimer(100);
}

void VideoListController::stop()
{
	if (internalTimer != 0)
		this->killTimer(internalTimer);

	internalTimer = 0;
}

VideoItem* VideoListController::addVideo(const QString URL)
{
	videoList->append(new VideoItem(URL));
	emit videoAdded(videoList->last());
	return videoList->last();
}

VideoItem* VideoListController::addVideo(const QString URL, const OverridedVideoConversionConfig overridedConversionConfig)
{
	videoList->append(new VideoItem(URL, overridedConversionConfig));
	emit videoAdded(videoList->last());
	return videoList->last();
}

VideoItem* VideoListController::addVideo(VideoItem *videoItem)
{
	videoList->append(new VideoItem);
	videoList->last()->assign(videoItem);
	emit videoAdded(videoList->last());
	return videoList->last();
}

VideoItem* VideoListController::addVideo(const QString URL, const QString title)
{
	videoList->append(new VideoItem(URL));
	VideoDefinition videoDef;
	videoDef.title = title;
	videoDef.URL = URL;
	videoDef.extension = extractFileExt(URL);
	videoList->last()->setVideoInformation(videoDef);
	videoList->last()->setVideoFile(cleanFileName(title + videoDef.extension), NULL);
	videoList->last()->setAsGettedURL();
	videoList->last()->setAsCustomDownload();
	emit videoAdded(videoList->last());
	return videoList->last();
}

VideoItem* VideoListController::addVideo(const QString URL, const QString title,
										 const OverridedVideoConversionConfig overridedConversionConfig)
{
	videoList->append(new VideoItem(URL, overridedConversionConfig));
	VideoDefinition videoDef;
	videoDef.title = title;
	videoDef.URL = URL;
	videoDef.extension = extractFileExt(URL);
	videoList->last()->setVideoInformation(videoDef);
	videoList->last()->setVideoFile(cleanFileName(title + videoDef.extension), NULL);
	videoList->last()->setAsGettedURL();
	videoList->last()->setAsCustomDownload();
	emit videoAdded(videoList->last());
	return videoList->last();
}

void VideoListController::deleteVideo(const int index, bool removePausedFile)
{
	if (validItemIndex(index))
	{
		VideoItem *videoItem = getVideoItem(index);

		if (!videoItem->isRemovable()) return;

		// remove the "incompleted" video
		if (videoItem->isPaused() && removePausedFile)
			if (QFile::exists(videoItem->getVideoFile()))
				QFile::remove(videoItem->getVideoFile());

		videoItem->setAsDeleted();
		emit videoDeleted(videoItem);
		delete videoList->at(index);
		videoList->removeAt(index);
	}
}

void VideoListController::deleteVideo(VideoItem *videoItem, bool removePausedFile)
{
	deleteVideo(videoList->indexOf(videoItem), removePausedFile);
}

void VideoListController::renameVideo(const int index, QString newName)
{
	if (validItemIndex(index))
	{
		VideoItem *videoItem = getVideoItem(index);

		if (!videoItem->isRenameable() || videoItem->getDisplayLabel() == newName) return;

		// prepare the new title
		newName = htmlToStr(newName);

		// update the video title
		VideoDefinition inf = videoItem->getVideoInformation();
		inf.title = newName;
		videoItem->setVideoInformation(inf);

		// generate the new file name if it is assigned
		if (videoItem->getVideoFile() != "")
		{
			QString newVideoFile = uniqueFileName(changeFileName(videoItem->getVideoFile(), cleanFileName(newName)));
			// if the file is paused or finished, then
			if (QFile::exists(videoItem->getVideoFile()))
				QFile::rename(videoItem->getVideoFile(), newVideoFile);
			// update the videoFile
			videoItem->setVideoFile(newVideoFile);
		}

		// generate the new file saved name
		if (videoItem->getVideoFileSavedTo() != "")
		{
			QString newVideoFileSavedTo = uniqueFileName(changeFileName(videoItem->getVideoFileSavedTo(), cleanFileName(newName)));
			// if the file is paused or finished, then
			if (QFile::exists(videoItem->getVideoFileSavedTo()))
				QFile::rename(videoItem->getVideoFileSavedTo(), newVideoFileSavedTo);
			// update the videoFile
			videoItem->setVideoFileSavedTo(newVideoFileSavedTo);
		}

		// advertise about this change
		emit videoRenamed(videoItem);
	}
}

void VideoListController::renameVideo(VideoItem *videoItem, QString newName)
{
	renameVideo(videoList->indexOf(videoItem), newName);
}

void VideoListController::clear(bool removePausedFile)
{
	if (videoInformation->isGettingInfo())
		videoInformation->cancel();

	while (!videoList->isEmpty())
	{
		// process any pending event... (such as downloadPaused, or others...)
		qApp->processEvents();
		// try to delete this video
		deleteVideo(videoList->at(0), removePausedFile);
	}
}

VideoItem* VideoListController::getFirstNULL()
{
	return getFirstByState(vsNULL);
}

VideoItem* VideoListController::getFirstReady()
{
	VideoItem *result = getFirstByState(vsResuming);
	// return the first item ready to "be downloaded" (first try to get the resuming items)
	if (result == NULL)
		return getFirstByState(vsGettedURL);
	else
		return result;
}

VideoItem* VideoListController::getFirstError(bool ignoreReported)
{
	if (!ignoreReported)
		return getFirstByState(vsError);
	else
	{
		foreach (VideoItem *videoItem, *videoList)
			if (videoItem->getVideoState() == vsError && !videoItem->isReported())
				return videoItem;
		// no items are ready to be downloaded
		return NULL;
	}
}

VideoItem* VideoListController::getFirstDownloaded()
{
	return getFirstByState(vsDownloaded);
}

VideoItem* VideoListController::getFirstDownloadedWithOverridedConversion()
{
	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->isCustomDownload() && videoItem->hasOverridedConversion())
			return videoItem;
	// no items are ready to be downloaded
	return NULL;
}

VideoItem* VideoListController::getFirstConverted()
{
	return getFirstByState(vsConverted);
}

VideoItem* VideoListController::getFirstWhichNeedUpdateUrl()
{
	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->needUpdateUrl())
			return videoItem;
	// no items found
	return NULL;
}

VideoItem* VideoListController::getFirstWithPreState()
{
	foreach (VideoItem *videoItem, *videoList)
		if (!videoItem->isPreStateNothing())
			return videoItem;
	// no items found
	return NULL;
}

VideoItem* VideoListController::getCurrentDownloadingVideo()
{
	return getFirstByState(vsDownloading);
}

VideoItem* VideoListController::getCurrentConvertingVideo()
{
	return getFirstByState(vsConverting);
}

bool VideoListController::canBeDeleted(const int index)
{
	return !isBussy(index);
}

bool VideoListController::canBeDeleted(VideoItem *videoItem)
{
	return canBeDeleted(videoList->indexOf(videoItem));
}

bool VideoListController::isBussy(const int index)
{
	if (validItemIndex(index))
		return getVideoItem(index)->isBussy();
	else
		return false;
}

bool VideoListController::isBussy(VideoItem *videoItem)
{
	return isBussy(videoList->indexOf(videoItem));
}

void VideoListController::deleteCompletedItems()
{
	for (int n = videoList->count() - 1; n >= 0; n--)
		if (videoList->at(n)->isCompleted())
			deleteVideo(videoList->at(n));
}

int VideoListController::getCompletedItemsCount()
{
	int count = 0;

	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->isCompleted())
			count++;

	return count;
}

VideoItem* VideoListController::getVideoItem(const int index)
{
	if (validItemIndex(index))
		return videoList->at(index);
	else
		return NULL;
}

VideoItem* VideoListController::getVideoItemByID(const int ID)
{
	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->getID() == ID)
			return videoItem;
	// no items found with this ID
	return NULL;
}

VideoItem* VideoListController::getVideoItemFromQVAriant(const QVariant variant)
{
	bool ok;
	int ID = variant.toInt(&ok);
	// ok?
	if (ok)
		return getVideoItemByID(ID);
	else
		return NULL;
}

int VideoListController::getVideoItemIndexByVideoItem(VideoItem* videoItem)
{
	return videoList->indexOf(videoItem);
}

int VideoListController::getVideoItemCount(bool ignoreDeleted)
{
	if (ignoreDeleted)
	{
		int count = 0;

		foreach (VideoItem *videoItem, *videoList)
			if (!videoItem->isDeleted())
				count++;

		return count;
	}

	else
		return videoList->count();
}

int VideoListController::getActiveDownloadsCount()
{
	int total = 0;

	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->isDownloading())
			total++;

	return total;
}

int VideoListController::getDownloableVideosCount(bool includeDownloadingVideos)
{
	int total = 0;

	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->isReady())
			total++;
		else if (includeDownloadingVideos && videoItem->isDownloading())
			total++;

	return total;
}

int VideoListController::getTotalDownloadSpeed()
{
	int totalDownloadSpeed = 0;

	foreach (VideoItem *videoItem, *videoList)
		if (videoItem->isDownloading())
			totalDownloadSpeed += videoItem->getDownloadSpeed();

	return totalDownloadSpeed;
}

bool VideoListController::isWorking()
{
	return isDownloading() || isConverting();
}

bool VideoListController::isDownloading()
{
	return videoDownload->isDownloading();
}

bool VideoListController::isConverting()
{
	return videoConverter->isConverting();
}

bool VideoListController::canStartDownload()
{
	return videoDownload->canStartDownload();
}

bool VideoListController::isAlreadyAdded(const QString URL)
{
	// find a video with this URL
	foreach (VideoItem *videoItem, *videoList)
		if (URL == videoItem->getURL())
			return true;
	// not added
	return false;
}

void VideoListController::startGetInformation(VideoItem *videoItem)
{
	videoInformation->getVideoInformation(videoItem);
}

void VideoListController::startDownload(VideoItem *videoItem)
{
	if (videoItem == NULL) return;
	// check if is possible download this video
	if (!videoInformation->isBlockedHost(videoItem->getURL()))
	{
		// check if his URL hasn't been expired
		if (!videoItem->isUrlExpired())
			videoDownload->downloadVideo(videoItem);
		else // url expired and not is updating this URL
			if (!videoItem->isUpdatingUrl() && !videoItem->needUpdateUrl())
				videoItem->setAsNeedUpdateURL(vpsPreDownloading);
	}
	else
	{
		videoItem->setAsBlocked();
		emit videoItemUpdated(videoItem);
	}
}

void VideoListController::pauseDownload(VideoItem *videoItem)
{
	if (videoItem != NULL)
	{
		if (videoItem->isResuming())
		{
			videoItem->setAsPaused();
			videoItem->setAsNothingPreState();
			emit videoUpdated(videoItem);
		}
		else // pause
		{
			// isn't downloading, but can be paused because is ready (new state will be ReadyAndPaused)
			if (videoItem->isReady())
			{
				videoItem->setAsReadyAndPaused();
				emit videoUpdated(videoItem);
			}
			else // is downloading, so pause it
				videoDownload->pauseDownload(videoItem);
		}
	}
}

void VideoListController::resumeDownload(VideoItem *videoItem)
{
	// check if his URL hasn't been expired
	if (!videoItem->isUrlExpired())
		videoDownload->resumeDownload(videoItem);
	else // url expired and not is updating this URL
		if (!videoItem->isUpdatingUrl() && !videoItem->needUpdateUrl())
			videoItem->setAsNeedUpdateURL(videoItem->isReadyAndPaused() ? vpsPreResumingReadyPaused : vpsPreResuming);
}

void VideoListController::cancelDownload(VideoItem *videoItem)
{
	videoDownload->cancelDownload(videoItem);
}

void VideoListController::pauseAllDownloads()
{
	videoDownload->pauseAllDownloads();
}

void VideoListController::cancelAllDownloads()
{
	videoDownload->cancelAllDownloads();
}

void VideoListController::startConversion(VideoItem *videoItem)
{
	videoConverter->startConversionVideo(videoItem);
}

void VideoListController::cancelConversion()
{
	videoConverter->cancelConversion();
}

void VideoListController::displayError(VideoItem *videoItem)
{
	if (videoItem != NULL)
		emit videoError(videoItem);
}

void VideoListController::updateOptions()
{
	// video information
	videoInformation->setBlockAdultContent(programOptions->getBlockAdultContent());
	videoInformation->setBlockAdultContentList(programOptions->getBlockedWebsitesList());

	// video download
	videoDownload->setDownloadDir(programOptions->getDownloadDir());
	videoDownload->setMaxActiveDownloads(programOptions->getMaxActiveDownloads());
	videoDownload->updateHttpConfiguration();

	// converter
	videoConverter->setFFmpegApp(programOptions->getFfmpegLibLocation());
	videoConverter->setWorkingDir(programOptions->getDownloadDir());
	videoConverter->setConversionConfig(programOptions->getConversionConf());
	videoConverter->setDeleteOriginalVideo(programOptions->getDeleteVideosOnConvert());

	// schedule
	schedule->setEnabled(programOptions->getScheduleEnabled());
	schedule->load();

	// configure proxy
	setupProxy();
}

void VideoListController::setupProxy()
{
	// use user proxy config
	if (!programOptions->getUseSystemProxyConfig())
	{
		// update proxy
		if (programOptions->getUseProxy())
		{
			QNetworkProxy networkProxy;
			networkProxy.setUser(programOptions->getProxyUserName());
			networkProxy.setPassword(programOptions->getProxyPassword());
			networkProxy.setHostName(programOptions->getProxyAddress());
			networkProxy.setPort(programOptions->getProxyPort());
			networkProxy.setType(static_cast<QNetworkProxy::ProxyType>(programOptions->getProxyType()));
			// set application proxy
			QNetworkProxy::setApplicationProxy(networkProxy);
		}
		else // no proxy
		{
			QNetworkProxy networkProxy;
			networkProxy.setType(QNetworkProxy::NoProxy);
			// remove application proxy
			QNetworkProxy::setApplicationProxy(networkProxy);
		}
	}
	else // use system configuration
	{
		class SystemProxyFactory : public QNetworkProxyFactory
		{
			public:
				virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query)
				{
					return QNetworkProxyFactory::systemProxyForQuery(query);
				}
		};
		// assign this system-proxy
		QNetworkProxyFactory::setApplicationProxyFactory(new SystemProxyFactory());
	}
}

VideoInformation* VideoListController::getVideoInformation()
{
	return videoInformation;
}

void VideoListController::moveUP(VideoItem *videoItem)
{
	int prevItem = videoList->indexOf(videoItem);

	if (prevItem > 0)
		swapVideoItems(videoItem, videoList->at(prevItem - 1));
}

void VideoListController::moveDOWN(VideoItem *videoItem)
{
	int nextItem = videoList->indexOf(videoItem);

	if (nextItem < getVideoItemCount() - 1)
		swapVideoItems(videoItem, videoList->at(nextItem + 1));
}

VideoListController* VideoListController::instance()
{
	return lastInstance;
}

void VideoListController::videoItemUpdated(VideoItem *videoItem)
{
	if (videoItem != NULL)
		emit videoUpdated(videoItem);
}

void VideoListController::actionStarted(VideoItem *videoItem)
{
	if (videoItem != NULL)
		emit videoUpdated(videoItem);
}

void VideoListController::actionFinished(VideoItem *videoItem)
{
	if (videoItem != NULL)
	{
		if (!videoItem->isCanceled() && !videoItem->isPaused() && !videoItem->hasErrors())
			if	(sender() == videoConverter ||
			    (sender() == videoDownload && !programOptions->getConvertVideos()) ||
			    (sender() == videoDownload && programOptions->getConvertVideos() && !videoConverter->ffmpegInstalled()) ||
				(sender() == videoDownload && videoItem->isAudioFile()))
			{
				// set the saved to
				if (sender() == videoDownload)
					videoItem->setVideoFileSavedTo(videoItem->getVideoFile());
				// ok, set as completed
				videoItem->setAsCompleted(videoItem);
			}
		emit videoUpdated(videoItem);
	}
}

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

#ifndef __VIDEOLISTCONTROLLER_H__
#define __VIDEOLISTCONTROLLER_H__

#include <QtWidgets>
//#include <QNetworkProxy>
#include <QtNetwork>

#include "videoitem.h"

class VideoItem;
class VideoInformation;
class VideoDownload;
class VideoConverter;
class ProgramOptions;
class ScheduleController;

struct OverridedVideoConversionConfig;

class VideoListController : public QObject
{
Q_OBJECT
	private:
		QList<VideoItem *> *videoList;		//!< list with all stored videos
		VideoInformation *videoInformation;	//!< video information class
		VideoDownload *videoDownload;		//!< video download class
		VideoConverter *videoConverter;		//!< video converter class
		ProgramOptions *programOptions; 	//!< pointer to the program options
		ScheduleController *schedule;		//!< schedule controller
		int internalTimer;					//!< internal timer
		/*! Determine if this index is a valid item index */
		bool validItemIndex(const int index);
		/*! Get the first item found using the videoState */
		VideoItem* getFirstByState(VideoState videoState);
		/*! Swap two items */
		void swapVideoItems(VideoItem *from, VideoItem *to);
		/*! Configure the proxy settings */
		void setupProxy();
	protected:
		/*! Internal timer event */
		void timerEvent(QTimerEvent *event);
	public:
		/*! Class constructor */
		VideoListController(ProgramOptions *programOptions);
		/*! Class destructor */
		~VideoListController();
		/*! Start the main loop */
		void start();
		/*! Stop the main loop */
		void stop();
		/*! Add a new VideoItem */
		VideoItem* addVideo(const QString URL);
		/*! Add a new VideoItem with an overrided Conversion configuration */
		VideoItem* addVideo(const QString URL, const OverridedVideoConversionConfig overridedConversionConfig);
		/*! Add a stored VideoItem */
		VideoItem* addVideo(VideoItem *videoItem);
		/*! Add a custom VideoItem */
		VideoItem* addVideo(const QString URL, const QString title);
		/*! Add a custom VideoItem with an overrided Conversion configuration */
		VideoItem* addVideo(const QString URL, const QString title, const OverridedVideoConversionConfig overridedConversionConfig);
		/*! Delete by Index an existent VideoItem */
		void deleteVideo(const int index, bool removePausedFile = false);
		/*! Delete an existent VideoItem */
		void deleteVideo(VideoItem *videoItem, bool removePausedFile = false);
		/*! Rename by Index an existent VideoItem */
		void renameVideo(const int index, QString newName);
		/*! Rename an existent VideoItem */
		void renameVideo(VideoItem *videoItem, QString newName);
		/*! Clear videoList list */
		void clear(bool removePausedFile = false);
		/*! Get the first null item to get info */
		VideoItem* getFirstNULL();
		/*! Get the first ready to be downloaded */
		VideoItem* getFirstReady();
		/*! Get the first video with an error */
		VideoItem* getFirstError(bool ignoreReported = true);
		/*! Get the first downloaded to be converted */
		VideoItem* getFirstDownloaded();
		/*! Get the first downloaded to be converted with an overrided conversion */
		VideoItem* getFirstDownloadedWithOverridedConversion();
		/*! Get the first item converted */
		VideoItem* getFirstConverted();
		/*! Get the first item which need to update the url */
		VideoItem* getFirstWhichNeedUpdateUrl();
		/*! Get the first item which has a pre-state */
		VideoItem* getFirstWithPreState();
		/*! Get the current downloading video */
		VideoItem* getCurrentDownloadingVideo();
		/*! get the current converting video */
		VideoItem* getCurrentConvertingVideo();
		/*! Get if this item can be deleted or not (by Position) */
		bool canBeDeleted(const int index);
		/*! Get if this item can be deleted or not (by Item) */
		bool canBeDeleted(VideoItem *videoItem);
		/*! Get if this item is bussy (by Position) */
		bool isBussy(const int index);
		/*! Get if this item is bussy (by Item) */
		bool isBussy(VideoItem *videoItem);
		/*! Delete completed items */
		void deleteCompletedItems();
		/*! Get the completed items count */
		int getCompletedItemsCount();
		/*! Get a VideoItem by ibdex */
		VideoItem* getVideoItem(const int index);
		/*! Get a VideoItem by ID */
		VideoItem* getVideoItemByID(const int ID);
		/*! Get a VideoItem by QVAriant (variant must have the item ID)*/
		VideoItem* getVideoItemFromQVAriant(const QVariant variant);
		/*! Get a VideoItem Index by VideoItem */
		int getVideoItemIndexByVideoItem(VideoItem* videoItem);
		/*! Get videos count */
		int getVideoItemCount(bool ignoreDeleted = false);
		/*! Get active downloads count */
		int getActiveDownloadsCount();
		/*! Get downloable items count */
		int getDownloableVideosCount(bool includeDownloadingVideos = true);
		/*! Get total download speed */
		int getTotalDownloadSpeed();
		/*! Get if some process is working */
		bool isWorking();
		/*! Get if is downloading */
		bool isDownloading();
		/*! Get if is converting */
		bool isConverting();
		/*! Get if is can start a new download */
		bool canStartDownload();
		/*! Get if this URL is already into the list */
		bool isAlreadyAdded(const QString URL);
		/*! Start to get video information */
		void startGetInformation(VideoItem *videoItem);
		/*! Start a new download */
		void startDownload(VideoItem *videoItem);
		/*! Pause download */
		void pauseDownload(VideoItem *videoItem);
		/*! Resume download */
		void resumeDownload(VideoItem *videoItem);
		/*! Cancel download */
		void cancelDownload(VideoItem *videoItem);
		/*! Pause all downloads */
		void pauseAllDownloads();
		/*! Cancel all downloads */
		void cancelAllDownloads();
		/*! Start a new conversion */
		void startConversion(VideoItem *videoItem);
		/*! Cancel conversion */
		void cancelConversion();
		/*! Inform about an error */
		void displayError(VideoItem *videoItem);
		/*! Update options */
		void updateOptions();
		/*! Get the internal video information */
		VideoInformation* getVideoInformation();
		/*! Move an item up (into prior list) */
		void moveUP(VideoItem *videoItem);
		/*! Move an item down (into prior list) */
		void moveDOWN(VideoItem *videoItem);
		/*! Get the last created instance */
		static VideoListController* instance();
	private slots:
		/*! Video item has been updated */
		void videoItemUpdated(VideoItem *videoItem);
		/*! Video srtart a new process */
		void actionStarted(VideoItem *videoItem);
		/*! Video has been finished the process */
		void actionFinished(VideoItem *videoItem);
	signals:
		/*! When a video is added, this signal is emited */
		void videoAdded(VideoItem *videoItem);
		/*! When a video is deleted, this signal is emited */
		void videoDeleted(VideoItem *videoItem);
		/*! When a videoItem is renamed, this signal is emited */
		void videoRenamed(VideoItem *videoItem);
		/*! When a videoItem is updated, this signal is emited */
		void videoUpdated(VideoItem *videoItem);
		/*! When a videoItem trhows an error */
		void videoError(VideoItem *videoItem);
		/*! When a videoItem has been moved */
		void videoMoved(int from, int to);
};

#endif // __VIDEOLISTCONTROLLER_H__

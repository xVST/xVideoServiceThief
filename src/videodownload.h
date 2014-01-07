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

#ifndef __VIDEODOWNLOAD_H__
#define __VIDEODOWNLOAD_H__

#include <QtWidgets>

class Http;
class RTMP;
class VideoItem;
class VideoDownload;

/*! Single download (Base class) */
class DownloadItem : public QObject
{
Q_OBJECT
	protected:
		VideoDownload *parent;	//!< parent class
		VideoItem *videoItem;	//!< current video item
		/*! When work started */
		void workStarted();
		/*! When work finished */
		void workFinished();
		/*! Set download speed */
		virtual int getDownloadSpeed() = 0;
		/*! Set time remaining */
		virtual int getTimeRemaining() = 0;
		/*! Get the video size */
		virtual int getFileSize() = 0;
	public:
		/*! Get the video item associated */
		VideoItem* getVideoItem();
		/*! Start the download */
		virtual void startDownload() = 0;
		/*! Pause the download */
		virtual void pauseDownload() = 0;
		/*! Resume the download */
		virtual void resumeDownload() = 0;
		/*! Cancel the download */
		virtual void cancelDownload() = 0;
	protected slots:
		/*! A new download started */
		void downloadStarted();
		/*! Download has been paused */
		void downloadPaused(const QFileInfo destFile);
		/*! Download has resumed */
		void downloadResumed();
		/*! A download finished */
		void downloadFinished(const QFileInfo destFile);
		/*! Download has been canceled */
		void downloadCanceled();
		/*! Ops, an error ocurred */
		void downloadError(int error);
		/*! A single download event */
		void downloadEvent(int pos, int max);
	signals:
		/*! Video item has been modified */
		void videoItemUpdated_child(VideoItem *videoItem);
		/*! A Video item download started */
		void downloadStarted_child(VideoItem *videoItem);
		/*! A Video item download finished */
		void downloadFinished_child(VideoItem *videoItem);
		/*! This signal is send when all work finished */
		void downloadDestroyable();
};

/*! Single download via HTTP */
class DownloadItem_HTTP : public DownloadItem
{
	private:
		Http *http;	//!< download class
		/*! Set download speed */
		int getDownloadSpeed();
		/*! Set time remaining */
		int getTimeRemaining();
		/*! Get the video size */
		int getFileSize();
	public:
		/*! Class constructor */
		DownloadItem_HTTP(VideoDownload *parent, VideoItem *videoItem);
		/*! Class destructor */
		~DownloadItem_HTTP();
		/*! Start the download */
		void startDownload();
		/*! Pause the download */
		void pauseDownload();
		/*! Resume the download */
		void resumeDownload();
		/*! Cancel the download */
		void cancelDownload();
		/*! Update the http configuration */
		void updateConfiguration();
};

/*! Single download via RTMP */
class DownloadItem_RTMP : public DownloadItem
{
	private:
		RTMP *rtmp;	//!< download class
		/*! Set download speed */
		int getDownloadSpeed();
		/*! Set time remaining */
		int getTimeRemaining();
		/*! Get the video size */
		int getFileSize();
	public:
		/*! Class constructor */
		DownloadItem_RTMP(VideoDownload *parent, VideoItem *videoItem);
		/*! Class destructor */
		~DownloadItem_RTMP();
		/*! Start the download */
		void startDownload();
		/*! Pause the download */
		void pauseDownload();
		/*! Resume the download */
		void resumeDownload();
		/*! Cancel the download */
		void cancelDownload();
};

/*! Downloads controller */
class VideoDownload : public QObject
{
Q_OBJECT
	private:
		QList<DownloadItem *> *downloads;		//!< active downloads
		QString downloadDir;					//!< where to download videos
		int maxActiveDownloads;					//!< maximum active downloads
		QList<DownloadItem *> *garbageCollecotrQueue;	//!< queue of downloads to destroy
		/*! Find a download item by a video item*/
		DownloadItem* findDownloadItemByVideoItem(VideoItem *videoItem);
		/*! Stop all downloads */
		void stopAllDownloads(bool doCancel = false);
	public:
		/*! Class csontructor */
		VideoDownload(QString downloadDir, int maxActiveDownloads);
		/*! class destructor */
		~VideoDownload();
		/*! start to download video */
		void downloadVideo(VideoItem *videoItem);
		/*! pause the video download */
		void pauseDownload(VideoItem *videoItem);
		/*! resume the video download */
		void resumeDownload(VideoItem *videoItem);
		/*! cancel download */
		void cancelDownload(VideoItem *videoItem);
		/*! pause all downloads */
		void pauseAllDownloads();
		/*! cancel all downloads */
		void cancelAllDownloads();
		/*! Get if possible start a new download */
		bool canStartDownload();
		/*! Get if is downloading some video */
		bool isDownloading();
		/*! Get where to save downloads */
		QString getDownloadDir();
		/*! Set where to save downloads */
		void setDownloadDir(QString downloadDir);
		/*! Get how many downloads can run simultaneous */
		int getMaxActiveDownloads();
		/*! Set how many downloads can run simultaneous */
		void setMaxActiveDownloads(int maxActiveDownloads);
		/*! Update configurations */
		void updateHttpConfiguration();
	private slots:
		/*! Video item has been modified */
		void videoItemUpdated_child(VideoItem *videoItem);
		/*! A Video item download started */
		void downloadStarted_child(VideoItem *videoItem);
		/*! A Video item download finished */
		void downloadFinished_child(VideoItem *videoItem);
		/*! The download object can be destroyed (and deleted from list) */
		void downloadDestroyable();
		/*! Destroy the last downloadItem finished */
		void destroyDownload();
	signals:
		/*! Video item has been modified */
		void videoItemUpdated(VideoItem *videoItem);
		/*! A Video item download started */
		void downloadStarted(VideoItem *videoItem);
		/*! A Video item download finished */
		void downloadFinished(VideoItem *videoItem);
};

#endif // __VIDEODOWNLOAD_H__

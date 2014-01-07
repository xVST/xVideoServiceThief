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

#ifndef __VIDEOITEM_H__
#define __VIDEOITEM_H__

#include <QtWidgets>

#include "videoconverttypes.h"

enum VideoState
{
	vsNULL,
	vsGettingURL, vsGettedURL,
	vsDownloading, vsDownloaded,
	vsConverting, vsConverted,
	vsCompleted,
	vsCanceled,
	vsBlocked,
	vsError,
	vsDeleted,
	vsPaused, vsResuming,
	vsNeedLogin,
	vsReadyAndPaused
};

enum VideoUpdateState
{
	vusNothing,
	vusNeedUpdateURL,
	vusUpdatingURL
};

enum VideoPreState
{
	vpsNothing,
	vpsPreDownloading,
	vpsPreResuming,
	vpsPreResumingReadyPaused
};

struct VideoDefinition
{
    QString URL;            //!< real video URL
    QString title;          //!< video title
    QString extension;      //!< video extension (default = flv)
    bool needLogin;         //!< flag for know if this video need a login
    bool isAudioFile;       //!< flag for know if is a audio file
    QString cookies;        //!< special cookies for this video (only if is needed)
    QString headers;        //!< special headers for this video (only if is needed)
    QString userAgent;      //!< special userAgent for this video (only if is needed)
    QStringList rtmpParams; //!< RTMP parameters (only if is needed)
	/*! Struct initializer */
	VideoDefinition()
	{
		URL = "";
		title = "";
		extension = ".flv";
		needLogin = false;
		isAudioFile = false;
		cookies = "";
		headers = "";
		userAgent = "";
        rtmpParams = QStringList();
	}
};

struct OverridedVideoConversionConfig
{
	bool convertVideo;
	VideoConversionConfig videoConversionConfig;
};

const static int TIMEOUT_MINS = 5;

class VideoItem : public QObject
{
Q_OBJECT
	private:
		static int internalID;			//!< internal id counter
		int ID;							//!< internal assigned id for this instance
		VideoDefinition videoInfo;		//!< internal video information
		QString URL;					//!< initial video url (i.e.: http://youtube.com/watch?v=AzXR58sd2jY)
		QString videoFile;				//!< downloaded file (*.flv)
		QString videoFileSavedTo;		//!< where to find the final video
		int errorCode;					//!< what error is (if exsit an error)
		float progress;					//!< current action progress (downloading/converting)
		int videoSize;					//!< video file size in bytes
		int downloadSpeed;				//!< current downoad speed
		int timeRemaining;				//!< current time remaining
		VideoState videoState;			//!< current video state
		VideoPreState videoPreState;	//!< current video pre-state
		QObject *locker;				//!< pointer to the locker
		bool reported;					//!< flag for know if this item has been reported (error)
		bool audioFile;					//!< flag for know if this item is a "Audio only" (no conversion...)
		bool overrideConversionConfig;						//!< flag for know if this video has an overrided conversion config
		OverridedVideoConversionConfig overridedConvConf;	//!< overrided conversion config for this video
		bool customItemDownload;		//!< flag for know if this item has been added as "Custom download" so no plugin is needed to download it
		QDateTime lastUpdateDateTime;	//!< used as timeout for url's flv
		VideoUpdateState updateSate;	//!< current update video state
		/*! Init internal data */
		void initData();
		/*! Assign a unique ID for this instance */
		void assignID();
	public:
		/*! Class constructor */
		VideoItem();
		/*! Class constructor */
		VideoItem(const QString URL);
		/*! Class constructor */
		VideoItem(const QString URL, const OverridedVideoConversionConfig overridedConversionConfig);
		/*! Assign video info */
		void assign(VideoItem *videoItem);
		/*! Lock item */
		bool lock(QObject *locker);
		/*! Unlock item (only who locked the item can ulnock it) */
		bool unlock(QObject *locker);
		/*! Get if is currently locked */
		bool isLocked();
		/*! Get the current state */
		VideoState getVideoState();
		/*! Get the current state as String */
		QString getVideoStateAsString();
		/*! Get if is NULL yet */
		bool isNULL();
		/*! Get if is Ready to be downloaded */
		bool isReady();
		/*! Get if is Ready but paused (download didn't started yet) */
		bool isReadyAndPaused();
		/*! Get if is Getting the video URL */
		bool isGettingURL();
		/*! Get if has the video URL */
		bool isGettedURL();
		/*! Get if this item can be downloaded */
		bool isDownloadable();
		/*! Get if is Downloading */
		bool isDownloading();
		/*! Get if has been Downloaded */
		bool isDownloaded();
		/*! Get if is Converting */
		bool isConverted();
		/*! Get if has been Converted */
		bool isConverting();
		/*! Get if this item has been finished */
		bool isCompleted();
		/*! Get if is Blocked */
		bool isBlocked();
		/*! Get if has Errors */
		bool hasErrors();
		/*! Get if is an "important" error */
		bool hasAnImportantError();
		/*! Get if this item is Bussy */
		bool isBussy();
		/*! Get if is removable */
		bool isRemovable();
		/*! Get if is renameable */
		bool isRenameable();
		/*! Get if this item is canceled */
		bool isCanceled();
		/*! Get if is market as "deleted" */
		bool isDeleted();
		/*! Get if is pausable */
		bool isPauseable();
		/*! Get if is paused */
		bool isPaused();
		/*! Get if is resetable */
		bool isResetable();
		/*! Get if is any kind of paused (paused or readyPaused) */
		bool isAnyKindOfPaused();
		/*! Get if is being resumed */
		bool isResuming();
		/*! Get if is reported */
		bool isReported();
		/*! Get if is an Audio file */
		bool isAudioFile();
		/*! Get if don't has any pre-state */
		bool isPreStateNothing();
		/*! Get if is pre-downloading */
		bool isPreDownloading();
		/*! Get if is pre-resuming */
		bool isPreResuming();
		/*! Get if is pre-resuming a readyPaused item */
		bool isPreResumingReadyPaused();
		/*! Get if is a custom download */
		bool isCustomDownload();
		/*! Get last update date */
		bool isUrlExpired();
		/*! Get if is updating the URL */
		bool isUpdatingUrl();
		/*! Get if need to update the URL (high priority) */
		bool needUpdateUrl();
		/*! Get if need login */
		bool needLogin();
		/*! Get the internal ID */
		int getID();
		/*! Get if has an overrided conversion config */
		bool hasOverridedConversion();
		/*! Get the overrided conversion config (warning: it may not be initialized if no overrided config is assigned) */
		OverridedVideoConversionConfig getOverridedVideoConversionConfig();
		/*! Get the display label */
		QString getDisplayLabel();
		/*! Get the display size */
		QString getDisplaySize();
		/*! Get the display progress */
		QString getDisplayProgress();
		/*! Get the display time remaining */
		QString getDisplayTimeRemaining();
		/*! Get the display download speed */
		QString getDisplayDownloadSpeed();
		/*! Get video info */
		VideoDefinition getVideoInformation();
		/*! Get the Video URL */
		QString getURL();
		/*! Get the video file */
		QString getVideoFile();
		/*! Get the video file saved to */
		QString getVideoFileSavedTo();
		/*! Get the current progress value */
		float getProgress();
		/*! Get the video file size */
		int getVideoSize();
		/*! Get download speed */
		int getDownloadSpeed();
		/*! Get time remaingin */
		int getTimeRemaining();
		/*! Get the error code */
		int getErrorCode();
		/*! Get the error as String */
		QString getErrorMessage();
		/*! Get last update date time */
		QDateTime getLastUpdateDateTime();
		/*! Set the video information */
		void setVideoInformation(VideoDefinition videoInformation, QObject *who = NULL);
		/*! Set video file */
		void setVideoFile(QString videoFile, QObject *who = NULL);
		/*! Set video file saved to */
		void setVideoFileSavedTo(QString videoFileSavedTo, QObject *who = NULL);
		/*! Set the current progress value */
		void setProgress(float progress, QObject *who = NULL);
		/*! Set the video file size */
		void setVideoSize(int videoSize, QObject *who = NULL);
		/*! Set the current download speed */
		void setDownloadSpeed(int downloadSpeed, QObject *who = NULL);
		/*! Set the time remaining */
		void setTimeRemaining(int timeRemaining, QObject *who = NULL);
		/*! Set the error code */
		void setErrorCode(int errorCode, QObject *who = NULL);
		/*! Set state */
		void setState(VideoState videoState, QObject *who = NULL);
		/*! Set as NULL */
		void setAsNULL(QObject *who = NULL);
		/*! Set as ready */
		void setAsReady(QObject *who = NULL);
		/*! Set as ready and paused */
		void setAsReadyAndPaused(QObject *who = NULL);
		/*! Set as getting url */
		void setAsGettingURL(QObject *who = NULL);
		/*! Set as getted url */
		void setAsGettedURL(QObject *who = NULL);
		/*! Set as downloading */
		void setAsDownloading(QObject *who = NULL);
		/*! Set as downloaded */
		void setAsDownloaded(QObject *who = NULL);
		/*! Set as converting */
		void setAsConverting(QObject *who = NULL);
		/*! Set as converted */
		void setAsConverted(QObject *who = NULL);
		/*! Set as completed */
		void setAsCompleted(QObject *who = NULL);
		/*! Set as canceled */
		void setAsCanceled(QObject *who = NULL);
		/*! Set as blocked */
		void setAsBlocked(QObject *who = NULL);
		/*! Set as error */
		void setAsError(QObject *who = NULL);
		/*! Set as deleted */
		void setAsDeleted(QObject *who = NULL);
		/*! Set as paused */
		void setAsPaused(QObject *who = NULL);
		/*! Set as resuming */
		void setAsResuming(QObject *who = NULL);
		/*! Set as Reported */
		void setAsReported(QObject *who = NULL);
		/*! Set as Audio file */
		void setAsAudioFile(QObject *who = NULL);
		/*! Set as Need login */
		void setAsNeedLogin(QObject *who = NULL);
		/*! Set as no pre-state */
		void setAsNothingPreState();
		/*! Set a custom pre-state */
		void setCustomPreState(VideoPreState preState = vpsNothing);
		/*! Set as Custom download */
		void setAsCustomDownload();
		/*! Set as Need update URL */
		void setAsNeedUpdateURL(VideoPreState preState = vpsNothing);
		/*! Set as updating URL */
		void setAsUpdatingURL();
		/*! Remove the updating URL status (aborted?) */
		void removeUpdatingURLStatus();
		/*! Set last update date time */
		void setLastUpdateDateTime(QDateTime dateTime);
		/*! Init a VideoDefinition structure */
		static void initVideoDefinition(VideoDefinition &videoDef);
};

#endif // __VIDEOITEM_H__

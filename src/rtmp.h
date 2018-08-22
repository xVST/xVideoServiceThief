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

#ifndef RTMP_H
#define RTMP_H

#include <QtCore>

#ifdef Q_OS_WIN32
static const QString RTMP_APP_PATH = "rtmpdump.exe";	//!< RTMPDump app (Win32)
#else
static const QString RTMP_APP_PATH = "rtmpdump";		//!< RTMPDump app (*unix)
#endif

class ArrayAvg;

namespace EnumRTMP
{
	enum Error
	{
		NO_RTMP_ERROR = 0,		//0
		RTMP_APP_MISSING = 100,	//100
		FAILED_TO_OPEN_FILE,	//101
		FAILED_TO_CONNECT,		//102
		COULDNT_RESUME_FLV,		//103
		DOWNLOAD_INCOMPLETE,	//104
		PTHREAD_CREATE_FAILED,	//105
		FILE_NOT_FOUND			//106
	};

	enum StopReason
	{
		NO_STOPPED = 200,			//200
		DOWNLOAD_FINISHED,			//201
		USER_CANCELLED,				//202
		USER_PAUSED,				//203
		DOWNLOAD_ERROR				//204
	};
}

/*! rtmpdump Qt4 wrapper */
class RTMP : public QObject
{
Q_OBJECT
	private:
		QProcess *rtmpProcess;				//!< Rtmpdump process
		QString rtmpAppPath;				//!< Base dir where Rtmpdump is located (ie: /users/xesk/.xvst/)
		ArrayAvg *downloadSpeedAvg;			//!< Download speed avg calculator
		ArrayAvg *timeRemainingAvg;			//!< Time remaining avg calculator
		QFileInfo destinationFile;			//!< Destination file information
		EnumRTMP::StopReason stopReason;	//!< Flag for know if the user aborted
		bool resuming;						//!< Flag for know if is being resumed
		int fileSize;						//!< File size to download
		int downloadSpeed;					//!< Current download speed (bytes second)
		int internalTimer;					//!< Internal timer id
		int currentDownloadedSize;			//!< Currend downloaded size
		int lastDownloadedSize;				//!< Last downloaded size
		int timeRemaining;					//!< Time remaining in seconds
		bool pauseOnDestroyF;				//!< Should pause the download instead of cancel it?
		int errorCode;						//!< Last error code detected
		bool usingPercentage;				//!< Flag for know if we are fetching the % instead of size
		float currentPercentage;			//!< Stores the current percentage
		float lastPercentage;				//!< Stores the latest percentage
		/*! Parse the current output text */
		void parseOutput(QString output);
		/*! Single timer shot */
		void singleTimerShot();
		/*! Init internal variables */
		void init();
	public:
		/*! Class constructor */
		RTMP(QString rtmpAppPath, QString workingDir);
		/*! Class destructor */
		~RTMP();
		/*! Return if Rtmpdump is installed (detected) */
		bool isRtmpappInstalled();
		/*! Start a new asynchronously download */
		int download(const QString URL, QString destination, QString fileName = "", QStringList params = QStringList(), bool autoName = true);
		/*! Resume a previous asynchronously download */
		int resume(const QString URL, QString fileName);
		/*! Pause the current asynchronously download */
		void pause();
		/*! Cancel current download */
		void cancel();
		/*! Pause on destroy the RTMP class (only if is downloading) */
		void pauseOnDestroy(bool pauseOnDestroyF = true);
		/*! Get if is downloading */
		bool isDownloading();
		/*! Get file size */
		int getFileSize();
		/*! Get download speed */
		int getDownloadSpeed();
		/*! Get time remaining */
		int getTimeRemaining();
		/*! Get the destination file name */
		QFileInfo getDestinationFile();
	private slots:
		/*! Rtmpdump started */
		void started();
		/*! Rtmpdump finished */
		void finished(int exitCode, QProcess::ExitStatus exitStatus);
		/*! output recived */
		void readyReadStandardOutput();
		/*! output by error recived */
		void readyReadStandardError();
		/*! Internal timer event */
		void downloadTimerEvent();
	signals:
		/*! when a download started */
		void downloadStarted();
		/*! when a download finished */
		void downloadFinished(const QFileInfo destFile);
		/*! when a download has been paused */
		void downloadPaused(const QFileInfo destFile);
		/*! when a download has been resumed */
		void downloadResumed();
		/*! when a download file has been canceled */
		void downloadCanceled();
		/*! an error ocurred during the download process */
		void downloadError(int error);
		/*! when the rtmp read data */
		void downloadEvent(int pos, int max);
};

#endif // RTMP_H

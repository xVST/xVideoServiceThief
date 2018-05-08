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

#ifndef YOUTUBEDL_H
#define YOUTUBEDL_H

#include <QtCore>

#ifdef Q_OS_WIN32
static const QString YOUTUBE_DL_APP_PATH = "youtube-dl.exe"; //!< Youtube-dl app (win32)
#else
static const QString YOUTUBE_DL_APP_PATH = "youtube-dl"; //!< Youtube-dl app (*unix)
#endif

namespace EnumYoutubeDL
{
	enum Error
	{
        NO_YOUTUBE_DL_ERROR    = 0,		//0
        YOUTUBE_DL_APP_MISSING = 300,	//300
        INVALID_JSON_RESPONSE,          //301
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

/*! YoutubeDL Qt5 wrapper */
class YoutubeDL : public QObject
{
Q_OBJECT
    private:
        QProcess *process;                      //!< YoutubeDL process
        QString appPath;                        //!< Base dir where Youtube-dl is located (ie: /users/xesk/.xvst/)
        QFileInfo destinationFile;              //!< Destination file information
        EnumYoutubeDL::StopReason stopReason;	//!< Flag for know if the user aborted
        bool resuming;                          //!< Flag for know if is being resumed
        int fileSize;                       	//!< File size to download (bytes)
        int downloadSpeed;                      //!< Current download speed (bytes second)
        bool pauseOnDestroyF;               	//!< Should pause the download instead of cancel it?
        int timeRemaining;                      //!< Time remaining in seconds
        int errorCode;                          //!< Last error code detected
        QString errorMessage;                   //!< Last error message detected
        /*! Init internal variables */
        void init();
        /*! Parse the current output text */
        void parseOutput(QString output);
    public:
        /*! Class constructor */
        YoutubeDL(QString appPath, QString workingDir);
        /*! Class destructor */
        ~YoutubeDL();
        /*! Return if Youtube-DL is installed (detected) */
        bool isYoutubeDLappInstalled();
        /*! Start a new asynchronously download */
        int download(const QString URL, QString destination, QString fileName = "", QStringList params = QStringList(), bool autoName = true);
        /*! Resume a previous asynchronously download */
        int resume(const QString URL, QString fileName);
        /*! Pause the current asynchronously download */
        void pause();
        /*! Cancel current download */
        void cancel();
        /*! Pause on destroy the Youtube-DL class (only if is downloading) */
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
        /*! Get information  */
        QJsonDocument getVideoInformation(const QString URL);
    private slots:
        /*! Rtmpdump started */
        void started();
        /*! Rtmpdump finished */
        void finished(int exitCode, QProcess::ExitStatus exitStatus);
        /*! output recived */
        void readyReadStandardOutput();
        /*! output by error recived */
        void readyReadStandardError();
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
        /*! when the youtube-dl read data */
        void downloadEvent(int pos, int max);
};

#endif // YOUTUBEDL_H

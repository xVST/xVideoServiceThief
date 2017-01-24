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

#ifndef HTTP_H
#define HTTP_H

#include <QtCore>
#include <QtNetwork>

namespace EnumHTTP
{
	enum Error
	{
		NO_HTTP_ERROR = 0,				//0
		UNABLE_CREATE_DIR = 20,		//20
		UNABLE_CREATE_FILE,			//21
		INVALID_URL,				//22
		ALREADY_DOWNLOADING,		//23
		INVALID_INITIAL_FILE_SIZE,	//24
		MISSING_RESUME_FILE,		//25
		UNABLE_RESUME_DOWNLOAD,		//26
		UNABLE_APPEND_FILE,			//27
		TOO_MUCH_REDIRECTIONS,		//28
		INTERNAL_NETWEORK_TIME_OUT,	//29
		CONNECTION_REFUSED,			//30
		HOST_NOT_FOUND_ERROR,		//31
		CONTENT_ACCESS_DENIED,		//32
		CONTENT_NOT_FOUND,			//33
		UNKNOW_NETWEORK_ERROR		//34
	};

	enum StopReason
	{
		NO_STOPPED = 100,			//100
		DOWNLOAD_FINISHED,			//101
		USER_CANCELLED,				//102
		USER_PAUSED,				//103
		TIME_OUT,					//104
		MAX_AUTO_JUMPS_REACHED,		//105
		AUTO_RESTART,				//106
		CANNOT_RESUME				//107
	};

	enum HttpMethod
	{
		httpGet,
		httpPost,
		httpHead,
		httpDeleteResource
	};
}

class ArrayAvg : public QObject
{
	private:
		QList<float> arrayAvg;	//!< Array of values (float)
		int maxItems;			//!< Max. possible values
	public:
		/*! Class constructor */
		ArrayAvg(const int maxItems = 10);
		/*! Class dsetructor */
		~ArrayAvg();
		/*! Add new value */
		float add(const float value);
		/*! Get array avarage */
		float avg();
		/*! Reset array */
		void reset();
};

class Cookies : public QNetworkCookieJar
{
	public:
		/*! Add custom user cookie */
		void addCookie(QString cookieInf);
		/*! Add a list of custom cookies */
		void addCookies(QString cookies, const QString separator = "|");
		/*! Get all stored cookies as String */
		QString getCookies(const QString separator = "|");
		/*! Clear the stored cookies */
		void clearCookies();
};

static QString HTTP_GLOBAL_USER_AGENT = QString();

class Http : public QObject
{
Q_OBJECT
	private:
		QNetworkAccessManager *http;		//!< Http protocol
		QNetworkReply *currentReply;		//!< Current http reply

		EnumHTTP::HttpMethod httpMethod;	//!< Http request method: get, post or head
		EnumHTTP::StopReason stopReason;	//!< Stores the stop reason (User aborted, canceled, failed...)

		ArrayAvg *downloadSpeedAvg;	//!< Download speed avgarage calculator
		ArrayAvg *timeRemainingAvg;	//!< Time remaining avgarage calculator

		QStringList *customHeaders;	//!< List of user custom header parameters

		bool cookiesEnabled;		//!< Flag for know if cookies are anabled or not

		bool pauseOnDestroyF;		//!< Should pause the download instead of cancel it?

		bool resuming;				//!< Flag for know if we are resuming a download

		bool autoRestartOnFail;		//!< Restart the download on fail?

		QString userAgent;			//!< "user agent" used for this http instance

		QFile *outputFile;			//!< Destination file
		QFileInfo destFile;			//!< Destination file information

		QUrl oriURL;				//!< First url (original)
		bool syncFlag;				//!< Flag for know if is a synchronously request

		QString data;				//!< Internal downloaded data
		QString parameters;			//!< Internal post parameters

		int maxAutoJumps;			//!< Maximum automatic jumps
		int autoJumps;				//!< Current automatic jumps realized
		bool autoJump;				//!< Flag for know if automatic jumps are enabled

		bool timeOut;				//!< Flag for know if the time out option is enabled
		int timeOutSeconds;			//!< Timeout in seconds
		int timeOutTimerId;			//!< Timeout timer id

		int maxRetries;				//!< Maximum number of retries for session (each download is a session)
		int retriesCount;			//!< Current retries in this session (each download is a session)

		bool skipExistentFiles;		//!< Skip files which are already present (downloaded)

		int totalToDownloadSize;		//!< File size which are downloading (in bytes)
		int totalDownloadedSize;		//!< Current downloaded data size (in bytes)
		int prevTotalDownloadedSize;	//!< Latest current downloaded data size, used in time-remaining calculation (in bytes)
		int initialDownloadSize;		//!< Previous downloaded data size, used for resume downloads (in bytes)

		float currentPercentage;	//!< Stores the current percentage
		float lastPercentage;		//!< Stores the latest percentage

		int downloadSpeed;			//!< Current download speed (in bytes)

		int timeLeft;					//!< Current time left (in seconds)
		int timeLeftTimerId;			//!< Time remaining calculation timer
		bool usePercentageForTimeLeft;	//!< Flag for know how to calcule the time remaining

		bool downloadStartedFlag;	//!< Flag for know if the real download started

		bool deleteFileOnError;		//!< Flag for know if the partial downloaded data is removed on fail
		int lastErrorCode;			//!< Stores the last error code

		/*! Init Http class */
		void initClass();
		/*! Init data */
		void initData();
		/*! Copy class */
		void copyHttpObject(const Http &);
		/*! Clear the current (last?) reply */
		void clearCurrentReply();
		/*! Executes a new http request */
		void jumpToURL(QUrl url);
		/*! Create a sync. request */
		QString syncRequest(EnumHTTP::HttpMethod httpMethod, QUrl url, QString parameters, bool isUtf8);
		/*! Start the time out timer */
		void startTimeOutTimer();
		/*! Stop the active time out timer */
		void stopActiveTimeOutTimer();
		/*! Start the time left timer */
		void startTimeLeftTimer();
		/*! Stop the time left timer */
		void stopTimeLeftTimer();
		/*! Inherited timer event */
		void timerEvent(QTimerEvent *event);
		/*! Abort current request with a stop reason */
		void abortRequest(EnumHTTP::StopReason stopReason);
		/*! Close and dealloc the output file */
		void closeOutputFile();
		/*! Delete the output file */
		void deleteOutputFile();
		/*! Delete the downloaded file and send the download error */
		void sendDownloadError(int error);
		/*! Check if can continue or not with the time out error */
		void timeOutDownloadError();
		/*! Convert a ReplyError to EnumHTTP::Error */
		EnumHTTP::Error networkReplyToEnumHTTP(QNetworkReply::NetworkError error);
		/*! Reset retries counter */
		void resetRetries();
		/*! Check if can restart the current failed download */
		bool canRestartFailedDownload();
		/*! Restart a failded download */
		void restartFailedDownload();
	public:
		/*! Class constructor  */
		Http();
		/*! Copy class constructor */
		Http(const Http&);
		/*! Override = operator */
		Http &operator=(const Http&);
		/*! Class destructor */
		~Http();
		/*! Start a new asynchronously download */
		int download(const QUrl URL, QString destination, QString fileName = QString(), bool autoName = true);
		/*! Resume a previous asynchronously download */
		int resume(const QUrl URL, QString fileName, bool autoRestartOnFail = true);
		/*! Pause the current asynchronously download */
		void pause();
		/*! Cancel current download */
		void cancel();
		/*! Download a Webpage synchronously (return the webpage content) */
		QString downloadWebpage(const QUrl URL, bool isUtf8 = true);
		/*! Download a Webpage synchronously: post mode (return the webpage content) */
		QString downloadWebpagePost(const QUrl URL, QString parameters, bool isUtf8 = true);
		/*! Get only the response headers */
		QString head(const QUrl URL);
		/*! Add custom cookie */
		void addCookie(QString cookie);
		/*! Add custom cookies */
		void addCookies(QString cookies, const QString separator = "|");
		/*! Get all stored cookies as String */
		QString getCookies(const QString separator = "|");
		/*! Clear the stored cookies */
		void clearCookies();
		/*! Add custom header parameter */
		void addHeaderParameter(QString key, QString value);
		/*! Add custom header parameters */
		void addHeaderParameters(QString headers, const QString separator = "|");
		/*! Clear custom header parameters */
		void clearHeaderParameters();
		/*! Get if is downloading */
		bool isDownloading();
		/*! Get file size */
		int getFileSize();
		/*! Get download speed */
		int getDownloadSpeed();
		/*! Get time remaining */
		int getTimeRemaining();
		/*! Get the destination file name */
		QFileInfo getDestiationFile();
		/*! Get the last error generated (on start a new download, this value is reset) */
		int getLastError();
		/*! Get the last stop reason (on start a new download, this value is reset) */
		int getLastStopReason();
		/*! Set the user agent for this http instance (this override the global user agent) */
		void setUserAgent(QString value);
		/*! Set the global (default) HTTP user agent */
		static void setGlobalUserAgent(QString value);
		/*! Set if automatic jumps are enabled */
		void setAutoJumps(bool value);
		/*! Set the max automatic jumps */
		void setMaxAutoJumps(int value);
		/*! Set if the time out option is enabled */
		void setTimeOutOption(bool value);
		/*! Set the time out in seconds */
		void setTimeOut(int value);
		/*! Set the max retries */
		void setMaxRetries(int value);
		/*! Set if should skip existent files (avoid download duplicated data) */
		void setSkipExistentFiles(bool value);
		/*! Pause on destroy the Http class (only if is downloading) */
		void setPauseOnDestroy(bool value);
		/*! Set how to calculate the time remaining */
		void setUsePercentageForTimeRemaining(bool value);
		/*! Set if cookies are enabled or not */
		void setCookiesEnabled(bool value);
		/*! Set if the partial downloaded data should be removed on fail */
		void setDeleteFileOnError(bool value);
	private slots:
		/*! Http netweork request finished */
		void finished(QNetworkReply *reply);
		/*! Http network download ready read */
		void readyRead();
		/*! Http network download progress */
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		/*! Http network metadata changed */
		void metaDataChanged();
		/*! Restart the current download (on resume fail or auto-retry) */
		void restartDownloadSignal();
	signals:
		/*! When a download started */
		void downloadStarted();
		/*! When a download finished */
		void downloadFinished(const QFileInfo destFile);
		/*! When a download has been paused */
		void downloadPaused(const QFileInfo destFile);
		/*! When a download has been resumed */
		void downloadResumed();
		/*! When a download file has been canceled */
		void downloadCanceled();
		/*! An error ocurred during the download process */
		void downloadError(int error);
		/*! When the http read data */
		void downloadEvent(int pos, int max);
	};

#endif // HTTP_H

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

#include "http.h"

#include "tools.h"

// ArrayAvg class

ArrayAvg::ArrayAvg(const int maxItems)
{
	this->maxItems = maxItems;
	reset();
}

ArrayAvg::~ArrayAvg()
{
	arrayAvg.clear();
}

float ArrayAvg::add(const float value)
{
	if (arrayAvg.count() >= maxItems)
		arrayAvg.removeAt(0);

	arrayAvg.append(value);

	return avg();
}

float ArrayAvg::avg()
{
	float total = 0.0;

	for (int n = 0; n < arrayAvg.count(); n++)
		total += arrayAvg.at(n);

	if (arrayAvg.count() > 0)
		return total / arrayAvg.count();
	else
		return 0.0;
}

void ArrayAvg::reset()
{
	arrayAvg.clear();
}

// Cookies class

void Cookies::addCookie(QString cookieInf)
{
	// add those new cookies (cookie) into our current cookies list
	QList<QNetworkCookie> storedCookies = allCookies();
	storedCookies.append(QNetworkCookie::parseCookies(cookieInf.toLocal8Bit()));
	setAllCookies(storedCookies);
}

void Cookies::addCookies(QString cookies, const QString separator)
{
	// split cookies
	QStringList cookiesList = cookies.split(separator);
	// add all cookies
	foreach (QString cookie, cookiesList)
		addCookie(cookie);
}

QString Cookies::getCookies(const QString separator)
{
	QString result = QString();
	// get all stored cookies
	foreach (QNetworkCookie cookie, allCookies())
		result += QString(cookie.toRawForm()) + separator;
	// remove last separator
	if (!result.isEmpty())
		result = result.remove(result.length() - 1, 1);
	// return cookies
	return result;
}

void Cookies::clearCookies()
{
	setAllCookies(QList<QNetworkCookie>());
}

// Http class

Http::Http()
{
	initClass();
}

Http::Http(const Http &other): QObject()
{
	initClass();
	// copy internal data from "other"
	copyHttpObject(other);
}

Http &Http::operator=(const Http &other)
{
	// copy internal data from "other"
	copyHttpObject(other);
	// return my self updated
	return *this;
}

Http::~Http()
{
	if (pauseOnDestroyF) pause(); else cancel();
	clearCurrentReply();
	closeOutputFile();
	stopActiveTimeOutTimer();
	stopTimeLeftTimer();
	delete customHeaders;
	delete timeRemainingAvg;
	delete downloadSpeedAvg;
	delete http;
}

void Http::initClass()
{
	setObjectName("Http");
	// http protocol
	http = new QNetworkAccessManager();
	http->setCookieJar(new Cookies());
	currentReply = NULL;
	outputFile = NULL;
	// download speed avg calculator
	downloadSpeedAvg = new ArrayAvg(100);
	timeRemainingAvg = new ArrayAvg(100);
	// init retries
	resetRetries();
	// init custom header parameters
	customHeaders = new QStringList();
	// default configuration
	setUserAgent(QString());
	setAutoJumps(true);
	setMaxAutoJumps(5);
	setMaxRetries(3);
	setTimeOutOption(true);
	setTimeOut(30);
	setSkipExistentFiles(false);
	setPauseOnDestroy(false);
	setUsePercentageForTimeRemaining(true);
	setCookiesEnabled(true);
	setDeleteFileOnError(true);
	// init others
	timeOutTimerId = -1;
	timeLeftTimerId = -1;
	// connect signals
	connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

void Http::initData()
{
	httpMethod = EnumHTTP::httpGet;
	stopReason = EnumHTTP::NO_STOPPED;
	resuming = false;
	syncFlag = false;
	downloadStartedFlag = false;
	autoRestartOnFail = false;
	oriURL.clear();
	autoJumps = 0;
	clearCurrentReply();
	stopActiveTimeOutTimer();
	data = QString();
	totalToDownloadSize = 0;
	totalDownloadedSize = 0;
	prevTotalDownloadedSize = 0;
	initialDownloadSize = 0;
	currentPercentage = 0.0;
	lastPercentage = 0.0;
	downloadSpeed = 0;
	timeLeft = 0;
	lastErrorCode = 0;
}

void Http::copyHttpObject(const Http &other)
{
	// copy configuration
	setUserAgent(other.userAgent);
	setAutoJumps(other.autoJumps);
	setMaxAutoJumps(other.maxAutoJumps);
	setMaxRetries(other.maxRetries);
	setTimeOutOption(other.timeOut);
	setTimeOut(other.timeOutSeconds/1000);
	setSkipExistentFiles(other.skipExistentFiles);
	setPauseOnDestroy(other.pauseOnDestroyF);
	setUsePercentageForTimeRemaining(other.usePercentageForTimeLeft);
	setCookiesEnabled(other.cookiesEnabled);
	// copy destination file
	if (other.outputFile)
	{
		closeOutputFile();
		outputFile = new QFile(other.outputFile->fileName());
	}
	// copy stored cookies
	clearCookies();
	addCookies(static_cast<Cookies*>(other.http->cookieJar())->getCookies());
	// copy custom header parameters
	delete customHeaders;
	customHeaders = new QStringList(*other.customHeaders);
}

void Http::clearCurrentReply()
{
	if (currentReply)
	{
		// disconnect local signals
		disconnect(currentReply, SLOT(readyRead()));
		disconnect(currentReply, SLOT(downloadProgress(qint64, qint64)));
		disconnect(currentReply, SLOT(metaDataChanged()));
		// destroy the current reply
		currentReply->deleteLater();
		currentReply = NULL;
	}
}

void Http::startTimeOutTimer()
{
	// stop any active time out
	stopActiveTimeOutTimer();
	// start the time out timer
	timeOutTimerId = this->startTimer(timeOutSeconds);
}

void Http::stopActiveTimeOutTimer()
{
	// kill the active timer
	if (timeOutTimerId != -1) this->killTimer(timeOutTimerId);
	// reset the timer id
	timeOutTimerId = -1;
}

void Http::startTimeLeftTimer()
{
	// stop the possible active timer
	stopTimeLeftTimer();
	// start the time left timer
	timeLeftTimerId = this->startTimer(1000);
}

void Http::stopTimeLeftTimer()
{
	// kill the active timer
	if (timeLeftTimerId != -1) this->killTimer(timeLeftTimerId);
	// reset the timer id
	timeLeftTimerId = -1;
}

void Http::timerEvent(QTimerEvent *event)
{
	// time out timer
	if (event->timerId() == timeOutTimerId)
	{
		if (timeOut) abortRequest(EnumHTTP::TIME_OUT);
		// stop this timer
		this->killTimer(timeOutTimerId);
		// reset timer id
		timeOutTimerId = -1;
	}
	// time left timer
	else if (event->timerId() == timeLeftTimerId)
	{
		// calcule download speed
		downloadSpeed = static_cast<int>(downloadSpeedAvg->add(totalDownloadedSize - prevTotalDownloadedSize));
		prevTotalDownloadedSize = totalDownloadedSize;

		// calcule the time left using the percentage (more accurate)
		if (usePercentageForTimeLeft)
		{
			float timeRemAvg = timeRemainingAvg->add(currentPercentage - lastPercentage);
			// if the time left avarage is not 0 then
			if (timeRemAvg != 0)
			{
				float timeRem = (100.0 - currentPercentage) / timeRemAvg;
				timeLeft = static_cast<int>(timeRem);
				lastPercentage = currentPercentage;
			}
		}
		// calcule the time remaining using the dowload speed
		else if (downloadSpeed != 0)
		{
			float downSpeedAvg = downloadSpeedAvg->avg();
			// if the time left avarage is not 0 then
			if (downSpeedAvg != 0)
			{
				float timeRem = timeRemainingAvg->add(totalToDownloadSize - totalToDownloadSize) / downSpeedAvg;
				timeLeft = static_cast<int>(timeRem);
			}
		}
	}
}

void Http::restartDownloadSignal()
{
	download(oriURL, destFile.path(), destFile.fileName(), false);
	// increase retries
	retriesCount++;
}

void Http::abortRequest(EnumHTTP::StopReason stopReason)
{
	this->stopReason = stopReason;
	// abort the current network reply
	if (currentReply) currentReply->abort();
}

void Http::closeOutputFile()
{
	if (outputFile)
	{
		if (outputFile->isOpen()) outputFile->close();
		delete outputFile;
		outputFile = NULL;
	}
}

void Http::deleteOutputFile()
{
	if (deleteFileOnError && outputFile && outputFile->exists())
		outputFile->remove();
}

void Http::sendDownloadError(int error)
{
	lastErrorCode = error;
	// delete the downloaded file
	deleteOutputFile();
	// emit the download error signal
	emit downloadError(error);
}

void Http::timeOutDownloadError()
{
	// can give it a second try?
	if (canRestartFailedDownload())
		restartFailedDownload();
	else // no more tries... so send the time out error
		sendDownloadError(EnumHTTP::INTERNAL_NETWEORK_TIME_OUT);
}

EnumHTTP::Error Http::networkReplyToEnumHTTP(QNetworkReply::NetworkError error)
{
	switch (error)
	{
		case QNetworkReply::NoError:
			return EnumHTTP::NO_HTTP_ERROR;
		case QNetworkReply::TimeoutError:
			return EnumHTTP::INTERNAL_NETWEORK_TIME_OUT;
		// connection refused error
		case QNetworkReply::ConnectionRefusedError:
			return EnumHTTP::CONNECTION_REFUSED;
		// host not found error
		case QNetworkReply::HostNotFoundError:
			return EnumHTTP::HOST_NOT_FOUND_ERROR;
		// content access denied error
		case QNetworkReply::ContentAccessDenied:
			return EnumHTTP::CONTENT_ACCESS_DENIED;
		// content not found error
		case QNetworkReply::ContentNotFoundError:
			return EnumHTTP::CONTENT_NOT_FOUND;
		// other errors
		default:
			return EnumHTTP::UNKNOW_NETWEORK_ERROR;
	}
}

void Http::resetRetries()
{
	retriesCount = 1;
}

bool Http::canRestartFailedDownload()
{
	return retriesCount < maxRetries;
}

void Http::restartFailedDownload()
{
	QTimer::singleShot(250, this, SLOT(restartDownloadSignal()));
}

void Http::jumpToURL(QUrl url)
{
	// start a new time out timer
	if (timeOut) startTimeOutTimer();

	// start the init time left timer
	startTimeLeftTimer();

	// check if we supperated the autojumps control
	if (autoJump && autoJumps > maxAutoJumps)
	{
		abortRequest(EnumHTTP::MAX_AUTO_JUMPS_REACHED);
		// quit process
		return;
	}

	// check if the current url has a host (if not, assign the original url host)
	if (url.host().isEmpty())
	{
		QString tmpUrl = url.toString();
		if (!tmpUrl.isEmpty()) if (tmpUrl.at(0) != '/') tmpUrl = "/" + tmpUrl;
		// new url
		url = QUrl("http://" + oriURL.host() + tmpUrl);
	}

	// clear any previous reply
	clearCurrentReply();

	// if cookies are disabled, then remove all previous cookies
	if (!cookiesEnabled) clearCookies();

	// create the new http request
	QNetworkRequest request(url);

	// set user agent
	if (!userAgent.isEmpty())
		request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
	else if (!HTTP_GLOBAL_USER_AGENT.isEmpty()) // set the global user agent
		request.setRawHeader("User-Agent", HTTP_GLOBAL_USER_AGENT.toLocal8Bit());

	// set connection: "keep alive"
	request.setRawHeader("Connection", "Keep-Alive");

	// set Accept-Ranges: "bytes" (to indicate its acceptance of range requests for a resource)
	request.setRawHeader("Accept-Ranges", "bytes");

	// add user custom header parameters
	for (int n = 0; n < customHeaders->count(); n++)
	{
		QString key = getToken(customHeaders->at(n), "|", 0);
		QString value = getToken(customHeaders->at(n), "|", 1);
		// add this new custom header
		request.setRawHeader(key.toLocal8Bit(), value.toLocal8Bit());
	}

	// if we are resuming a download...
	if (resuming && outputFile)
	{
		QString range = QString("bytes=%1-").arg(outputFile->size());
		request.setRawHeader("Range", range.toLocal8Bit());
	}

	// execute the http request
	switch (httpMethod)
	{
		case EnumHTTP::httpGet:
			currentReply = http->get(request);
			break;
		case EnumHTTP::httpPost:
			currentReply = http->post(request, parameters.toLocal8Bit());
			break;
		case EnumHTTP::httpHead:
			currentReply = http->head(request);
			break;
		case EnumHTTP::httpDeleteResource:
			currentReply = http->deleteResource(request);
			break;
	}

	// connect local signals from QNetworkReply
	connect(currentReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(currentReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(currentReply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));

	// post method off
	if (httpMethod == EnumHTTP::httpPost) httpMethod = EnumHTTP::httpGet;

	// inc auto jumps controller
	autoJumps++;
}

int Http::download(const QUrl URL, QString destination, QString fileName, bool autoName)
{
	// clean destination dir
	destination = QDir::cleanPath(destination);

#ifdef Q_OS_WIN32
	// fix Qt4.4.0 bug in windows
	if (destination.indexOf(":/") == -1)
		destination.replace(":", ":/");
#endif

	// check if is already downloading another file
	if (isDownloading())
		return EnumHTTP::ALREADY_DOWNLOADING;

	// check if is a valid URL
	if (!validURL(URL.toString()))
		return EnumHTTP::INVALID_URL;

	// create the destination path, if it don't exists
	if (!QDir(destination).exists())
		if (!QDir(destination).mkpath(destination))
			return EnumHTTP::UNABLE_CREATE_DIR;

	// set a default name (only if it is empty)
	if (fileName.isEmpty())
		fileName = "download.file";

	fileName = QFileInfo(fileName).fileName();

	// set destination file name
	fileName = cleanFileName(fileName);

	// get an unique file name for this download
	if (autoName)
		fileName = uniqueFileName(destination + "/" + fileName);
	else
		fileName = destination + "/" + fileName;

	// create file
	outputFile = new QFile(fileName);
	if (!outputFile->open(QIODevice::WriteOnly))
	{
		closeOutputFile();
		return EnumHTTP::UNABLE_CREATE_FILE;
	}

	// set file info
	destFile = QFileInfo(fileName);

	// if skip existent files is enabled and the output file already exists, then...
	if (QFile::exists(fileName) && skipExistentFiles)
	{
		emit downloadFinished(destFile);
		return QNetworkReply::NoError;
	}

	// init http variables
	initData();

	// start the download process
	oriURL = URL;
	jumpToURL(URL);

	// ok
	return EnumHTTP::NO_HTTP_ERROR;
}

int Http::resume(const QUrl URL, QString fileName, bool autoRestartOnFail)
{
	// check if is already downloading another file
	if (isDownloading())
		return EnumHTTP::ALREADY_DOWNLOADING;

	// check if is a valid URL
	if (!validURL(URL.toString()))
		return EnumHTTP::INVALID_URL;

	if (!QFile::exists(fileName))
		return EnumHTTP::MISSING_RESUME_FILE;

	// open the existent file in append mode
	outputFile = new QFile(fileName);
	if (!outputFile->open(QIODevice::Append))
	{
		closeOutputFile();
		return EnumHTTP::UNABLE_APPEND_FILE;
	}

	// set file info
	destFile = QFileInfo(fileName);

	// init http variables
	initData();

	// configure the resuming
	resuming = true;
	this->autoRestartOnFail = autoRestartOnFail;
	initialDownloadSize = outputFile->size();
	prevTotalDownloadedSize = initialDownloadSize;

	// start the download process
	oriURL = URL;
	jumpToURL(URL);

	// ok
	return EnumHTTP::NO_HTTP_ERROR;
}

void Http::pause()
{
	if (isDownloading()) abortRequest(EnumHTTP::USER_PAUSED);
}

void Http::cancel()
{
	if (isDownloading()) abortRequest(EnumHTTP::USER_CANCELLED);
}

QString Http::syncRequest(EnumHTTP::HttpMethod httpMethod, QUrl url, QString parameters, bool isUtf8)
{
	QString result = QString();
	// if is not downloading and has a valid url
	if (!isDownloading() && url.isValid())
	{
		// init http variables
		initData();
		// set the sync flag active
		syncFlag = true;
		this->httpMethod = httpMethod;
		// set parameters
		this->parameters = parameters;
		// do the first jump
		oriURL = url;
		jumpToURL(url);
		// wait while the webpage is being downloaded
		while (syncFlag) qApp->processEvents();
		// if is utf8 then convert the downloaded data to utf8 (else, return data as is)
		result = isUtf8 ? QString::fromUtf8(data.toLocal8Bit()) : data;
	}
	// final result (output)
	return result;
}

QString Http::downloadWebpage(const QUrl URL, bool isUtf8)
{
	return syncRequest(EnumHTTP::httpGet, URL, QString(), isUtf8);
}

QString Http::downloadWebpagePost(const QUrl URL, QString parameters, bool isUtf8)
{
	return syncRequest(EnumHTTP::httpPost, URL, parameters, isUtf8);
}

QString Http::head(const QUrl URL)
{
	return syncRequest(EnumHTTP::httpHead, URL, QString(), false);
}

void Http::addCookie(QString cookie)
{
	return static_cast<Cookies*>(http->cookieJar())->addCookie(cookie);
}

void Http::addCookies(QString cookies, const QString separator)
{
	return static_cast<Cookies*>(http->cookieJar())->addCookies(cookies, separator);
}

QString Http::getCookies(const QString separator)
{
	return static_cast<Cookies*>(http->cookieJar())->getCookies(separator);
}

void Http::clearCookies()
{
	return static_cast<Cookies*>(http->cookieJar())->clearCookies();
}

void Http::addHeaderParameter(QString key, QString value)
{
	customHeaders->append(key + "|" + value);
}

void Http::addHeaderParameters(QString headers, const QString separator)
{
	QStringList headersList = headers.split(separator);
	// add all headers
	foreach(QString header, headersList)
	{
		QString key = copy(header, 0, header.indexOf("="));
		QString value = header.remove(0, header.indexOf("=") + 1);
		addHeaderParameter(key, value);
	}
}

void Http::clearHeaderParameters()
{
	customHeaders->clear();
}

bool Http::isDownloading()
{
	return currentReply && currentReply->isRunning();
}

int Http::getFileSize()
{
	return totalToDownloadSize;
}

int Http::getDownloadSpeed()
{
	return downloadSpeed;
}

int Http::getTimeRemaining()
{
	return timeLeft;
}

QFileInfo Http::getDestiationFile()
{
	return destFile;
}

int Http::getLastError()
{
	return lastErrorCode;
}

int Http::getLastStopReason()
{
	return stopReason;
}

void Http::finished(QNetworkReply *reply)
{
	// if this reply is not the current reply we ignore this
	if (currentReply != reply) return;

	// stop all active timers
	stopActiveTimeOutTimer();
	stopTimeLeftTimer();

	// get the possible redirection url
	QUrl redirectURL = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	// check if is a valid redirection
	if (autoJump && !redirectURL.isEmpty())
	{
		// clear previous downloaded data
		if (syncFlag) data = QString(); else outputFile->reset();
		// jump to this new url
		jumpToURL(redirectURL);
	}
	else // other finishes (error, ok, etc...)
	{
		if (!syncFlag) // async. request
		{
			// check possible errors
			switch (reply->error())
			{
				case QNetworkReply::NoError:
				{
					// reset retries count
					resetRetries();
					// if we have a size reference then check if we download all
					if (totalToDownloadSize != 0 && outputFile->size() < totalDownloadedSize)
					{
						sendDownloadError(EnumHTTP::INVALID_INITIAL_FILE_SIZE);
					}
					else // ok, we assume which this a valid download
					{
						stopReason = EnumHTTP::DOWNLOAD_FINISHED;
						// send the download finished signal
						emit downloadFinished(destFile);
					}
					break;
				}
				// user did cancel or pause
				case QNetworkReply::OperationCanceledError:
				{
					switch (stopReason)
					{
						case EnumHTTP::USER_CANCELLED:
						{
							// remove the temporal downloaded file
							deleteOutputFile();
							// send the canceled signal
							emit downloadCanceled();
							break;
						}
						case EnumHTTP::USER_PAUSED:
						{
							emit downloadPaused(destFile);
							break;
						}
						case EnumHTTP::TIME_OUT:
						{
							timeOutDownloadError();
							break;
						}
						case EnumHTTP::MAX_AUTO_JUMPS_REACHED:
						{
							sendDownloadError(EnumHTTP::TOO_MUCH_REDIRECTIONS);
							break;
						}
						case EnumHTTP::AUTO_RESTART:
						{
							restartFailedDownload();
							break;
						}
						case EnumHTTP::CANNOT_RESUME:
						{
							sendDownloadError(EnumHTTP::UNABLE_RESUME_DOWNLOAD);
							break;
						}
						// ignore others
						default: break;
					}
					break;
				}
				// connection reached a time out error
				case QNetworkReply::TimeoutError:
				{
					timeOutDownloadError();
					break;
				}
				// other errors
				default:
				{
					// check if we can give it an another try...
					if (canRestartFailedDownload())
						restartFailedDownload();
					else // no more tries... so finish with an error
						sendDownloadError(networkReplyToEnumHTTP(reply->error()));
					break;
				}
			}
			// destroy output file
			closeOutputFile();
		}
		else // sync. request
		{
			// get the latest error
			lastErrorCode = networkReplyToEnumHTTP(reply->error());
			// get the response headers (HEAD only)
			if (httpMethod == EnumHTTP::httpHead)
			{
				data = QString();
				// add each rawHeader into our data variable
				foreach (QByteArray rawHeader, reply->rawHeaderList())
					data += QString("%1: %2\n").arg(QString(rawHeader)).arg(QString(reply->rawHeader(rawHeader)));
			}
			else // get the downloaded data (GET and POST)
				data = reply->error() == QNetworkReply::NoError ? data : QString();
			// we finished...
			syncFlag = false;
			// emit the download finished signal
			emit downloadFinished(QFileInfo());
		}
	}
}

void Http::readyRead()
{
	// update timeout controller
	if (timeOut) startTimeOutTimer();
	// write data
	if (syncFlag) // sync. method (save to QString)
		data += currentReply->readAll();
	else // async. method (save to file)
		outputFile->write(currentReply->readAll());
}

void Http::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	// avoid fake downloads progress
	if (!downloadStartedFlag) return;

	// if we have a missing "total"... we assume which received all the data
	if (bytesTotal == -1) bytesTotal = bytesReceived;

	// update totals
	totalToDownloadSize = totalToDownloadSize != 0 ? totalToDownloadSize : bytesTotal;
	totalDownloadedSize = totalToDownloadSize != 0 ? initialDownloadSize + bytesReceived : 0;

	// update the download percentage
	if (totalToDownloadSize != 0)
	{
		currentPercentage = static_cast<float>(totalDownloadedSize) / static_cast<float>(totalToDownloadSize) * 100.0;
		// if the current percentage is not 0 and the last yes, we reajust it (for better calculation)
		if (currentPercentage != 0.0 && lastPercentage == 0.0) lastPercentage = currentPercentage;
	}

	// sent the download event signal
	emit downloadEvent(totalDownloadedSize, totalToDownloadSize);
}

void Http::metaDataChanged()
{
	// if we are redirecting... then ignore any "meta data change" for this time
	if (!currentReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull())
		return;

	// get the file size from content-length header (if exists)
	QVariant contentLength = currentReply->header(QNetworkRequest::ContentLengthHeader);
	if (!contentLength.isNull()) totalToDownloadSize = contentLength.toInt();

	// if we are resuming then get the total to download size
	if (resuming)
	{
		if (currentReply->hasRawHeader("content-range"))
			totalToDownloadSize = getToken(currentReply->rawHeader("content-range"), "/", 1).toInt();
		else if (autoRestartOnFail) // auto-restart download
			abortRequest(EnumHTTP::AUTO_RESTART);
		else // no auto-restart
			abortRequest(EnumHTTP::CANNOT_RESUME);
	}

	// no error found and no location header found
	if (currentReply->error() == QNetworkReply::NoError)
	{
		// send the download/resume signal
		if (resuming) emit downloadResumed(); else emit downloadStarted();
		// ok, real now we can say that the download started...
		downloadStartedFlag = true;
	}
}

void Http::setUserAgent(QString value)
{
	userAgent = value;
}

void Http::setGlobalUserAgent(QString value)
{
	HTTP_GLOBAL_USER_AGENT = value;
}

void Http::setAutoJumps(bool value)
{
	autoJump = value;
}

void Http::setMaxAutoJumps(int value)
{
	maxAutoJumps = value > 0 ? value : 1;
}

void Http::setTimeOutOption(bool value)
{
	timeOut = value;
}

void Http::setTimeOut(int value)
{
	value = value < 1 ? 1 : value;
	// update the time out
	timeOutSeconds = value * 1000;
}

void Http::setMaxRetries(int value)
{
	maxRetries = value > 0 ? value : 1;
}

void Http::setSkipExistentFiles(bool value)
{
	skipExistentFiles = value;
}

void Http::setPauseOnDestroy(bool value)
{
	this->pauseOnDestroyF = value;
}

void Http::setUsePercentageForTimeRemaining(bool value)
{
	usePercentageForTimeLeft = value;
}

void Http::setCookiesEnabled(bool value)
{
	cookiesEnabled = value;
}

void Http::setDeleteFileOnError(bool value)
{
	deleteFileOnError = value;
}

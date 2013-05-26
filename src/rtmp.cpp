/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2012 Xesc & Technology
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

#include "rtmp.h"

#include "http.h"
#include "tools.h"

#ifndef Q_OS_WIN32
#include <signal.h>
#endif

RTMP::RTMP(QString flvstreamerPath, QString workingDir)
{
	setObjectName("RTMP");
	// init internal vars
	init();
	// set up the flvstream app path
	this->flvstreamerPath = flvstreamerPath + "/" + RTMP_APP_PATH;
	// create speed avarage
	downloadSpeedAvg = new ArrayAvg(100);
	timeRemainingAvg = new ArrayAvg(100);
	// create a new qprocess
	rtmpProcess = new QProcess();
	// connect signals
	connect(rtmpProcess, SIGNAL(started()), this, SLOT(started()));
	connect(rtmpProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
	connect(rtmpProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
	connect(rtmpProcess, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
	// cofnigure process
	rtmpProcess->setWorkingDirectory(workingDir);
}

RTMP::~RTMP()
{
	if (pauseOnDestroyF) pause(); else cancel();

	delete timeRemainingAvg;
	delete downloadSpeedAvg;
	delete rtmpProcess;
}

void RTMP::init()
{
	stopReason = EnumRTMP::NO_STOPPED;
	resuming = false;
	errorCode = 0;
	fileSize = 0;
	currentDownloadedSize = 0;
	lastDownloadedSize = 0;
	downloadSpeed = 0;
	timeRemaining = 0;
	usingPercentage = false;
	currentPercentage = 0.0;
	lastPercentage = 0.0;
}

bool RTMP::isFlvstreamerInstalled()
{
	return QFile::exists(flvstreamerPath);
}

int RTMP::download(const QString URL, QString destination, QString fileName, QStringList params, bool autoName)
{
	if (!isFlvstreamerInstalled())
		return EnumRTMP::FLVSTREAMER_MISSING;

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

	// set file info
	destinationFile = QFileInfo(fileName);

	// set as downloading
	resuming = false;

	// build the command line
	QStringList commandLine = QStringList() << "-r" << URL << "-o" << fileName;
	foreach (QString value, params)
	{
		commandLine << value.split("=", QString::SkipEmptyParts);
	}

	// start download
	rtmpProcess->start(flvstreamerPath, commandLine);

	// ok
	return EnumRTMP::NO_RTMP_ERROR;
}

int RTMP::resume(const QString URL, QString fileName)
{
	if (!isFlvstreamerInstalled())
		return EnumRTMP::FLVSTREAMER_MISSING;

	// check if is already downloading another file
	if (isDownloading())
		return EnumHTTP::ALREADY_DOWNLOADING;

	if (!QFile::exists(fileName))
		return EnumHTTP::MISSING_RESUME_FILE;

	// set file info
	destinationFile = QFileInfo(fileName);

	// set as resuming
	resuming = true;

	// resume download
	rtmpProcess->start(flvstreamerPath, QStringList() << "-r" << URL << "-e" << "-o" << fileName);

	// ok
	return EnumRTMP::NO_RTMP_ERROR;
}

void RTMP::pause()
{
	if (isDownloading())
	{
		stopReason = EnumRTMP::USER_PAUSED;
#ifndef Q_OS_WIN32
		// kill the flvstreaming
		kill(rtmpProcess->pid(), SIGINT);
#endif
	}
}

void RTMP::cancel()
{
	if (isDownloading())
	{
		stopReason = EnumRTMP::USER_CANCELLED;
		// kill the flvstreaming
		rtmpProcess->kill();
		// remove file from disc
		QFile::remove(destinationFile.absoluteFilePath());
	}
}

void RTMP::pauseOnDestroy(bool pauseOnDestroyF)
{
	this->pauseOnDestroyF = pauseOnDestroyF;
}

bool RTMP::isDownloading()
{
	return rtmpProcess->state() != QProcess::NotRunning;
}

int RTMP::getFileSize()
{
	return fileSize;
}

int RTMP::getDownloadSpeed()
{
	return downloadSpeed;
}

int RTMP::getTimeRemaining()
{
	return timeRemaining;
}

QFileInfo RTMP::getDestinationFile()
{
	return destinationFile;
}

void RTMP::started()
{
	init();
	// start timer
	singleTimerShot();
	// send download started signal
	if (resuming)
		emit downloadResumed();
	else // not resuming (starting)
		emit downloadStarted();
}

void RTMP::finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	// user cancelled
	if (stopReason == EnumRTMP::USER_CANCELLED)
		emit downloadCanceled();
	// user paused
	else if (stopReason == EnumRTMP::USER_PAUSED)
		emit downloadPaused(destinationFile);
	// download error
	else if (stopReason == EnumRTMP::DOWNLOAD_ERROR)
		emit downloadError(errorCode);
	// download finished
	else if (stopReason == EnumRTMP::NO_STOPPED)
		emit downloadFinished(destinationFile);
}

void RTMP::readyReadStandardOutput()
{
	parseOutput(QString::fromLocal8Bit(rtmpProcess->readAllStandardOutput()));
}

void RTMP::readyReadStandardError()
{
	parseOutput(QString::fromLocal8Bit(rtmpProcess->readAllStandardError()));
}

void RTMP::parseOutput(QString output)
{
	output = output.trimmed();
	// split output into lines
	QStringList lines = output.split("\n");
	// parse each line
	foreach(QString line, lines)
	{
		QStringList tokens = line.split(" ", QString::SkipEmptyParts);
		// parse tokens
		if (!tokens.isEmpty())
		{
			// get fileSize
			if (tokens.at(0).trimmed() == "filesize")
				fileSize = tokens.at(1).trimmed().toInt(NULL, 10);
			// get the starting size
			else if (tokens.at(0).trimmed() == "S:")
				lastDownloadedSize = tokens.at(1).trimmed().toInt(NULL, 10);
			// get error and send error signal
			else if ((tokens.at(0).trimmed() == "E:" || tokens.at(0).trimmed() == "ERROR:") && stopReason == EnumRTMP::NO_STOPPED)
			{
				stopReason = EnumRTMP::DOWNLOAD_ERROR;
				// send error signal
				errorCode = tokens.at(1).trimmed().toInt(NULL, 10);
			}
			// get download event
			else if (tokens.at(0).trimmed() == "D:")
			{
				// get the current downloaded data
				currentDownloadedSize = tokens.at(1).trimmed().toInt(NULL, 10);
				// if fileSize is avaialbe, use the downloaded data as %
				if (fileSize != 0)
				{
					usingPercentage = false;
					// send the download event
					emit downloadEvent(currentDownloadedSize, fileSize);
				}
				else if (tokens.count() == 4) // use the % instead of downloaded data
				{
					usingPercentage = true;
					currentPercentage = tokens.at(3).trimmed().toFloat()/1000 * 100;
					// send the download event
					emit downloadEvent(tokens.at(3).trimmed().toInt(NULL, 10), 1000);
				}
			}
		}
	}
	// small win32 hack due to problems with: GenerateConsoleCtrlEvent(CTRL_C_EVENT, flvstreamerProcess->pid()->hProcess);
#ifdef Q_OS_WIN32
	// user canceled?
	if (stopReason == EnumRTMP::USER_PAUSED)
		flvstreamerProcess->write("1\n"); // cancel download
	else // continue...
		flvstreamerProcess->write("0\n"); // continue download
#endif
}

void RTMP::singleTimerShot()
{
	QTimer::singleShot(1000, this, SLOT(timerEvent()));
}

void RTMP::timerEvent()
{
	// calcule download speed
	downloadSpeed = static_cast<int>(downloadSpeedAvg->add(currentDownloadedSize - lastDownloadedSize));

	// time remaining
	if (downloadSpeed != 0 && !usingPercentage) // using dowload speed
	{
		float timeRem = timeRemainingAvg->add(fileSize - currentDownloadedSize) / downloadSpeedAvg->avg();
		timeRemaining = static_cast<int>(timeRem);
	}
	else if (usingPercentage) // using downloaded percentage
	{
		float timeRem = (100 - currentPercentage)/timeRemainingAvg->add(currentPercentage - lastPercentage);
		timeRemaining = static_cast<int>(timeRem);
		lastPercentage = currentPercentage;
	}

	// update counters
	lastDownloadedSize = currentDownloadedSize;
	// next timer shot
	if (isDownloading()) singleTimerShot();
}

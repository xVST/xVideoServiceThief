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

#include "youtubedl.h"

#include "http.h"
#include "tools.h"

#ifndef Q_OS_WIN32
#include <signal.h>
#endif

YoutubeDL::YoutubeDL(QString appPath, QString workingDir)
{
	setObjectName("YoutubeDL");
	// init internal vars
	init();
    // set up the RTMP app path
    this->appPath = appPath + "/" + YOUTUBE_DL_APP_PATH;
	// create a new qprocess
	process = new QProcess();
	// connect signals
	connect(process, SIGNAL(started()), this, SLOT(started()));
	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
	connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
	connect(process, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
	// cofnigure process
	process->setWorkingDirectory(workingDir);
}

YoutubeDL::~YoutubeDL()
{
    if (process->isOpen())
    {
        process->kill();
    }
    // cleanup...
    delete process;
}

void YoutubeDL::init()
{
	stopReason = EnumYoutubeDL::NO_STOPPED;
	resuming = false;
	errorCode = 0;
	fileSize = 0;
	downloadSpeed = 0;
	timeRemaining = 0;
}

bool YoutubeDL::isYoutubeDLappInstalled()
{
	return QFile::exists(appPath);
}

int YoutubeDL::download(const QString URL, QString destination, QString fileName, QStringList params, bool autoName)
{
	if ( ! isYoutubeDLappInstalled())
		return EnumYoutubeDL::YOUTUBE_DL_APP_MISSING;

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
	if ( ! QDir(destination).exists())
		if ( ! QDir(destination).mkpath(destination))
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
	QStringList commandLine = QStringList() << "-o" << fileName << URL;
	foreach (QString value, params)
	{
		commandLine << value.split("=", QString::SkipEmptyParts);
	}

	// start download
	process->start(appPath, commandLine);

	// ok
	return EnumYoutubeDL::NO_YOUTUBE_DL_ERROR;
}

int YoutubeDL::resume(const QString URL, QString fileName)
{
	if ( ! isYoutubeDLappInstalled())
		return EnumYoutubeDL::YOUTUBE_DL_APP_MISSING;

	// check if is already downloading another file
	if (isDownloading())
		return EnumHTTP::ALREADY_DOWNLOADING;

	if ( ! QFile::exists(fileName + ".part"))
		return EnumHTTP::MISSING_RESUME_FILE;

	// set file info
	destinationFile = QFileInfo(fileName);

	// set as resuming
	resuming = true;

	// resume download
	process->start(appPath, QStringList() << "-o" << fileName << URL);

	// ok
	return EnumYoutubeDL::NO_YOUTUBE_DL_ERROR;
}

void YoutubeDL::pause()
{
	if (isDownloading())
	{
		stopReason = EnumYoutubeDL::USER_PAUSED;
		// kill the rtmp process
		process->kill();
	}
}

void YoutubeDL::cancel()
{
	if (isDownloading())
	{
		stopReason = EnumYoutubeDL::USER_CANCELLED;
		// kill the rtmp process
		process->kill();
		// remove file from disc
		QFile::remove(destinationFile.absoluteFilePath() + ".part");
	}
}

void YoutubeDL::pauseOnDestroy(bool pauseOnDestroyF)
{
	this->pauseOnDestroyF = pauseOnDestroyF;
}

bool YoutubeDL::isDownloading()
{
	return process->state() != QProcess::NotRunning;
}

int YoutubeDL::getFileSize()
{
	return fileSize;
}

int YoutubeDL::getDownloadSpeed()
{
	return downloadSpeed;
}

int YoutubeDL::getTimeRemaining()
{
	return timeRemaining;
}

QFileInfo YoutubeDL::getDestinationFile()
{
	return destinationFile;
}

QJsonDocument YoutubeDL::getVideoInformation(const QString URL)
{
    QStringList arguments = QStringList() << "--flat-playlist" << "--dump-single-json" << URL;
    // execute command
    QProcess process;
    process.start(appPath, arguments);
    process.waitForFinished();
    // read the standard output
    QString standardOutput(process.readAllStandardOutput());
    // is the standard output empty? then read the error output
    if (standardOutput.isEmpty())
    {
        QJsonObject error;
        error.insert("error", QJsonValue::fromVariant(process.readAllStandardError()));
        // create document
        QJsonDocument json;
        json.setObject(error);
        // the error json
        return json;
    }
    // parse the json
    return QJsonDocument::fromJson(standardOutput.toUtf8());
}

void YoutubeDL::started()
{
	init();
	// start timer
//	singleTimerShot();
	// send download started signal
	if (resuming)
		emit downloadResumed();
	else // not resuming (starting)
		emit downloadStarted();
}

void YoutubeDL::finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	// user cancelled
	if (stopReason == EnumYoutubeDL::USER_CANCELLED)
		emit downloadCanceled();
	// user paused
	else if (stopReason == EnumYoutubeDL::USER_PAUSED)
		emit downloadPaused(destinationFile);
	// download error
	else if (stopReason == EnumYoutubeDL::DOWNLOAD_ERROR)
		emit downloadError(errorCode);
	// download finished
	else if (stopReason == EnumYoutubeDL::NO_STOPPED)
		emit downloadFinished(destinationFile);
}

void YoutubeDL::readyReadStandardOutput()
{
	parseOutput(QString::fromLocal8Bit(process->readAllStandardOutput()));
}

void YoutubeDL::readyReadStandardError()
{
	parseOutput(QString::fromLocal8Bit(process->readAllStandardError()));
}

void YoutubeDL::parseOutput(QString output)
{
	output = output.trimmed();
	// split output into lines
	QStringList lines = output.replace("\r", "\n").split("\n");
	// parse each line
	foreach(QString line, lines)
	{
		QRegExp downloadRegExp("\\[download\\]\\s+(.+)%\\s+of\\s+(.+)\\s+at\\s+(.+)\\s+ETA\\s+(.+)");
		// parese information
		if (downloadRegExp.indexIn(line) > -1)
		{
			// get file size
			fileSize = fileSizeToBytes(downloadRegExp.capturedTexts().at(2));
			// parse the download speed
			if (downloadRegExp.capturedTexts().at(3) != "Unknown speed")
			{
				QString speed = downloadRegExp.capturedTexts().at(3);
				// convert download speed to bytes
				downloadSpeed = fileSizeToBytes(speed.replace("/s", ""));
			}
			// parse the ETA time
			if (downloadRegExp.capturedTexts().at(4) != "Unknown speed")
			{
				QString dhms = downloadRegExp.capturedTexts().at(4);
				QStringList parts = dhms.split(":");
				// parse inits
				int index = parts.count() - 1;
				timeRemaining = 0;
				// calcule the remaining time
				foreach (QString value, parts)
				{
					timeRemaining += value.toInt()*pow(60, index);
					index--;
				}
			}
			// calcule downlaod progress
			QString progress = downloadRegExp.capturedTexts().at(1);
			// send the download event
			emit downloadEvent(progress.toFloat()*100, 10000);
		}

        QRegExp errorRegExp("ERROR:\\s*(.*)");
		// parse error
		if (errorRegExp.indexIn(line) > -1)
		{
            errorMessage = downloadRegExp.capturedTexts().at(1);
            qDebug() << errorMessage;
			stopReason = EnumYoutubeDL::DOWNLOAD_ERROR;
		}
	}
}

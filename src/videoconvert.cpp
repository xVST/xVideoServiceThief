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

#include "videoconvert.h"

#include "tools.h"
#include "videoitem.h"

VideoConverter::VideoConverter(QString ffmpegApp, QString workingDir,
                               VideoConversionConfig convConf, bool deleteOriginalVideo)
{
	setObjectName("VideoConverter");
	// set vars
	setFFmpegApp(ffmpegApp);
	setWorkingDir(workingDir);
	setConversionConfig(convConf);
	setDeleteOriginalVideo(deleteOriginalVideo);
	//
	internalTimer = 0;
	// create a new qprocess
	ffmpegProcess = new QProcess();
	// connect signals
	connect(ffmpegProcess, SIGNAL(started()), this, SLOT(started()));
	connect(ffmpegProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
	connect(ffmpegProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadStandardOutput()));
	connect(ffmpegProcess, SIGNAL(readyReadStandardError()), this, SLOT(readyReadStandardError()));
	// cofnigure process
	ffmpegProcess->setWorkingDirectory(workingDir);
}

VideoConverter::~VideoConverter()
{
	delete ffmpegProcess;
}

QStringList VideoConverter::getCommandLine()
{
	QStringList parameters;
	QString extension;

	parameters << "-i" << validPath(videoItem->getVideoFile());

	switch (convConfToUse.outputFormat)
	{
		case ofAVI:
			parameters << "-f" << "avi" << "-vcodec" << "mpeg4";
			extension = ".avi";
			break;
		case ofWMV:
			extension = ".wmv";
			break;
		case ofMPEG1:
			parameters << "-vcodec" << "mpeg1video";
			extension = ".mpg";
			break;
		case ofMPEG2:
			parameters << "-vcodec" << "mpeg2video";
			extension = ".mpeg";
			break;
		case ofMP4:
		case ofMP4_hd:
		{
			QString vpre = convConfToUse.outputFormat == ofMP4 ? "default" : "hq";
			// configure h264 codec
#ifndef Q_OS_LINUX
	#ifdef Q_OS_WIN32
			bool use_x264 = true;
	#else
			bool use_x264 = isRunningOn64bits();
	#endif
			if (use_x264)
				parameters	<< "-vcodec" << "libx264"
							<< "-fpre" << QFileInfo(ffmpegApp).absolutePath() + "/ffmpeg-presets/libx264-" + vpre + ".ffpreset";
			else // no x264 support
				parameters << "-f" << "mp4" << "-vcodec" << "mpeg4";
	#else
			parameters << "-vcodec" << "libx264" << "-vpre" << vpre;
#endif
			extension = ".mp4";
			break;
		}
		case ofAppleiPod:
			parameters	<< "-acodec" << "libfaac" << "-ab" << "128kb" << "-vcodec" << "mpeg4" << "-b" << "1200kb"
						<< "-mbd" << "2" << "-cmp" << "2" << "-subcmp" << "2" << "-s" << "320x240";
			extension = ".mp4";
			break;
		case ofSonyPSP:
			parameters << "-acodec" << "libfaac" << "-ab" << "128kb" <<  "-vcodec" << "mpeg4" << "-b" << "1200kb"
					   << "-mbd" << "2" << "-flags" << "+4mv" << "-trellis" << "2" << "-cmp" << "2" << "-subcmp" << "2"
					   << "-r" << "30000/1001" << "-f" << "psp";
			extension = ".mp4";
			break;
		case of3GP:
			parameters << "-vcodec" << "mpeg4" << "-acodec" << "libfaac" << "-ar" << "8000" << "-ac" << "1"
					   << "-ab" << "12.2k" << "-f" << "3gp";
			extension = ".3gp";
			break;
		case ofMP3:
			extension = ".mp3";
			break;
	}

	// this is used only for NON Apple iPod videos
	if (convConfToUse.outputFormat != ofAppleiPod)
	{
		switch (convConfToUse.videoResolution)
		{
			case vrOriginal:
				break;
			case vr96x72:
				parameters << "-s" << "96x72";
				break;
			case vr128x96:
				parameters << "-s" << "128x96";
				break;
			case vr160x120:
				parameters << "-s" << "160x120";
				break;
			case vr176x120:
				parameters << "-s" << "176x120";
				break;
			case vr176x144:
				parameters << "-s" << "176x144";
				break;
			case vr192x144:
				parameters << "-s" << "192x144";
				break;
			case vr240x180:
				parameters << "-s" << "240x180";
				break;
			case vr320x200:
				parameters << "-s" << "320x200";
				break;
			case vr320x240:
				parameters << "-s" << "320x240";
				break;
			case vr352x240:
				parameters << "-s" << "352x240";
				break;
			case vr352x288:
				parameters << "-s" << "352x288";
				break;
			case vr480x272:
				parameters << "-s" << "480x272";
				break;
			case vr480x360:
				parameters << "-s" << "480x360";
				break;
			case vr480x480:
				parameters << "-s" << "480x480";
				break;
			case vr624x352:
				parameters << "-s" << "624x352";
				break;
			case vr640x480:
				parameters << "-s" << "640x480";
				break;
			case vr720x480:
				parameters << "-s" << "720x480";
				break;
			case vr720x576:
				parameters << "-s" << "720x576";
				break;
		}

		if (convConfToUse.videoFrameRate == vfr15)
			parameters << "-r" << "15";
		else if (convConfToUse.videoFrameRate == vfr24)
			parameters << "-r" << "24";
		else if (convConfToUse.videoFrameRate == vfr25)
			parameters << "-r" << "25";
		else if (convConfToUse.videoFrameRate == vfr29_97)
			parameters << "-r" << "29.97";
		else if (convConfToUse.videoFrameRate == vfr30)
			parameters << "-r" << "30";

		switch (convConfToUse.audioSampleRatio)
		{
			case asrOriginal:
				break;
			case asr22050:
				parameters << "-ar" << "22050";
				break;
			case asr44100:
				parameters << "-ar" << "44100";
				break;
		}

		switch (convConfToUse.outputQuality)
		{
			case oqLower_quality:
				parameters << "-b" << "384k" << "-ab" << "64k";
				break;
			case oqLow_quality:
				parameters << "-b" << "512k" << "-ab" << "80k";
				break;
			case oqNormal_quality:
				parameters << "-b" << "640k" << "-ab" << "96k";
				break;
			case oqMedium_quality:
				parameters << "-b" << "800k" << "-ab" << "96k";
				break;
			case oqGood_quality:
				parameters << "-b" << "1000k" << "-ab" << "128k";
				break;
			case oqSuperb_quality:
				parameters << "-b" << "1200k" << "-ab" << "128k";
				break;
			case oqSame_quality:
				break;
		}
	}

	videoItem->setVideoFileSavedTo(uniqueFileName(changeFileExt(videoItem->getVideoFile(), extension)), this);

	parameters << "-y" << validPath(videoItem->getVideoFileSavedTo());

	return parameters;
}

float VideoConverter::hms2seconds(QString strHMS)
{
	float seconds = 0;
	// extrat h-m-s
	QStringList hms = strHMS.split(":");
	// get hours
	seconds = hms.at(0).toFloat() * 3600;
	// get minutes
	seconds = seconds + hms.at(1).toFloat() * 60;
	// get seconds
	seconds = seconds + hms.at(2).toFloat();
	// return the conversion
	return seconds;
}

void VideoConverter::setVideoDuration(QString strVideoDuration)
{
	if (strVideoDuration == "N/A" || strVideoDuration.isEmpty()) return;
	// get the video length
	videoLength = this->hms2seconds(strVideoDuration);
}

void VideoConverter::getCurrentTimeConversion(QString strFrame)
{
	if (strFrame.isEmpty()) return;

	// get the current progress
	float time = this->hms2seconds(strFrame);

	videoItem->setProgress(calculePercent(time, videoLength), this);

	emit videoItemUpdated(videoItem);
}

void VideoConverter::parseOutput(QString output)
{
	if (videoLength == 0)
	{
		outputAll += output;

		if (outputAll.indexOf("Duration: ") > -1)
		{
			setVideoDuration(copyBetween(outputAll, "Duration: ", ", start"));

			// we have the durtion? if yes, then capture the first step
			if (videoLength != 0)
			{
				getCurrentTimeConversion(copyBetween(outputAll, "time=", " bitrate"));
			}
		}
	}
	else // get the current frame
	{
		getCurrentTimeConversion(copyBetween(output, "time=", " bitrate"));
	}
}

void VideoConverter::initTimer()
{
	// start internal timer
	if (internalTimer != 0)
	{
		this->killTimer(internalTimer);
	}

	internalTimer = this->startTimer(1000);
}

void VideoConverter::deinitTimer()
{
	if (internalTimer != 0)
	{
		// finish timer
		this->killTimer(internalTimer);
		internalTimer = 0;
	}
}

void VideoConverter::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == internalTimer)
	{
		float step = videoItem->getProgress() - lastProgress;
		videoItem->setTimeRemaining(static_cast<int>((100 - videoItem->getProgress()) / step), this);
		lastProgress = videoItem->getProgress();
	}
}

void VideoConverter::startConversionVideo(VideoItem *videoItem)
{
	if (videoItem == NULL || !canStartConversion() || !ffmpegInstalled()) return;
	// check if this videoItem has an overrided conversion config
	if (videoItem->hasOverridedConversion())
	{
		// the user don't want convert it
		if (!videoItem->getOverridedVideoConversionConfig().convertVideo)
		{
			// mark as "converted"
			videoItem->setAsConverted(NULL);
			// emit the conversion finished
			emit conversionFinished(videoItem);
			return;
		}
		else // set the "overrided" conversion config
		{
			convConfToUse = videoItem->getOverridedVideoConversionConfig().videoConversionConfig;
		}
	}
	else // set the conversion config stored in options (standard user options)
	{
		convConfToUse = convConf;
	}
	// assign data
	this->videoItem = videoItem;
	videoItem->lock(this);
	videoItem->setProgress(0, this);
	// init internal data
	videoLength = 0;
	outputAll = "";
	// check if the "input" video file exists
	if (QFile::exists(videoItem->getVideoFile()))
	{
		QFile input(videoItem->getVideoFile());
		// the video is > than 0kb? if yes, then start the conversion
		if (input.size() > 0)
		{
			lastProgress = 0;
			// launch the ffmpeg application
			ffmpegProcess->start(ffmpegApp, getCommandLine());
			return;
		}
	}
	// if we are here, then this video is invalid (error)
	videoItem->setAsError(this);
	videoItem->unlock(this);
	emit videoItemUpdated(videoItem);
}

void VideoConverter::cancelConversion()
{
	if (isConverting())
	{
		ffmpegProcess->kill();
		ffmpegProcess->waitForFinished();
	}
}

bool VideoConverter::canStartConversion()
{
	return !isConverting();
}

bool VideoConverter::isConverting()
{
	return ffmpegProcess->state() == QProcess::Running;
}

bool VideoConverter::ffmpegInstalled()
{
	return QFile::exists(ffmpegApp);
}

QString VideoConverter::getFFmpegApp()
{
	return ffmpegApp;
}

QString VideoConverter::getWorkingDir()
{
	return workingDir;
}

VideoConversionConfig VideoConverter::getConversionConfig()
{
	return convConf;
}

bool VideoConverter::getDeleteOriginalVideo()
{
	return deleteOriginalVideo;
}

void VideoConverter::setFFmpegApp(QString ffmpegApp)
{
	this->ffmpegApp = ffmpegApp;
}

void VideoConverter::setWorkingDir(QString workingDir)
{
	this->workingDir = workingDir;
}

void VideoConverter::setConversionConfig(VideoConversionConfig convConf)
{
	this->convConf = convConf;
}

void VideoConverter::setDeleteOriginalVideo(bool deleteOriginalVideo)
{
	this->deleteOriginalVideo = deleteOriginalVideo;
}

QStringList VideoConverter::getOutputFormatAsStrings()
{
	QStringList itemsToAdd;
	// set items to outputFormat
	itemsToAdd	<< tr("AVI Format (*.avi)") << tr("WMV Format ( *.wmv)")
	<< tr("MPEG1 Format ( *.mpg)") << tr("MPEG2 Format ( *.mpg)")
	<< tr("MP4 Format (*.mp4)") << tr("MP4 HD Format (*.mp4)") << tr("Apple iPod (*.mp4)") << tr("Sony PSP (*.mp4)")
	<< tr("3GP Format (*.3gp)") << tr("MP3 Format (*.mp3)");
	// return values
	return itemsToAdd;
}

QStringList VideoConverter::getVideoResolutionAsStrings()
{
	QStringList itemsToAdd;
	// set items to videoResolution
	itemsToAdd	<< tr("Original") << "96 x 72" << "128 x 96" << "160 x 120"
	<< "176 x 120" << "176 x 144" << "192 x 144" << "240 x 180"
	<< "320 x 200" << "320 x 240" << "352 x 240" << "352 x 288"
	<< "480 x 272" << "480 x 360" << "480 x 480" << "624 x 352"
	<< "640 x 480" << "720 x 480" << "720 x 576";
	// return values
	return itemsToAdd;
}

QStringList VideoConverter::getAudioSampleRatioAsStrings()
{
	QStringList itemsToAdd;
	// set items to audioSampleRatio
	itemsToAdd	<< tr("Original") << "22050" << "44100";
	// return values
	return itemsToAdd;
}

QStringList VideoConverter::getVideoFrameRateAsStrings()
{
	QStringList itemsToAdd;
	// set items to videoFrameRate
	itemsToAdd	<< tr("Original") << "15" << "24" << "25" << "29.97" << "30";
	// return values
	return itemsToAdd;
}

QStringList VideoConverter::getOutputQualityAsStrings()
{
	QStringList itemsToAdd;
	// set items to outputQuality
	itemsToAdd	<< tr("Lower quality (Video bitrate: 384kbps; Audio bitrate: 64kbps)")
	<< tr("Low quality (Video bitrate: 512kbps; Audio bitrate: 80kbps)")
	<< tr("Normal quality (Video bitrate: 640kbps; Audio bitrate: 96kbps)")
	<< tr("Medium quality (Video bitrate: 800kbps; Audio bitrate: 96kbps)")
	<< tr("Good quality (Video bitrate: 1000kbps; Audio bitrate: 128kbps)")
	<< tr("Superb quality (Video bitrate: 1200kbps; Audio bitrate: 128kbps)")
	<< tr("The Same quality as the original Video");
	// return values
	return itemsToAdd;
}

void VideoConverter::started()
{
	initTimer();

	videoItem->setAsConverting(this);
	emit conversionStarted(videoItem);
}

void VideoConverter::finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	deinitTimer();

	if (QFile::exists(videoItem->getVideoFileSavedTo()))
	{
		QFile output(videoItem->getVideoFileSavedTo());
		// has been converted?!
		if (output.size() == 0) // no
			{
				QFile::remove(videoItem->getVideoFileSavedTo());
				videoItem->setVideoFileSavedTo(videoItem->getVideoFile(), this);
			}
		else // yes
			if (deleteOriginalVideo)
				QFile::remove(videoItem->getVideoFile());
	}
	else
		videoItem->setVideoFileSavedTo(videoItem->getVideoFile(), this);

	videoItem->setAsConverted(this);
	videoItem->unlock(this);

	emit conversionFinished(videoItem);
}

void VideoConverter::readyReadStandardOutput()
{
	parseOutput(QString::fromLocal8Bit(ffmpegProcess->readAllStandardOutput()));
}

void VideoConverter::readyReadStandardError()
{
	parseOutput(QString::fromLocal8Bit(ffmpegProcess->readAllStandardError()));
}

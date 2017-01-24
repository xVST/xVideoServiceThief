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

#ifndef __CONVERTVIDEO_H__
#define __CONVERTVIDEO_H__

#include <QtWidgets>

#include "videoconverttypes.h"

class VideoItem;

class VideoConverter : public QObject
{
Q_OBJECT
	private:
		QProcess *ffmpegProcess;	//!< ffmpeg process
		QString ffmpegApp;			//!< ffmpeg path (including the app)
		QString workingDir;			//!< where to save converted videos
		VideoConversionConfig convConf;			//!< conversion config stored (configured in options)
		VideoConversionConfig convConfToUse;	//!< conversion config used at the moment to convert (it can be overrided)
		VideoItem *videoItem;		//!< current video item
		float videoLength;			//!< video time duration (seconds)
		bool deleteOriginalVideo;	//!< delete the input video on finish?
		QString outputAll;			//!< Temporal console output
		int internalTimer;			//!< Internal timer id
		float lastProgress;			//!< Last progress value
		/*! Build the ffmpeg command line */
		QStringList getCommandLine();
		/*! Convert & set the video duration to seconds */
		void setVideoDuration(QString strVideoDuration);
		/*! Get the current time conversion */
		void getCurrentTimeConversion(QString strFrame);
		/*! Parse the current output text */
		void parseOutput(QString output);
		/*! Start timer */
		void initTimer();
		/*! Stop timer */
		void deinitTimer();
	protected:
		/*! internal timer event */
		void timerEvent(QTimerEvent *event);
	public:
		/*! Class constructor */
		VideoConverter(QString ffmpegApp, QString workingDir,
			VideoConversionConfig convConf, bool deleteOriginalVideo);
		/*! Class destructor */
		~VideoConverter();
		/*! Start a video conversion */
		void startConversionVideo(VideoItem *videoItem);
		/*! Cancel current conversion */
		void cancelConversion();
		/*! Get if possible start a new download */
		bool canStartConversion();
		/*! Get if is converting some video */
		bool isConverting();
		/*! Get if is possible convert videos (ffmpeg installed?) */
		bool ffmpegInstalled();
		/*! Get the ffmpeg app */
		QString getFFmpegApp();
		/*! Get the working dir */
		QString getWorkingDir();
		/*! Get the conversion config */
		VideoConversionConfig getConversionConfig();
		/* Get the flag of delete original video */
		bool getDeleteOriginalVideo();
		/*! Set the ffmpeg app */
		void setFFmpegApp(QString ffmpegApp);
		/*! Set the working dir */
		void setWorkingDir(QString workingDir);
		/*! Set the video conversion config */
		void setConversionConfig(VideoConversionConfig convConf);
		/*! Set the flag for delete original videos on finish the conversion */
		void setDeleteOriginalVideo(bool deleteOriginalVideo);
		/*! Get the output format as redeable strings */
		static QStringList getOutputFormatAsStrings();
		/*! Get the video resolution as redeable strings */
		static QStringList getVideoResolutionAsStrings();
		/*! Get the audio sample ratio as redeable strings */
		static QStringList getAudioSampleRatioAsStrings();
		/*! Get the video frame rate as redeable strings */
		static QStringList getVideoFrameRateAsStrings();
		/*! Get the output quality as redeable strings */
		static QStringList getOutputQualityAsStrings();
	private slots:
		/*! ffmpeg started */
		void started();
		/*! ffmpeg finished */
		void finished(int exitCode, QProcess::ExitStatus exitStatus);
		/*! output recived */
		void readyReadStandardOutput();
		/*! output by error recived */
		void readyReadStandardError();
	signals:
		/*! Video item has been modified */
		void videoItemUpdated(VideoItem *videoItem);
		/*! A Video item conversion started */
		void conversionStarted(VideoItem *videoItem);
		/*! A Video item conversion finished */
		void conversionFinished(VideoItem *videoItem);
};

#endif // __CONVERTVIDEO_H__

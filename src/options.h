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

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <QtGui>
#include <QNetworkProxy>

#include "videoconverttypes.h"

#ifdef Q_OS_LINUX // modification made by "AzalSup"
static const QString DEFAULT_DOWNLOADS = "/xVideoServiceThief_downloads";	//!< Default sub-directory for downloads
#else
static const QString DEFAULT_DOWNLOADS = "/downloads";	//!< Default sub-directory for downloads
#endif

#ifdef Q_OS_WIN32
static const QString DEFAULT_FFMPEGLIB = "/bin";		//!< Default sub-directory for the ffmpeg lib (win32)
#else
static const QString DEFAULT_FFMPEGLIB = "/usr/bin";	//!< Default directory for the ffmpeg lib (*nix)
#endif

/*! Program options class: Save and Load the main program options */
class ProgramOptions : public QObject
{
Q_OBJECT
	private:
		QString ffmpegLibLocation;		//!< ffmpeg lib (app) path (path + name)
		bool deleteVideosOnConvert;		//!< flag for know if the original video must be deleted after its conversion
		bool convertVideos;				//!< flag for know if the downloaded video should be converted
		QString downloadDir;			//!< downloads directory
		bool downloadAutomatically;		//!< flag for know if can download videos automatically
		VideoConversionConfig conversionConf;	//!< Video conversion configuration
		bool displayPopup;				//!< flag for know if can display popups on finish
		bool saveRestoreSessions;		//!< flag for know if can save and restore sessions
		bool saveLogDownloadedVideos;	//!< flag for know if can make a log with all downloaded files
		bool dontRememberDownloadedVideos;	//!< flag for know if can remember "completed" videos
		bool blockAdultContent;			//!< flag for know if adult content is allowed or not
		QString blockAdultContentPassword;	//!< Ups password protection
		QString blockedWebsitesList;	//!< fist of services which are blocked (not allowed)	int proxyPort;
		int maxActiveDownloads;			//!< Maximum active downloads
		bool systemProxyConfiguration;	//!< flag for know if Proxy is automatic (using system-configuration)
		bool useProxy;					//!< flag for know if Proxy is enabled or not
		QString proxyAdress;			//!< Proxy Adress
		QString proxyPassword;			//!< Proxy Password
		QString proxyUserName;			//!< Proxy UserName
		int proxyPort;					//!< Proxy Port
		int proxyType;					//!< Proxy Type
		int dragDropLeft;				//!< Drag & Drop left position
		int dragDropTop;				//!< Drag & Drop top position
		int dragDropAlphaBlendValue;	//!< Drag & Drop enable alpha blend
		bool dragDropAlphaBlend;		//!< Drag & Drop alpha blend value
		QString languageFile;			//!< Language file
		bool installAutomaticallyUpdates;	//!< Download and Install automatically the updates
		QDate lastUpdate;				//!< Last update date
		bool checkForUpdatesOnStartup;	//!< Flag for know if must check for updates on program start
		int checkForUpdatesEvery;		//!< Check for updates every X days
		bool displayBugReport;			//!< Flag for know if can display the "Error Report"
		bool firstTime;					//!< Is the first program executation??
		bool stayOnTop;					//!< Display the application always on top?
		bool minimizeToSystemTray;		//!< Minimize the application to the system tray?
		int mainWindowHeight;			//!< Main window height
		int mainWindowWidth;			//!< Main window width
		bool mainWinowMaximized;		//!< Flag for know if the main window should be maximized
		bool vistaUpdatesMessage;		//!< Flag for know if we won't see the attention message of vista (updates)
		bool useInternalFFmpeg;			//!< Flag for know if the ffmpeg lib is into the app bundle
		bool displayWelcomeMessage;		//!< Flag for know if can display the welcome message
		bool displayDownloadsMigrator;	//!< Flag for know if can display/check the windows vista/7 downloads migrator message
		bool scheduleEnabled;			//!< Flag for know if the shcedule option is enabled
		QString latestVersionExecuted;	//!< Stores the latest xVST version executed
		bool deleteFailedDownloads;		//!< Flag for know if should delete the downloaded file after download error
		bool enableTimeOut;				//!< Flag for know if the time out-option is enabled or disabled
		int timeOut;					//!< Time-out value
		int maxRetries;					//!< Max. retries value
		int timeRemainingCalculation;	//!< Time remaining algorithm calculation
		QDir appDir;		//!< Initial program path
		QString appExecutable;	//!< Executable program path
		QString optionsFile;	//!< Options file name (path + name)

		bool canSendUpdateSignal;	//!< flag for know if is the update signal can be sended
		/*! Send the update signal (only if is possible) */
		void sendUpdateSignal();

		/*! Class constructor (private due to singleton) */
		ProgramOptions(QString optionsPath);
		/*! Class destructor (private due to singleton) */
		~ProgramOptions();
	public:
		/*! Get singleton instance of ProgramOptions */
		static ProgramOptions* instance();
		/*! Destroy singleton instance of ProgramOptions */
		static void destroyInstance();
		/*! Load the program options */
		void load();
		/*! Save the program options */
		void save();
		/*! Set the default program options */
		void setDefault();
		/*! Set if the update signal can be launched */
		void setCanSendUpdateSignal(bool canSendUpdateSignal);
		/*! Get the application path */
		QString getApplicationPath();
		/*! Get the options path */
		QString getOptionsPath();
		/*! Get the languages path */
		QString getLanguagesPath();
		/*! Get the plugins path */
		QString getPluginsPath();
		/*! Get the tools path */
		QString getToolsPath();
		/*! Get the options format */
		QSettings::Format getOptionsFormat();
#ifdef Q_WS_MAC
		/*! Get if internal ffmpeg is installed */
		bool getIfInternalFFmpegIsInstalled();
		/*! Get the internal ffmpeg path */
		QString getInternalFFmpegPath();
#endif
		// set / get methods
		void setFfmpegLibLocation(QString value);
		QString getFfmpegLibLocation();
		void setDeleteVideosOnConvert(bool value);
		bool getDeleteVideosOnConvert();
		void setConvertVideos(bool value);
		bool getConvertVideos();
		void setDownloadDir(QString value);
		QString getDownloadDir();
		void setDownloadAutomatically(bool value);
		bool getDownloadAutomatically();
		void setConversionConf(VideoConversionConfig value);
		VideoConversionConfig getConversionConf();
		void setDisplayPopup(bool value);
		bool getDisplayPopup();
		void setSaveRestoreSessions(bool value);
		bool getSaveRestoreSessions();
		void setSaveLogDownloadedVideos(bool value);
		bool getSaveLogDownloadedVideos();
		void setDontRememberDownloadedVideos(bool value);
		bool getDontRememberDownloadedVideos();
		void setBlockAdultContent(bool value);
		bool getBlockAdultContent();
		void setBlockedWebsitesList(QString value);
		QString getBlockedWebsitesList();
		void setMaxActiveDownloads(int value);
		int getMaxActiveDownloads();
		void setProxyPort(int value);
		int getProxyPort();
		void setProxyAdress(QString value);
		QString getProxyAdress();
		void setProxyPassword(QString value);
		QString getProxyPassword();
		void setProxyUserName(QString value);
		QString getProxyUserName();
		void setUseProxy(bool value);
		bool getUseProxy();
		void setUseSystemProxyConfig(bool value);
		bool getUseSystemProxyConfig();
		void setProxyType(int value);
		int getProxyType();
		void setDragDropLeft(int value);
		int getDragDropLeft();
		void setDragDropAlphaBlendValue(int value);
		int getDragDropAlphaBlendValue();
		void setDragDropAlphaBlend(bool value);
		bool getDragDropAlphaBlend();
		void setDragDropTop(int value);
		int getDragDropTop();
		void setLanguageFile(QString value);
		QString getLanguageFile(bool fullPath);
		void setInstallAutomaticallyUpdates(bool value);
		bool getInstallAutomaticallyUpdates();
		void setLastUpdate(QDate value);
		QDate getLastUpdate();
		void setCheckForUpdatesOnStartup(bool value);
		bool getCheckForUpdatesOnStartup();
		void setCheckForUpdatesEvery(int value);
		int getCheckForUpdatesEvery();
		void setDisplayBugReport(bool value);
		bool getDisplayBugReport();
		void setFirstTime(bool value);
		bool getFirstTime();
		void setStayOnTop(bool value);
		bool getStayOnTop();
		void setMinimizeToSystemTray(bool value);
		bool getMinimizeToSystemTray();
		void setMainWindowHeight(int value);
		int getMainWindowHeight();
		void setMainWindowWidth(int value);
		int getMainWindowWidth();
		void setMainWinowMaximized(bool value);
		bool getMainWinowMaximized();
		void setVistaUpdatesMessage(bool value);
		bool getVistaUpdatesMessage();
		void setUseInternalFFmpeg(bool value);
		bool getUseInternalFFmpeg();
		void setDisplayWelcomeMessage(bool value);
		bool getDisplayWelcomeMessage();
		void setDisplayDownloadsMigrator(bool value);
		bool getDisplayDownloadsMigrator();
		void setScheduleEnabled(bool value);
		bool getScheduleEnabled();
		void setBlockAdultContentPassword(QString value);
		QString getBlockAdultContentPassword();
		void setLatestVersionExecuted(QString value);
		QString getLatestVersionExecuted();
		void setDeleteFailedDownloads(bool value);
		bool getDeleteFailedDownloads();
		void setEnableTimeOut(bool value);
		bool getEnableTimeOut();
		void setTimeOut(int value);
		int getTimeOut();
		void setMaxRetries(int value);
		int getMaxRetries();
		void setTimeRemainingCalculation(int value);
		int getTimeRemainingCalculation();
	signals:
		/*! Options will load the configuration */
		void optionsLoadBefore();
		/*! Options loaded the configuration */
		void optionsLoadAfter();
		/*! Options will save the configuration */
		void optionsSaveBefore();
		/*! Options saved the configuration */
		void optionsSaveAfter();
		/*! An option has been modified */
		void optionsUpdated();
};

#endif // __OPTIONS_H__

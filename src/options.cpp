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

#include "options.h"

#include "videoconvert.h"
#include "languages.h"
#include "tools.h"

#ifdef Q_OS_MACX
#include "mac_only/mac_tools.h"
#endif

static ProgramOptions *programOptionsInstance = nullptr;

ProgramOptions::ProgramOptions(QString optionsPath)
{
#ifdef Q_OS_MACX
	appDir.setPath(QCoreApplication::applicationDirPath());
	optionsFile = QString(optionsPath + "/com.xVideoServiceThief.config.plist");
#endif
#ifdef Q_OS_WIN32
	appDir.setPath(QCoreApplication::applicationDirPath());
	optionsFile = QString(optionsPath + "/config.conf");
#endif
#ifdef Q_OS_LINUX
	appDir.setPath(optionsPath);
	optionsFile = QString(optionsPath + "/config.conf");
#endif
	// other inits
	canSendUpdateSignal = true;
}

ProgramOptions::~ProgramOptions()
{
	save();
}

ProgramOptions* ProgramOptions::instance()
{
    if (programOptionsInstance == nullptr)
	{
#ifdef Q_OS_LINUX // modification made by "AzalSup"
		QString _homeDirectory  = getenv("HOME");
		_homeDirectory += "/.xVideoServiceThief";
		programOptionsInstance = new ProgramOptions(_homeDirectory);
#endif
#ifdef Q_OS_MACX
		QString preferencesPath = QString(QDir::homePath() + "/Library/Preferences");
		programOptionsInstance = new ProgramOptions(preferencesPath);
#endif
#ifdef Q_OS_WIN32
		QString programFiles = QString(getenv("APPDATA")) + "/xVideoServiceThief";
		// for old users, check if config.conf is present in xVST dir and copy it to new location
		if (QFile::exists(qApp->applicationDirPath() + "/config.conf") &&
			!QFile::exists(programFiles + "/config.conf"))
		{
			QDir().mkpath(programFiles);
			QFile conf(qApp->applicationDirPath() + "/config.conf");
			conf.copy(programFiles + "/config.conf");
			QFile historic(qApp->applicationDirPath() + "/historic.log");
			historic.copy(programFiles + "/historic.log");
			QFile session(qApp->applicationDirPath() + "/session.conf");
			session.copy(programFiles + "/session.conf");
		}
		// load options
		programOptionsInstance = new ProgramOptions(programFiles);
#endif
	}
	// return program instance
	return programOptionsInstance;
}

void ProgramOptions::destroyInstance()
{
    if (programOptionsInstance != nullptr)
	{
		delete programOptionsInstance;
        programOptionsInstance = nullptr;
	}
}

void ProgramOptions::load()
{
	emit optionsLoadBefore();

	// set the default values
	setDefault();

	// if options file don't exists, then abort...
	if (!QFile::exists(optionsFile)) return;

	// load config
#ifdef Q_OS_MACX
	QSettings settings(optionsFile, QSettings::NativeFormat);
#else
	QSettings settings(optionsFile, QSettings::IniFormat);
#endif
	downloadAutomatically = settings.value("configuration/downloadAutomatically", downloadAutomatically).toBool();
	downloadDir = settings.value("configuration/downloadDir", downloadDir).toString();
	convertVideos = settings.value("configuration/convertVideos", convertVideos).toBool();
	deleteVideosOnConvert = settings.value("configuration/deleteVideosOnConvert", deleteVideosOnConvert).toBool();
	ffmpegLibLocation = settings.value("configuration/ffmpegLibLocation", ffmpegLibLocation).toString();
	displayPopup = settings.value("configuration/displayPopup", displayPopup).toBool();

	conversionConf.outputFormat = static_cast<OutputFormat>(settings.value("configuration/outputFormat", conversionConf.outputFormat).toInt());
	conversionConf.videoResolution = static_cast<VideoResolution>(settings.value("configuration/videoResolution", conversionConf.videoResolution).toInt());
	conversionConf.audioSampleRatio = static_cast<AudioSampleRatio>(settings.value("configuration/audioSampleRatio", conversionConf.audioSampleRatio).toInt());
	conversionConf.videoFrameRate = static_cast<VideoFrameRate>(settings.value("configuration/videoFrameRate", conversionConf.videoFrameRate).toInt());
	conversionConf.outputQuality = static_cast<OutputQuality>(settings.value("configuration/outputQuality", conversionConf.outputQuality).toInt());

	saveRestoreSessions = settings.value("configuration/saveRestoreSessions", saveRestoreSessions).toBool();
	saveLogDownloadedVideos = settings.value("configuration/saveLogDownloadedVideos", saveLogDownloadedVideos).toBool();
	dontRememberDownloadedVideos = settings.value("configuration/dontRememberDownloadedVideos", dontRememberDownloadedVideos).toBool();

	blockAdultContent = settings.value("configuration/blockAdultContent", blockAdultContent).toBool();
	blockedWebsitesList = settings.value("configuration/blockedWebsitesList", blockAdultContent).toString();
	blockAdultContentPassword = settings.value("configuration/blockAdultContentPassword", blockAdultContentPassword).toString();

	dragDropLeft = settings.value("configuration/dragDropLeft", dragDropTop).toInt();
	dragDropTop = settings.value("configuration/dragDropTop", dragDropTop).toInt();
	dragDropAlphaBlendValue = settings.value("configuration/dragDropAlphaBlendValue", dragDropAlphaBlendValue).toInt();
	dragDropAlphaBlend = settings.value("configuration/dragDropAlphaBlend", dragDropAlphaBlend).toBool();

	maxActiveDownloads = settings.value("configuration/maxActiveDownloads", maxActiveDownloads).toInt();

	systemProxyConfiguration = settings.value("configuration/systemProxyConfiguration", systemProxyConfiguration).toBool();
	useProxy = settings.value("configuration/useProxy", useProxy).toBool();
	proxyAddress = settings.value("configuration/proxyAddress", proxyAddress).toString();
	proxyPassword = settings.value("configuration/proxyPassword", proxyPassword).toString();
	proxyUserName = settings.value("configuration/proxyUserName", proxyUserName).toString();
	proxyPort = settings.value("configuration/proxyPort", proxyPort).toInt();
	proxyType = settings.value("configuration/proxyType", proxyType).toInt();

	languageFile = settings.value("configuration/languageFile", languageFile).toString();

	installAutomaticallyUpdates = settings.value("configuration/installAutomaticallyUpdates", installAutomaticallyUpdates).toBool();
	lastUpdate = settings.value("configuration/lastUpdate", lastUpdate).toDate();
	checkForUpdatesOnStartup = settings.value("configuration/checkForUpdatesOnStartup", checkForUpdatesOnStartup).toBool();
	checkForUpdatesEvery = settings.value("configuration/checkForUpdatesEvery", checkForUpdatesEvery).toInt();

	displayBugReport = settings.value("configuration/displayBugReport", displayBugReport).toBool();

	firstTime = settings.value("configuration/firstTime", firstTime).toBool();

	stayOnTop = settings.value("configuration/stayOnTop", stayOnTop).toBool();
	minimizeToSystemTray = settings.value("configuration/minimizeToSystemTray", minimizeToSystemTray).toBool();

	mainWindowHeight = settings.value("configuration/mainWindowHeight", mainWindowHeight).toInt();
	mainWindowWidth = settings.value("configuration/mainWindowWidth", mainWindowWidth).toInt();
	mainWinowMaximized = settings.value("configuration/mainWinowMaximized", mainWinowMaximized).toBool();

	vistaUpdatesMessage = settings.value("configuration/vistaUpdatesMessage", vistaUpdatesMessage).toBool();

	useInternalFFmpeg = settings.value("configuration/useInternalFFmpeg", useInternalFFmpeg).toBool();
#ifdef Q_OS_MACX
	if (useInternalFFmpeg) ffmpegLibLocation = getInternalFFmpegPath();
#endif

	displayWelcomeMessage = settings.value("configuration/displayWelcomeMessage", displayWelcomeMessage).toBool();
	displayDownloadsMigrator = settings.value("configuration/displayDownloadsMigrator", displayDownloadsMigrator).toBool();

	scheduleEnabled = settings.value("configuration/scheduleEnabled", scheduleEnabled).toBool();

	latestVersionExecuted = settings.value("configuration/latestVersionExecuted", latestVersionExecuted).toString();

	deleteFailedDownloads = settings.value("configuration/deleteFailedDownloads", deleteFailedDownloads).toBool();
	enableTimeOut = settings.value("configuration/enableTimeOut", enableTimeOut).toBool();
	timeOut = settings.value("configuration/timeOut", timeOut).toInt();
	maxRetries = settings.value("configuration/maxRetries", maxRetries).toInt();
	timeRemainingCalculation = settings.value("configuration/timeRemainingCalculation", timeRemainingCalculation).toInt();

	adultSitesAreAllowed = settings.value("configuration/adultSitesAreAllowed", adultSitesAreAllowed).toBool();
	adultsSitePermissionAsked = settings.value("configuration/adultsSitePermissionAsked", adultsSitePermissionAsked).toBool();

    classicUI = settings.value("configuration/classicUI", classicUI).toBool();
    hideDownloadInformationBox = settings.value("configuration/hideDownloadInformationBox", hideDownloadInformationBox).toBool();
    hideConvertInformationBox = settings.value("configuration/hideConvertInformationBox", hideConvertInformationBox).toBool();
    hideDownloadsBox = settings.value("configuration/hideDownloadsBox", hideDownloadsBox).toBool();

	emit optionsLoadAfter();
}

void ProgramOptions::save()
{
	emit optionsSaveBefore();

#ifdef Q_OS_MACX
	QSettings settings(optionsFile, QSettings::NativeFormat);
#else
	QSettings settings(optionsFile, QSettings::IniFormat);
#endif

	settings.beginGroup("configuration");

	settings.setValue("downloadAutomatically", downloadAutomatically);
	settings.setValue("downloadDir", downloadDir);
	settings.setValue("convertVideos", convertVideos);
	settings.setValue("deleteVideosOnConvert", deleteVideosOnConvert);
	settings.setValue("ffmpegLibLocation", ffmpegLibLocation);
	settings.setValue("displayPopup", displayPopup);

	settings.setValue("outputFormat", conversionConf.outputFormat);
	settings.setValue("videoResolution", conversionConf.videoResolution);
	settings.setValue("audioSampleRatio", conversionConf.audioSampleRatio);
	settings.setValue("videoFrameRate", conversionConf.videoFrameRate);
	settings.setValue("outputQuality", conversionConf.outputQuality);

	settings.setValue("saveRestoreSessions", saveRestoreSessions);
	settings.setValue("saveLogDownloadedVideos", saveLogDownloadedVideos);
	settings.setValue("dontRememberDownloadedVideos", dontRememberDownloadedVideos);

	settings.setValue("blockAdultContent", blockAdultContent);
	settings.setValue("blockedWebsitesList", blockedWebsitesList);
	settings.setValue("blockAdultContentPassword", blockAdultContentPassword);

	settings.setValue("maxActiveDownloads", maxActiveDownloads);

	settings.setValue("systemProxyConfiguration", systemProxyConfiguration);
	settings.setValue("useProxy", useProxy);
	settings.setValue("proxyAddress", proxyAddress);
	settings.setValue("proxyPassword", proxyPassword);
	settings.setValue("proxyUserName", proxyUserName);
	settings.setValue("proxyPort", proxyPort);
	settings.setValue("proxyType", proxyType);

	settings.setValue("dragDropLeft", dragDropLeft);
	settings.setValue("dragDropTop", dragDropTop);
	settings.setValue("dragDropAlphaBlendValue", dragDropAlphaBlendValue);
	settings.setValue("dragDropAlphaBlend", dragDropAlphaBlend);

	settings.setValue("languageFile", languageFile);

	settings.setValue("installAutomaticallyUpdates", installAutomaticallyUpdates);
	settings.setValue("lastUpdate", lastUpdate);
	settings.setValue("checkForUpdatesOnStartup", checkForUpdatesOnStartup);
	settings.setValue("checkForUpdatesEvery", checkForUpdatesEvery);

	settings.setValue("displayBugReport", displayBugReport);

	settings.setValue("firstTime", firstTime);

	settings.setValue("stayOnTop", stayOnTop);
	settings.setValue("minimizeToSystemTray", minimizeToSystemTray);

	settings.setValue("mainWindowHeight", mainWindowHeight);
	settings.setValue("mainWindowWidth", mainWindowWidth);
	settings.setValue("mainWinowMaximized", mainWinowMaximized);

	settings.setValue("vistaUpdatesMessage", vistaUpdatesMessage);

	settings.setValue("useInternalFFmpeg", useInternalFFmpeg);

	settings.setValue("displayWelcomeMessage", displayWelcomeMessage);
	settings.setValue("displayDownloadsMigrator", displayDownloadsMigrator);

	settings.setValue("scheduleEnabled", scheduleEnabled);

	settings.setValue("latestVersionExecuted", latestVersionExecuted);

	settings.setValue("deleteFailedDownloads", deleteFailedDownloads);
	settings.setValue("enableTimeOut", enableTimeOut);
	settings.setValue("timeOut", timeOut);
	settings.setValue("maxRetries", maxRetries);
	settings.setValue("timeRemainingCalculation", timeRemainingCalculation);

	settings.setValue("adultSitesAreAllowed", adultSitesAreAllowed);
	settings.setValue("adultsSitePermissionAsked", adultsSitePermissionAsked);

    settings.setValue("classicUI", classicUI);
    settings.setValue("hideDownloadInformationBox", hideDownloadInformationBox);
    settings.setValue("hideConvertInformationBox", hideConvertInformationBox);
    settings.setValue("hideDownloadsBox", hideDownloadsBox);

	settings.endGroup();

	emit optionsSaveAfter();
}

void ProgramOptions::setDefault()
{
	downloadAutomatically = true;
#ifdef Q_OS_MACX
	downloadDir = QString(QDir::homePath() + DEFAULT_DOWNLOADS);
#else
#ifdef Q_OS_LINUX // modification made by "AzalSup"
	QString _homeDirectory  = getenv("HOME");
	downloadDir = QString(_homeDirectory + DEFAULT_DOWNLOADS);
#else // windows
	if (isWindowsVista()) // windows vista and 7
		downloadDir = QString(QDir::homePath() + "/Videos/xVST");
	else // windows XP
		downloadDir = QString(appDir.absolutePath() + DEFAULT_DOWNLOADS);
#endif
#endif
	convertVideos = true;
	deleteVideosOnConvert = true;
#ifdef Q_OS_WIN32
	ffmpegLibLocation = QString(appDir.absolutePath() + DEFAULT_FFMPEGLIB + "/ffmpeg.exe");
#else
#ifdef Q_OS_MACX
	// check if our APP has a ffmpeg embeded
	if (QFile::exists(getInternalFFmpegPath()))
		ffmpegLibLocation = getInternalFFmpegPath();
	else // ffmpeg is not embedded into our .app, so set the "standard ffmpeg folder"
		ffmpegLibLocation = QString(DEFAULT_FFMPEGLIB + "/ffmpeg");
#else
	// linux scope
	ffmpegLibLocation = QString(DEFAULT_FFMPEGLIB + "/ffmpeg");
#endif
#endif

	displayPopup = true;

	conversionConf.outputFormat = ofAVI;
	conversionConf.videoResolution = vrOriginal;
	conversionConf.audioSampleRatio = asrOriginal;
	conversionConf.videoFrameRate = vfrOriginal;
	conversionConf.outputQuality = oqSame_quality;

	blockAdultContent = false;
	blockedWebsitesList = "";
	blockAdultContentPassword = "";

	saveRestoreSessions = true;
	saveLogDownloadedVideos = true;
	dontRememberDownloadedVideos = true;

	maxActiveDownloads = 4;

	systemProxyConfiguration = true;
	useProxy = false;
	proxyAddress = "";
	proxyPassword = "";
	proxyUserName = "";
	proxyPort = 0;
	proxyType = QNetworkProxy::NoProxy;

	dragDropLeft = -1;
	dragDropTop = -1;
	dragDropAlphaBlendValue = 100;
	dragDropAlphaBlend = false;

	languageFile = LanguageManager::getDefaultUserLanguage(getLanguagesPath()); // "english_uk.language";

	installAutomaticallyUpdates = true;
	lastUpdate = QDate(2007, 01, 01);
	checkForUpdatesOnStartup = true;
	checkForUpdatesEvery = 1;

	displayBugReport = true;

	firstTime = true;

	stayOnTop = false;
	minimizeToSystemTray = false;

	mainWindowHeight = 0;
	mainWindowWidth = 0;
	mainWinowMaximized = false;

	vistaUpdatesMessage = false;

	useInternalFFmpeg = true;

	displayWelcomeMessage = true;

	scheduleEnabled = false;

	deleteFailedDownloads = true;
	enableTimeOut = true;
	timeOut = 30;
	maxRetries = 3;
	timeRemainingCalculation = 0; // 0 = percentage; 1 = download speed

	adultsSitePermissionAsked = false;
	adultSitesAreAllowed = true;

#ifdef Q_OS_MACX
    classicUI = false;
    hideConvertInformationBox = false;
    hideDownloadInformationBox = true;
    hideDownloadsBox = true;
#else
    classicUI = true;
    hideConvertInformationBox = false;
    hideDownloadInformationBox = false;
    hideDownloadsBox = false;
#endif
}

void ProgramOptions::setCanSendUpdateSignal(bool canSendUpdateSignal)
{
	this->canSendUpdateSignal = canSendUpdateSignal;
}

QString ProgramOptions::getApplicationPath()
{
	return appDir.absolutePath();
}

QString ProgramOptions::getOptionsPath()
{
	return extractFilePath(optionsFile);
}

QString ProgramOptions::getLanguagesPath()
{
#ifdef Q_OS_MACX
	return getApplicationPath() + "/../Resources/languages";
#else
	return getApplicationPath() + "/languages";
#endif
}

QString ProgramOptions::getPluginsPath()
{
#ifdef Q_OS_MACX
	return getApplicationPath() + "/../Resources/plugins";
#else
	return getApplicationPath() + "/plugins";
#endif
}

QString ProgramOptions::getToolsPath()
{
#ifdef Q_OS_MACX
	return getApplicationPath() + "/../Resources/tools";
#endif
#ifdef Q_OS_WIN32
	return getApplicationPath() + "/bin";
#endif
#ifdef Q_OS_LINUX
	return getOptionsPath() + "/bin";
#endif
}

QSettings::Format ProgramOptions::getOptionsFormat()
{
#ifdef Q_OS_MACX
	return QSettings::NativeFormat;
#else
	return QSettings::IniFormat;
#endif
}

#ifdef Q_OS_MACX
bool ProgramOptions::getIfInternalFFmpegIsInstalled()
{
	return QFile::exists(getInternalFFmpegPath());
}

QString ProgramOptions::getInternalFFmpegPath()
{
	return QDir::cleanPath(appDir.absolutePath() + "/../Resources/tools/ffmpeg" + (isRunningSnowLeopard() ? "_universal" : "_x86"));
}
#endif

bool ProgramOptions::getAdultsSitePermissionAsked() const
{
	return adultsSitePermissionAsked;
}

void ProgramOptions::setAdultsSitePermissionAsked(bool value)
{
	adultsSitePermissionAsked = value;
}

bool ProgramOptions::getAdultSitesAreAllowed() const
{
	return adultSitesAreAllowed;
}

void ProgramOptions::setAdultSitesAreAllowed(bool value)
{
	adultSitesAreAllowed = value;
}

void ProgramOptions::sendUpdateSignal()
{
	if (canSendUpdateSignal)
		emit optionsUpdated();
}

QString ProgramOptions::getDownloadDir()
{
	return QDir::toNativeSeparators(downloadDir);
}

void ProgramOptions::setDownloadDir(QString value)
{
	downloadDir = value;

	sendUpdateSignal();
}

bool ProgramOptions::getDownloadAutomatically()
{
	return downloadAutomatically;
}

void ProgramOptions::setDownloadAutomatically(bool value)
{
	downloadAutomatically = value;

	sendUpdateSignal();
}

bool ProgramOptions::getConvertVideos()
{
	return convertVideos;
}

void ProgramOptions::setConvertVideos(bool value)
{
	convertVideos = value;

	sendUpdateSignal();
}

bool ProgramOptions::getDeleteVideosOnConvert()
{
	return deleteVideosOnConvert;
}

void ProgramOptions::setDeleteVideosOnConvert(bool value)
{
	deleteVideosOnConvert = value;

	sendUpdateSignal();
}

QString ProgramOptions::getFfmpegLibLocation()
{
	return QDir::toNativeSeparators(ffmpegLibLocation);
}

void ProgramOptions::setFfmpegLibLocation(QString value)
{
	ffmpegLibLocation = value.trimmed();

	sendUpdateSignal();
}

VideoConversionConfig ProgramOptions::getConversionConf()
{
	return conversionConf;
}

void ProgramOptions::setConversionConf(VideoConversionConfig value)
{
	conversionConf = value;

	sendUpdateSignal();
}

bool ProgramOptions::getDisplayPopup()
{
	return displayPopup;
}

void ProgramOptions::setDisplayPopup(bool value)
{
	displayPopup = value;

	sendUpdateSignal();
}

bool ProgramOptions::getSaveRestoreSessions()
{
	return saveRestoreSessions;
}

void ProgramOptions::setSaveRestoreSessions(bool value)
{
	saveRestoreSessions = value;
}

bool ProgramOptions::getDontRememberDownloadedVideos()
{
	return dontRememberDownloadedVideos;
}

void ProgramOptions::setDontRememberDownloadedVideos(bool value)
{
	dontRememberDownloadedVideos = value;
}

bool ProgramOptions::getSaveLogDownloadedVideos()
{
	return saveLogDownloadedVideos;
}

void ProgramOptions::setSaveLogDownloadedVideos(bool value)
{
	saveLogDownloadedVideos = value;
}

bool ProgramOptions::getBlockAdultContent()
{
	return blockAdultContent;
}

void ProgramOptions::setBlockAdultContent(bool value)
{
	blockAdultContent = value;
}

QString ProgramOptions::getBlockedWebsitesList()
{
	return blockedWebsitesList;
}

void ProgramOptions::setBlockedWebsitesList(QString value)
{
	blockedWebsitesList = value;
}

void ProgramOptions::setMaxActiveDownloads(int value)
{
	maxActiveDownloads = value;
}

int ProgramOptions::getMaxActiveDownloads()
{
	return maxActiveDownloads;
}

bool ProgramOptions::getUseProxy()
{
	return useProxy;
}

void ProgramOptions::setUseSystemProxyConfig(bool value)
{
	systemProxyConfiguration = value;
}

bool ProgramOptions::getUseSystemProxyConfig()
{
	return systemProxyConfiguration;
}

void ProgramOptions::setUseProxy(bool value)
{
	useProxy = value;
}

QString ProgramOptions::getProxyUserName()
{
	return proxyUserName;
}

void ProgramOptions::setProxyUserName(QString value)
{
	proxyUserName = value;
}

QString ProgramOptions::getProxyPassword()
{
	return proxyPassword;
}

void ProgramOptions::setProxyPassword(QString value)
{
	proxyPassword = value;
}

QString ProgramOptions::getProxyAddress()
{
	return proxyAddress;
}

void ProgramOptions::setProxyAddress(QString value)
{
	proxyAddress = value;
}

int ProgramOptions::getProxyPort()
{
	return proxyPort;
}

void ProgramOptions::setProxyPort(int value)
{
	proxyPort = value;
}

int ProgramOptions::getProxyType()
{
	return proxyType;
}

void ProgramOptions::setProxyType(int value)
{
	proxyType = value;
}

int ProgramOptions::getDragDropLeft()
{
	return dragDropLeft;
}

void ProgramOptions::setDragDropLeft(int value)
{
	dragDropLeft = value;
}

int ProgramOptions::getDragDropTop()
{
	return dragDropTop;
}

void ProgramOptions::setDragDropTop(int value)
{
	dragDropTop = value;
}

bool ProgramOptions::getDragDropAlphaBlend()
{
	return dragDropAlphaBlend;
}

void ProgramOptions::setDragDropAlphaBlend(bool value)
{
	dragDropAlphaBlend = value;
}

int ProgramOptions::getDragDropAlphaBlendValue()
{
	return dragDropAlphaBlendValue;
}

void ProgramOptions::setDragDropAlphaBlendValue(int value)
{
	dragDropAlphaBlendValue = value;
}

QString ProgramOptions::getLanguageFile(bool fullPath)
{
	return fullPath ? getLanguagesPath() + "/" + languageFile : languageFile;
}

void ProgramOptions::setLanguageFile(QString value)
{
	languageFile = value;
}

int ProgramOptions::getCheckForUpdatesEvery()
{
	return checkForUpdatesEvery;
}

void ProgramOptions::setCheckForUpdatesEvery(int value)
{
	checkForUpdatesEvery = value;
}

bool ProgramOptions::getCheckForUpdatesOnStartup()
{
	return checkForUpdatesOnStartup;
}

void ProgramOptions::setCheckForUpdatesOnStartup(bool value)
{
	checkForUpdatesOnStartup = value;
}

QDate ProgramOptions::getLastUpdate()
{
	return lastUpdate;
}

void ProgramOptions::setLastUpdate(QDate value)
{
	lastUpdate = value;
}

bool ProgramOptions::getInstallAutomaticallyUpdates()
{
	return installAutomaticallyUpdates;
}

void ProgramOptions::setInstallAutomaticallyUpdates(bool value)
{
	installAutomaticallyUpdates = value;
}

bool ProgramOptions::getDisplayBugReport()
{
	return displayBugReport;
}

void ProgramOptions::setDisplayBugReport(bool value)
{
	displayBugReport = value;
}

bool ProgramOptions::getFirstTime()
{
	return firstTime;
}

void ProgramOptions::setFirstTime(bool value)
{
	firstTime = value;
}

bool ProgramOptions::getStayOnTop()
{
	return stayOnTop;
}

void ProgramOptions::setStayOnTop(bool value)
{
	stayOnTop = value;
}

void ProgramOptions::setMinimizeToSystemTray(bool value)
{
	minimizeToSystemTray = value;
}

bool ProgramOptions::getMinimizeToSystemTray()
{
	return minimizeToSystemTray;
}

void ProgramOptions::setMainWindowHeight(int value)
{
	mainWindowHeight = value;
}

int ProgramOptions::getMainWindowHeight()
{
	return mainWindowHeight;
}

void ProgramOptions::setMainWindowWidth(int value)
{
	mainWindowWidth = value;
}

int ProgramOptions::getMainWindowWidth()
{
	return mainWindowWidth;
}

void ProgramOptions::setMainWinowMaximized(bool value)
{
	mainWinowMaximized = value;
}

bool ProgramOptions::getMainWinowMaximized()
{
	return mainWinowMaximized;
}

void ProgramOptions::setVistaUpdatesMessage(bool value)
{
	vistaUpdatesMessage = value;
}

bool ProgramOptions::getVistaUpdatesMessage()
{
	return vistaUpdatesMessage;
}

void ProgramOptions::setUseInternalFFmpeg(bool value)
{
	useInternalFFmpeg = value;
}

bool ProgramOptions::getUseInternalFFmpeg()
{
	return useInternalFFmpeg;
}

void ProgramOptions::setDisplayWelcomeMessage(bool value)
{
	displayWelcomeMessage = value;
}

bool ProgramOptions::getDisplayWelcomeMessage()
{
	return displayWelcomeMessage;
}

void ProgramOptions::setDisplayDownloadsMigrator(bool value)
{
	displayDownloadsMigrator = value;
}

bool ProgramOptions::getDisplayDownloadsMigrator()
{
	return displayDownloadsMigrator;
}

void ProgramOptions::setScheduleEnabled(bool value)
{
	scheduleEnabled = value;
}

bool ProgramOptions::getScheduleEnabled()
{
	return scheduleEnabled;
}

void ProgramOptions::setBlockAdultContentPassword(QString value)
{
	blockAdultContentPassword = value;
}

QString ProgramOptions::getBlockAdultContentPassword()
{
	return blockAdultContentPassword;
}

void ProgramOptions::setLatestVersionExecuted(QString value)
{
	latestVersionExecuted = value;
}

QString ProgramOptions::getLatestVersionExecuted()
{
	return latestVersionExecuted;
}

void ProgramOptions::setDeleteFailedDownloads(bool value)
{
	deleteFailedDownloads = value;
}

bool ProgramOptions::getDeleteFailedDownloads()
{
	return deleteFailedDownloads;
}

void ProgramOptions::setEnableTimeOut(bool value)
{
	enableTimeOut = value;
}

bool ProgramOptions::getEnableTimeOut()
{
	return enableTimeOut;
}

void ProgramOptions::setTimeOut(int value)
{
	timeOut = value;
}

int ProgramOptions::getTimeOut()
{
	return timeOut;
}

void ProgramOptions::setMaxRetries(int value)
{
	maxRetries = value;
}

int ProgramOptions::getMaxRetries()
{
	return maxRetries;
}

void ProgramOptions::setTimeRemainingCalculation(int value)
{
	timeRemainingCalculation = value;
}

int ProgramOptions::getTimeRemainingCalculation()
{
	return timeRemainingCalculation;
}

void ProgramOptions::setClassicUI(bool value)
{
    classicUI = value;
}

bool ProgramOptions::getClassicUI() const
{
    return classicUI;
}

void ProgramOptions::setHideConvertInformationBox(bool value)
{
    hideConvertInformationBox = value;
}

bool ProgramOptions::getHideConvertInformationBox() const
{
    return hideConvertInformationBox;
}

void ProgramOptions::setHideDownloadInformationBox(bool value)
{
    hideDownloadInformationBox = value;
}

bool ProgramOptions::getHideDownloadInformationBox() const
{
    return hideDownloadInformationBox;
}

void ProgramOptions::setHideDownloadsBox(bool value)
{
    hideDownloadsBox = value;
}

bool ProgramOptions::getHideDownloadsBox() const
{
    return hideDownloadsBox;
}

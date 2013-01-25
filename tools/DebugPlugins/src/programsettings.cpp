#include "programsettings.h"

ProgramSettings::ProgramSettings()
{
#ifdef Q_OS_LINUX // modification made by "AzalSup"
	settingsFile = QDir::homePath() + "/.DebugPlugins" + SETTINGS_FILE;
#endif
#ifdef Q_WS_MACX
	settingsFile = QDir::homePath() + "/Library/Preferences" + SETTINGS_FILE;
#endif
#ifdef Q_OS_WIN32
	settingsFile = QDir::homePath() + "/DebugPlugins" + SETTINGS_FILE;
#endif
	// init defaults
	videoInformationPlugin = "";
	videoInformationURL = "";
	videoSearchPlugin = "";
	videoSearchKeyWords = "";
	pluginIcon = "";
}

void ProgramSettings::save()
{
	// init settings file
#ifdef Q_WS_MACX
	QSettings settings(settingsFile, QSettings::NativeFormat);
#else
	QSettings settings(settingsFile, QSettings::IniFormat);
#endif
	settings.beginGroup("settings");
	// assign values
	settings.setValue("videoInformationPlugin", videoInformationPlugin);
	settings.setValue("videoInformationURL", videoInformationURL);
	settings.setValue("videoSearchPlugin", videoSearchPlugin);
	settings.setValue("videoSearchKeyWords", videoSearchKeyWords);
	settings.setValue("pluginIcon", pluginIcon);
	settings.setValue("pluginIconsPath", pluginIconsPath);
	settings.setValue("recursiveGeneration", recursiveGeneration);
	// save settings
	settings.endGroup();
}

void ProgramSettings::load()
{
	// load settings
#ifdef Q_WS_MACX
	QSettings settings(settingsFile, QSettings::NativeFormat);
#else
	QSettings settings(settingsFile, QSettings::IniFormat);
#endif
	// assign values
	videoInformationPlugin = settings.value("settings/videoInformationPlugin", videoInformationPlugin).toString();
	videoInformationURL = settings.value("settings/videoInformationURL", videoInformationURL).toString();
	videoSearchPlugin = settings.value("settings/videoSearchPlugin", videoSearchPlugin).toString();
	videoSearchKeyWords = settings.value("settings/videoSearchKeyWords", videoSearchKeyWords).toString();
	pluginIcon = settings.value("settings/pluginIcon", pluginIcon).toString();
	pluginIconsPath = settings.value("settings/pluginIconsPath", pluginIconsPath).toString();
	recursiveGeneration = settings.value("settings/recursiveGeneration", recursiveGeneration).toBool();
}

QString ProgramSettings::getVideoInformationPlugin()
{
	return videoInformationPlugin;
}

void ProgramSettings::setVideoInformationPlugin(QString value)
{
	videoInformationPlugin = value;
}

QString ProgramSettings::getVideoInformationURL()
{
	return videoInformationURL;
}

void ProgramSettings::setVideoInformationURL(QString value)
{
	videoInformationURL = value;
}

QString ProgramSettings::getVideoSearchPlugin()
{
	return videoSearchPlugin;
}

void ProgramSettings::setVideoSearchPlugin(QString value)
{
	videoSearchPlugin = value;
}

QString ProgramSettings::getVideoSearchKeyWords()
{
	return videoSearchKeyWords;
}

void ProgramSettings::setVideoSearchKeyWords(QString value)
{
	videoSearchKeyWords = value;
}

QString ProgramSettings::getPluginIcon()
{
	return pluginIcon;
}

void ProgramSettings::setPluginIcon(QString value)
{
	pluginIcon = value;
}

QString ProgramSettings::getPluginIconsPath()
{
	return pluginIconsPath;
}

void ProgramSettings::setPluginIconsPath(QString value)
{
	pluginIconsPath = value;
}

bool ProgramSettings::getRecursiveGeneration()
{
	return recursiveGeneration;
}

void ProgramSettings::setRecursiveGeneration(bool value)
{
	recursiveGeneration = value;
}


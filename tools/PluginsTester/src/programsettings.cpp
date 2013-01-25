#include "programsettings.h"

ProgramSettings::ProgramSettings()
{
#ifdef Q_OS_LINUX // modification made by "AzalSup"
	settingsFile = QDir::homePath() + "/.PluginsTester" + SETTINGS_FILE;
#endif
#ifdef Q_WS_MACX
	settingsFile = QDir::homePath() + "/Library/Preferences" + SETTINGS_FILE;
#endif
#ifdef Q_OS_WIN32
	settingsFile = QDir::homePath() + "/PluginsTester" + SETTINGS_FILE;
#endif
	// init defaults
	testingFile = "";
	pluginsDir = "";
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
	settings.setValue("testingFile", testingFile);
	settings.setValue("pluginsDir", pluginsDir);
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
	testingFile = settings.value("settings/testingFile", testingFile).toString();
	pluginsDir = settings.value("settings/pluginsDir", pluginsDir).toString();
}

void ProgramSettings::setTestingFile(QString value)
{
	testingFile = value;
}

QString ProgramSettings::getTestingFile()
{
	return testingFile;
}

void ProgramSettings::setPluginsDir(QString value)
{
	pluginsDir = value;
}

QString ProgramSettings::getPluginsDir()
{
	return pluginsDir;
}

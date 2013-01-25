#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QtCore>

#ifdef Q_WS_MACX
static const QString SETTINGS_FILE = "/com.PluginsTester.config.plist";
#else
static const QString SETTINGS_FILE = "/settings.conf";
#endif

class ProgramSettings : public QObject
{
	private:
		QString settingsFile;
		QString testingFile;
		QString pluginsDir;
	public:
		ProgramSettings();
		void save();
		void load();
		void setTestingFile(QString value);
		QString getTestingFile();
		void setPluginsDir(QString value);
		QString getPluginsDir();
};

#endif // PROGRAMSETTINGS_H

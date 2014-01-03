#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QtCore>

#ifdef Q_OS_MACX
static const QString SETTINGS_FILE = "/com.DebugPlugins.config.plist";
#else
static const QString SETTINGS_FILE = "/settings.conf";
#endif

class ProgramSettings : public QObject
{
	private:
		QString settingsFile;
		QString videoInformationPlugin;
		QString videoInformationURL;
		QString videoSearchPlugin;
		QString videoSearchKeyWords;
		QString pluginIcon;
		QString pluginIconsPath;
		bool recursiveGeneration;
	public:
		ProgramSettings();
		void save();
		void load();
		QString getVideoInformationPlugin();
		void setVideoInformationPlugin(QString value);
		QString getVideoInformationURL();
		void setVideoInformationURL(QString value);
		QString getVideoSearchPlugin();
		void setVideoSearchPlugin(QString value);
		QString getVideoSearchKeyWords();
		void setVideoSearchKeyWords(QString value);
		QString getPluginIcon();
		void setPluginIcon(QString value);
		QString getPluginIconsPath();
		void setPluginIconsPath(QString value);
		bool getRecursiveGeneration();
		void setRecursiveGeneration(bool value);
	};

#endif // PROGRAMSETTINGS_H

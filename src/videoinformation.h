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
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef __VIDEOINFORMATION_H__
#define __VIDEOINFORMATION_H__

#include <QtWidgets>
#include <QtScript>

#ifdef xVST_DEBUG_PLUGINS_ON
#include <QtScriptTools>
#endif

enum BlockedState
{
	bsNotBlocked,
	bsAdultContent,
	bsBlocked
};

#ifdef Q_OS_MACX
static const QString PLUGINS_IMAGE_CACHE_DIR = "/../Caches/xVideoServiceThief/plugins-cache/";
#else
static const QString PLUGINS_IMAGE_CACHE_DIR = "/plugins-cache/";
#endif

class Http;
class VideoItem;
class VideoInformation;
class SearchResults;

struct VideoDefinition;

/*! Stores the plugin information */
class VideoInformationPlugin : public QObject
{
	private:
		VideoInformation *owner;	//!< Plugin owner
		QString scriptCode;			//!< Plugin source code
		QString version;			//!< Plugin version
		QString minVersion;			//!< Min xVST version to run
		QString author;				//!< Plugin author
		QString pluginFilePath;		//!< Where the plugin is stored
		QString website;			//!< Plugin service website (i.e.: http://www.youtube.com/)
		QString ID;					//!< Plugin ID (used by the engine to determine if this plugin knows how to resolve an url)
		QString caption;			//!< Plugin public caption
		QPixmap *icon;				//!< Plugin public icon (acts as a proxy)
		bool adultContent;			//!< Flag for know if this webservice has adult contents
		bool musicSite;				//!< Flag for know if this webservice is a music site (i.e: mp3tube)
		bool loaded;				//!< Flag for know if this plugin has been loaded
		bool hasSearchEngine;		//!< Flag for know if this plugin has a search engine
		QString onlineFaviconUrl;	//!< Online favicon url
		QScriptEngine *engine;		//!< Pointer to script engine
		// debug options
		bool debug;					//!< Flag for know if Debug is ON or OFF
	private:
		/* VideoDefinition struct script definition */
		static QScriptValue create_VideoDefinition(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue toScriptValue_VideoDefinition(QScriptEngine *engine, const VideoDefinition &vd);
		static void fromScriptValue_VideoDefinition(const QScriptValue &obj, VideoDefinition &vd);
		/* Plugins executer */
		static QScriptValue func_isPluginInstalled(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_executePlugin(QScriptContext *context, QScriptEngine *engine);
		/* Plugins core information */
		static QScriptValue func_programVersion(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_programVersionShort(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_programVersionNumber(QScriptContext *context, QScriptEngine *engine);
	public:
		/*! Class constructor */
		VideoInformationPlugin(VideoInformation *videoInformation, QString videoServicePath);
		/*! Class destructor */
		~VideoInformationPlugin();
		/*! Check if the ID passed by parameters is like the stored plugin ID */
		bool isLikeMyId(QString ID);
		/*! Get the video information from URL (this function executes the JS plugin) */
		VideoDefinition getVideoInformation(const QString URL);
		/*! Search Videos and return the results */
		SearchResults searchVideos(const QString keyWords, const int page);
		/*! Abot current work (only if is running) */
		void abortExecution();
		/*! Get the plugin file path or only name */
		QString getScriptFile(const bool onlyName) const;
		/*! Get the plugin version */
		QString getVersion() const;
		/*! Get the min xVST version to run */
		QString getMinVersion() const;
		/*! Get the plugin author */
		QString getAuthor() const;
		/*! Get the plugin ID */
		QString getWebsite() const;
		/*! Get the plugin ID */
		QString getID() const;
		/*! Get the plugin caption */
		QString getCaption() const;
		/*! Get if this service has adult content (+18) */
		bool hasAdultContent() const;
		/*! Get if this service is a music site */
		bool isMusicSite() const;
		/*! Get the plugin icon */
		QPixmap *getIcon() const;
		/*! Get if this plugin use a online favicon */
		bool useOnlineFavicon() const;
		/*! Get the plugin online favicon url */
		QString getFaviconUrl() const;
		/*! Reload icon if is needed */
		void reloadIcon();
		/*! Get if has been loaded */
		bool isLoaded() const;
		/*! Get if has a search engine */
		bool isSearchEngineAvailable() const;
		// debug methods
#ifdef xVST_DEBUG_PLUGINS_ON
		void setDebug(bool value);
#endif
};

/*! Download plugins icons and save them into a cache */
class VideoInformationPluginIconsCatcher : public QObject
{
Q_OBJECT
	private:
		Http *http;		//!< Download class
		VideoInformation *videoInformation;			//!< VideoInformation parent
		QList<VideoInformationPlugin *> *plugins;	//!< List of plugins to download the favicon
	public:
		/*! Class constructor */
		VideoInformationPluginIconsCatcher(VideoInformation *videoInformation);
		/*! Class destructor */
		~VideoInformationPluginIconsCatcher();
		/*! Download needed favicons */
		void downloadFavicons();
	private slots:
		/*! Favicon download finished */
		void downloadFinished(const QFileInfo destFile);
		/*! Favicon download error */
		void downloadError(int error);
		/*! Download the next favicon plugin */
		void downloadNextFavicon();
};

/*! Extended Script engine class */
class ScriptEngineExt : public QScriptEngine
{
	private:
		VideoInformationPlugin *videoInformationPlugin;
	public:
		/*! Class constructor */
		ScriptEngineExt(VideoInformationPlugin *videoInformationPlugin);
		/*! Get the associated video information plugin */
		VideoInformationPlugin *getVideoInformationPlugin();
};

/*! Main video information class */
class VideoInformation : public QThread
{
Q_OBJECT
	private:
		QList<VideoInformationPlugin *> *plugins;	//!< List with all registered plugins (services)
		VideoItem *videoItem;	//!< Current video item
		bool blockAdultContent;	//!< Flag for know if adult content is accepted
		QStringList blockAdultContentList; //!< List of blocked services
		VideoInformationPluginIconsCatcher *faviconsCatcher;	//!< Plugins image downloader
		/*! Determine if this index is a valid item index */
		bool validItemIndex(const int index);
		/*! Clear and destroy all the stored plugins */
		void clearPlugins();
		/*! Thread executation */
		void run();
		/*! Load all plugins from /plugins/.js */
		void loadPlugins(QString pluginsDir);
		/*! Load a plugin and regist it */
		void loadPluginFile(QString scriptFile);
	public:
		/*! Class constructor (load all plugins from "pluginsDir" location) */
		VideoInformation(QString pluginsDir = "");
		/*! Class destructor */
		~VideoInformation();
		/*! Register a new videoInformationPlugin Plugin */
		void registerPlugin(VideoInformationPlugin *videoInformationPlugin);
		/*! Unregister an existent videoInformationPlugin Plugin */
		void unregisterPlugin(VideoInformationPlugin *videoInformationPlugin, bool destroy = true);
		/*! Get the VideoInformationPlugin according to a Host */
		VideoInformationPlugin* getPluginByHost(QUrl URL);
		/*! Get a registered VideoInformationPlugin */
		VideoInformationPlugin* getRegisteredPlugin(const int index);
		/*! Get a registered VideoInformationPlugin by file name (*.js) */
		VideoInformationPlugin* getRegisteredPlugin(const QString fileName, const bool onlyFileName);
		/*! Get a registered VideoInformationPlugin by id */
		VideoInformationPlugin* getRegisteredPlugin(const QString pluginId);
		/*! Get the list of all registered plugins */
		QStringList getPluginsList(bool asCaptions = true);
		/*! Get the list of all registered plugins (host + caption) */
		QStringList getPluginsCompleteList(const QString separator = "|");
		/*! Get the list of all standard sites */
		QList<VideoInformationPlugin*> getAllStandardPlugins() const;
		/*! Get the list of all adult sites */
		QList<VideoInformationPlugin*> getAllAdultPlugins() const;
		/*! Get the list of all music sites */
		QList<VideoInformationPlugin*> getAllMusicPlugins() const;
		/*! Get the list of all plugins with search capabilities */
		QList<VideoInformationPlugin*> getAllSearchPlugins() const;
		/*! Get the list of all standard plugins with search capabilities */
		QList<VideoInformationPlugin*> getAllSearchStandardPlugins() const;
		/*! Get the list of all adult plugins with search capabilities */
		QList<VideoInformationPlugin*> getAllSearchAdultPlugins() const;
		/*! Get the video URL */
		void getVideoInformation(VideoItem *videoItem);
		/*! Get plugins count */
		int pluginsCount() const;
		/*! Get if has plugins loaded */
		bool hasPlugins();
		/*! Abot current work (only if is running) */
		void abortExecution();
		/*! Cancel the current work */
		void cancel();
		/*! Get block adult content flag */
		bool getBlockAdultContent();
		/*! Set the block adult content flag */
		void setBlockAdultContent(bool blockAdultContent);
		/*! Get the block adult content list */
		QString getBlockAdultContentList();
		/*! Set the block adult content list */
		void setBlockAdultContentList(QString hosts);
		/*! Get if is getting the video information */
		bool isGettingInfo();
		/*! Get if can start to get information */
		bool canGetInformation();
		/*! Get the host icon */
		QPixmap getHostImage(QString URL, bool checkURL = false);
		/*! Get the caption of this host */
		QString getHostCaption(QString URL);
		/*! Get if is a valid host */
		bool isValidHost(QString URL);
		/*! Get if is a blocked host (and what block is) */
		bool isBlockedHost(QString URL, BlockedState &result);
		/*! Get if is a blocked host */
		bool isBlockedHost(QString URL);
		/*! Uninstall adult plugins */
		void removeAdultPlugins(bool removeFromDisk);
		/*! Get the last VideoInformation reference */
		static VideoInformation* instance();
	signals:
		/*! Started to get the video information of videoItem */
		void informationStarted(VideoItem *videoItem);
		/*! Finished to get the video information of videoItem */
		void informationFinished(VideoItem *videoItem);
};

#endif // __VIDEOINFORMATION_H__

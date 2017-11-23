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

#ifndef SEARCHVIDEOS_H
#define SEARCHVIDEOS_H

#include <QtCore>

static const QString SEARCH_ID_ALL = "*";		//!< All services id
static const QString SEARCH_ID_STANDARD = "~";	//!< All standard services id
static const QString SEARCH_ID_ADULTS = "!";	//!< All adults services id
static const QString SEARCH_ID_CUSTOM = "?";	//!< Custom services id

#ifdef Q_OS_MACX
static const QString SEARCH_SETTINGS_FILE = "/../Application Support/xVideoServiceThief/searchsettings.plist";
#else
static const QString SEARCH_SETTINGS_FILE = "/searchsettings.conf";
#endif

class Http;
class VideoInformationPlugin;

/*! Search result item which has all the search item information */
class SearchResultItem : public QObject
{
	private:
		QString videoId;		//! Video id
		QString videoUrl;		//! Video url
		QString imageUrl;		//! Video image preview url
		QString title;			//! Video title
		QString description;	//! Video description
		int duration;			//! Video duration
		double rating;			//! Video rating
	public:
		/*! Class constructor */
		SearchResultItem(QString videoUrl, QString imageUrl, QString title,
						 QString description, int duration, double rating);
		/*! Get the video Id */
		QString getVideoId();
		/*! Get the video url */
		QString getVideoUrl();
		/*! Get the video image preview url */
		QString getImageUrl();
		/*! Get the video title */
		QString getTitle();
		/*! Get the video description */
		QString getDescription();
		/*! Get the video duration */
		int getDuration();
		/*! Gt video rating */
		double getRating();
		/*! Get the preview file name */
		QString getPreviewFileName(bool withFullPath = false);
};

/*! Stores the list of search items results */
class SearchResults : public QObject
{
Q_OBJECT
	private:
		QList<SearchResultItem *> *results;	//! List of search results
		QString summary;					//! Search summary
		QString userLanguage;				//! Current user language configuration
		/*! Copy elements from another SearchResults */
		void copyFrom(const SearchResults&, bool clear);
	public:
		/*! Class constructor */
		SearchResults();
		/*! Copy class constructor */
		SearchResults(const SearchResults&);
		/*! Class destructor */
		~SearchResults();
		/*! Override = operator */
		SearchResults &operator=(const SearchResults&);
		/*! Add a new search result */
		void addSearchResult(QString videoUrl, QString imageUrl, QString title,
							 QString description, int duration, double rating);
		/*! Add a list of search results */
		void addSearchResults(const SearchResults &searchResults);
		/*! Get a search result */
		SearchResultItem *getSearchResult(int index);
		/*! Get search results count */
		int getSearchResultCount();
		/*! Set search summary */
		void setSummary(QString value);
		/*! Get search summary */
		QString getSummary();
		/*! Get current user language */
		QString getUserLanguage();
		/*! Remove all search results */
		void removeAllSearchResults();
};

/*! Download the video previews from searches */
class SearchResultsPreviewCatcher : public QObject
{
Q_OBJECT
	private:
		Http *http;							//!< Download class
		QList<SearchResultItem*> *previews;	//!< List of previews to download
	public:
		/*! Class constructor */
		SearchResultsPreviewCatcher();
		/*! Class destructor */
		~SearchResultsPreviewCatcher();
		/*! Add a new  */
		void addPreview(SearchResultItem *searchItem);
		/*! Download previews */
		void downloadPreviews();
		/*! Stop and also clear the downloads queue */
		void stop();
	private slots:
		/*! Preview download finished */
		void downloadFinished(const QFileInfo destFile);
		/*! Preivew download error */
		void downloadError(int error);
		/*! Download the next preview */
		void downloadNextPreview();
	signals:
		/*! Signal send when download preview started */
		void startedDownloadPreview(SearchResultItem *searchItem);
		/*! Signal send when download preview finished */
		void finishedDownloadPreview(SearchResultItem *searchItem, bool error);
};

/*! Save and restore the last search video settings */
class SearchVideosSettings : public QObject
{
	private:
		QStringList pluginsIds;
		QString searchInId;
		/*! Default settings */
		void setDefaults();
	public:
		/*! Save settings */
		void save();
		/*! Load settings */
		void load();
		/*! Set the plugins ids list */
		void setPluginsIds(QStringList value);
		/*! Get the plugins ids list */
		QStringList getPluginsIds();
		/*! Set the search in ID */
		void setSearchInId(QString value);
		/*! Get the search in ID */
		QString getSearchInId();
};

/*! Main threaded search videos */
class SearchVideos : public QThread
{
Q_OBJECT
	private:
		SearchResults *searchResults;				//!< List with search results
		SearchResultsPreviewCatcher *imageCatcher;	//!< Image preview downloader
		QStringList internalPluginsIds;	//!< Plugin id (to identify the plugin to use as searcher)
		QString internalKeyWords;		//!< Text to search
		int internalPage;				//!< Page to display
		bool destroying;				//!< Flag for know if this thread is being destroyed
		/*! Thread executation */
		void run();
		/*! Wait while the updater is running */
		void waitThread();
	public:
		/*! Class constructor */
		SearchVideos();
		/*! Class destructor */
		~SearchVideos();
		/*! Start to search videos */
		void searchVideos(QString keyWords, int page, QStringList pluginsIds);
		/*! Get the search summary */
		QString getSearchSummary();
		/*! Get the key words used for search */
		QString getKeyWords();
		/*! Download the preview images */
		void downloadPreviewImages();
		/*! Clean search images previews */
		static void removePreviews();
	private slots:
		/*! Download preview started */
		void privateStartedDownloadPreview(SearchResultItem *searchItem);
		/*! Download preview finished (w/o errors) */
		void privateFinishedDownloadPreview(SearchResultItem *searchItem, bool error);
	signals:
		/*! New search started */
		void searchStarted();
		/*! Added new search (used on multiple searches) */
		void addNewSearchBlock(VideoInformationPlugin *plugin);
		/*! The current search finished */
		void searchFinished();
		/*! A new search result has been added */
		void searchResultAdded(SearchResultItem *searchResultItem);
		/*! Signal send when download preview started */
		void startedDownloadPreview(SearchResultItem *searchItem);
		/*! Signal send when download preview finished */
		void finishedDownloadPreview(SearchResultItem *searchItem, bool error);
};

#endif // SEARCHVIDEOS_H

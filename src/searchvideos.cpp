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

#include "searchvideos.h"

#include "http.h"
#include "tools.h"
#include "videoinformation.h"
#include "options.h"
#include "languages.h"
#include "options.h"

SearchVideos::SearchVideos()
{
	searchResults = new SearchResults();
	imageCatcher = new SearchResultsPreviewCatcher();
	// inits
	internalKeyWords = "";
	internalPage = 0;
	destroying = false;
	// connect signals
	connect(imageCatcher, SIGNAL(startedDownloadPreview(SearchResultItem*)), this, SLOT(privateStartedDownloadPreview(SearchResultItem*)));
	connect(imageCatcher, SIGNAL(finishedDownloadPreview(SearchResultItem*,bool)), this, SLOT(privateFinishedDownloadPreview(SearchResultItem*,bool)));
}

SearchVideos::~SearchVideos()
{
	disconnect();
	// wait while the thread is running
	destroying = true;
	this->quit();
	waitThread();
	// destroy all
	delete searchResults;
	delete imageCatcher;
}

void SearchVideos::searchVideos(QString keyWords, int page, QStringList pluginsIds)
{
	if (!this->isRunning())
	{
		internalPluginsIds = pluginsIds;
		internalKeyWords = keyWords;
		internalPage = page;
		// start thread
		start();
	}
}

QString SearchVideos::getSearchSummary()
{
	return searchResults->getSummary();
}

QString SearchVideos::getKeyWords()
{
	return internalKeyWords;
}

void SearchVideos::downloadPreviewImages()
{
	imageCatcher->downloadPreviews();
}

void SearchVideos::removePreviews()
{
	QStringList previews;
	QDir previewsDir(QDir::tempPath());
	// get all previews
	previews = previewsDir.entryList(QDir::Files, QDir::Name);
	previews = previews.filter(".xvst.preview");
	// remove all previews from temporary dir
	foreach (QString preview, previews)
		if (preview.endsWith(".xvst.preview"))
			QFile::remove(QDir::tempPath() + "/" + preview);
}

void SearchVideos::privateStartedDownloadPreview(SearchResultItem *searchItem)
{
	emit startedDownloadPreview(searchItem);
}

void SearchVideos::privateFinishedDownloadPreview(SearchResultItem *searchItem, bool error)
{
	emit finishedDownloadPreview(searchItem, error);
}

void SearchVideos::run()
{
	// search started
	emit searchStarted();
	// stop getting previews
	imageCatcher->stop();
	// clear previous results
	searchResults->removeAllSearchResults();
	// has plugins to search?
	if (internalPluginsIds.count() == 0)
	{
		// search finished
		emit searchFinished();
		// abort process
		return;
	}
	// build the plugins search list
	QList<VideoInformationPlugin *> plugins;
	// check which plugins goes into the list
	if (internalPluginsIds.at(0) == SEARCH_ID_ALL) // all plugins
		plugins.append(VideoInformation::instance()->getAllSearchPlugins());
	else if (internalPluginsIds.at(0) == SEARCH_ID_STANDARD) // all standard plugins
		plugins.append(VideoInformation::instance()->getAllSearchStandardPlugins());
	else if (internalPluginsIds.at(0) == SEARCH_ID_ADULTS) // all adult plugins
		plugins.append(VideoInformation::instance()->getAllSearchAdultPlugins());
	else if (internalPluginsIds.count() == 1) // single search
		plugins.append(VideoInformation::instance()->getRegisteredPlugin(internalPluginsIds.at(0)));
	else // custom search, so add them...
		for (int n = 0; n < internalPluginsIds.count(); n++)
			plugins.append(VideoInformation::instance()->getRegisteredPlugin(internalPluginsIds.at(n)));
	// if adult sites are disabled, then remove them from list
	if (VideoInformation::instance()->getBlockAdultContent())
		for (int n = plugins.count() - 1; n >= 0; n--)
			if (plugins.at(n)->hasAdultContent())
				plugins.removeAt(n);
	// inits
	int lastCount = 0;
	// start to search
	while (!plugins.isEmpty())
	{
		// get results
		VideoInformationPlugin *plugin = plugins.takeFirst();
		if (plugin != NULL)
		{
			SearchResults results = plugin->searchVideos(internalKeyWords, internalPage);
			// check if we are destroying the searchResults (if yes, then abort the process)
			if (destroying) return;
			// add results
			searchResults->addSearchResults(results);
			searchResults->setSummary(results.getSummary());
			// add new search block
			emit addNewSearchBlock(plugin);
			// has results?
			if (searchResults->getSearchResultCount() == 0)
			{
				// emit add no results
				emit searchResultAdded(NULL);
				// next search...
				continue;
			}
			// show results
			for (int n = lastCount; n < searchResults->getSearchResultCount(); n++)
			{
				// emit add result
				emit searchResultAdded(searchResults->getSearchResult(n));
				// add to previews download previews
				imageCatcher->addPreview(searchResults->getSearchResult(n));
			}
		}
		// update last count
		lastCount = searchResults->getSearchResultCount();
	}
	// search finished
	emit searchFinished();
}

void SearchVideos::waitThread()
{
	while (this->isRunning())
		qApp->processEvents();
}

// SearchResultsSettings class

void SearchVideosSettings::setDefaults()
{
	searchInId = SEARCH_ID_STANDARD;
}

void SearchVideosSettings::save()
{
	// generate the search settings file path (path + name)
	QString searchSettingsFile = ProgramOptions::instance()->getOptionsPath() + SEARCH_SETTINGS_FILE;
	// init settings file
#ifdef Q_WS_MAC
	QSettings settings(searchSettingsFile, QSettings::NativeFormat);
#else
	QSettings settings(searchSettingsFile, QSettings::IniFormat);
#endif
	settings.beginGroup("lastsettings");
	// assign values
	settings.setValue("pluginsIds", pluginsIds);
	settings.setValue("searchInId", searchInId);
	// save settings
	settings.endGroup();
}

void SearchVideosSettings::load()
{
	// set default values
	setDefaults();
	// generate the search settings file path (path + name)
	QString searchSettingsFile = ProgramOptions::instance()->getOptionsPath() + SEARCH_SETTINGS_FILE;
	// load settings
#ifdef Q_WS_MAC
	QSettings settings(searchSettingsFile, QSettings::NativeFormat);
#else
	QSettings settings(searchSettingsFile, QSettings::IniFormat);
#endif
	// assign values
	pluginsIds = settings.value("lastsettings/pluginsIds", pluginsIds).toStringList();
	searchInId = settings.value("lastsettings/searchInId", searchInId).toString();
}

void SearchVideosSettings::setPluginsIds(QStringList value)
{
	pluginsIds = value;
}

QStringList SearchVideosSettings::getPluginsIds()
{
	return pluginsIds;
}

void SearchVideosSettings::setSearchInId(QString value)
{
	searchInId = value;
}

QString SearchVideosSettings::getSearchInId()
{
	return searchInId;
}

// SearchResultsPreviewCatcher class

SearchResultsPreviewCatcher::SearchResultsPreviewCatcher()
{
	previews = new QList<SearchResultItem*>();
	http = new Http();
	// connect signals
	connect(http, SIGNAL(downloadFinished(const QFileInfo)), this, SLOT(downloadFinished(const QFileInfo)));
	connect(http, SIGNAL(downloadError(int)), this, SLOT(downloadError(int)));
}

SearchResultsPreviewCatcher::~SearchResultsPreviewCatcher()
{
	disconnect();
	// stop and destroy
	stop();
	delete previews;
}

void SearchResultsPreviewCatcher::addPreview(SearchResultItem *searchItem)
{
	if (QFile(searchItem->getPreviewFileName(true)).size() == 0)
		previews->append(searchItem);
}

void SearchResultsPreviewCatcher::downloadPreviews()
{
	// start to download previews
	downloadNextPreview();
}

void SearchResultsPreviewCatcher::stop()
{
	previews->clear();
}

void SearchResultsPreviewCatcher::downloadFinished(const QFileInfo)
{
	if (previews->isEmpty()) return;
	// emit download finished
	emit finishedDownloadPreview(previews->takeFirst(), false);
	// download next preview
	QTimer::singleShot(50, this, SLOT(downloadNextPreview()));
}

void SearchResultsPreviewCatcher::downloadError(int /*error*/)
{
	if (previews->isEmpty()) return;
	// emit download finished with errors
	emit finishedDownloadPreview(previews->takeFirst(), true);
	// download next preview
	downloadNextPreview();
}

void SearchResultsPreviewCatcher::downloadNextPreview()
{
	// has previews to download or an image to download?
	if (!previews->isEmpty())
	{
		emit startedDownloadPreview(previews->first());
		int httpResult = http->download(QUrl(previews->first()->getImageUrl()), QDir::tempPath(), previews->first()->getPreviewFileName(), false);
		// error?
		if (httpResult != EnumHTTP::NO_HTTP_ERROR)
			downloadError(httpResult);
	}
}

// SearchResults class

SearchResults::SearchResults()
{
	results = new QList<SearchResultItem *>();
	// get current user language id
	QString langFile = ProgramOptions::instance()->getLanguageFile(true);
	Language *lang = LanguageManager::getLanguageInfo(langFile);
	userLanguage = lang != NULL ? lang->getLangLocale() : "en";
}

SearchResults::SearchResults(const SearchResults &other) : QObject()
{
	results = new QList<SearchResultItem *>();
	copyFrom(other, true);
	summary = other.summary;
	userLanguage = other.userLanguage;
}

SearchResults::~SearchResults()
{
	removeAllSearchResults();
	delete results;
}

SearchResults &SearchResults::operator=(const SearchResults &other)
{
	copyFrom(other, true);
	summary = other.summary;
	userLanguage = other.userLanguage;
	// return my self updated
	return *this;
}

void SearchResults::copyFrom(const SearchResults &other, bool clear)
{
	// remove old items
	if (clear) removeAllSearchResults();
	// copy all items from "other"
	for (int n = 0; n < other.results->count(); n++)
	{
		SearchResultItem *item = other.results->at(n);
		addSearchResult(item->getVideoUrl(), item->getImageUrl(), item->getTitle(),
						item->getDescription(), item->getDuration(), item->getRating());
	}
}

void SearchResults::removeAllSearchResults()
{
	while (!results->isEmpty())
		delete results->takeFirst();
}

void SearchResults::addSearchResult(QString videoUrl, QString imageUrl, QString title,
									QString description, int duration, double rating)
{
	results->append(new SearchResultItem(videoUrl, imageUrl, title, description, duration, rating));
}

void SearchResults::addSearchResults(const SearchResults &searchResults)
{
	copyFrom(searchResults, false);
}

SearchResultItem* SearchResults::getSearchResult(int index)
{
	if (index >= 0 && index < getSearchResultCount())
		return results->at(index);
	else
		return NULL;
}

int SearchResults::getSearchResultCount()
{
	return results->count();
}

void SearchResults::setSummary(QString value)
{
	summary = multiLineToSingleLine(value);
}

QString SearchResults::getSummary()
{
	return summary;
}

QString SearchResults::getUserLanguage()
{
	return userLanguage;
}

// SearchResultItem class

SearchResultItem::SearchResultItem(QString videoUrl, QString imageUrl, QString title,
									QString description, int duration, double rating)
{
	this->videoUrl = videoUrl.trimmed();
	this->imageUrl = imageUrl.trimmed();
	this->title = title.trimmed();
	this->description = description.trimmed();
	this->duration = duration;
	this->rating = rating;
	// auto-generate the video id (MD5 hash of video URL)
	this->videoId = QCryptographicHash::hash(videoUrl.toAscii(), QCryptographicHash::Md5).toHex();
}

QString SearchResultItem::getVideoId()
{
	return videoId;
}

QString SearchResultItem::getVideoUrl()
{
	return videoUrl;
}

QString SearchResultItem::getImageUrl()
{
	return imageUrl;
}

QString SearchResultItem::getTitle()
{
	return title;
}

QString SearchResultItem::getDescription()
{
	return description;
}

int SearchResultItem::getDuration()
{
	return duration;
}

double SearchResultItem::getRating()
{
	return rating;
}

QString SearchResultItem::getPreviewFileName(bool withFullPath)
{
	if (withFullPath)
		return QDir::tempPath() + "/" + videoId + ".xvst.preview";
	else
		return "/" + videoId + ".xvst.preview";
}

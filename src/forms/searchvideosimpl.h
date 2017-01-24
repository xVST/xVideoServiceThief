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

#ifndef SEARCHVIDEOSIMPL_H
#define SEARCHVIDEOSIMPL_H

#include <QtCore>

#include "ui_searchvideos.h"

class SearchVideos;
class SearchResultItem;
class SearchVideosItemImpl;
class VideoInformationPlugin;

class SearchVideosImpl : public QDialog, private Ui::SearchVideosImpl
{
Q_OBJECT
	private:
		SearchVideos *searchVideos;
		QStringList customServices;
		QList<SearchVideosItemImpl*> getSearchVideosItemImplBySearchItem(SearchResultItem *item);
		QMovie *loadingMovie;
		void clearResults();
		void updateButons(bool searching);
		void fillSearchServices();
		void centerWindow();
		void restoreLastSearchSettings();
		void saveLastSearchSettings();
	public:
		SearchVideosImpl(QWidget *parent = 0, Qt::WindowFlags f = 0);
		~SearchVideosImpl();
	private slots:
		void finished(int);
		void cmbSearchInActivated(int index);
		void btnSearchClicked();
		void btnPrevSearchClicked();
		void btnNextSearchClicked();
		void edtKeyWordChanged(QString text);
		void searchStarted();
		void addNewSearchBlock(VideoInformationPlugin *plugin);
		void searchFinished();
		void searchResultAdded(SearchResultItem *searchResultItem);
		void startedDownloadPreview(SearchResultItem *searchItem);
		void finishedDownloadPreview(SearchResultItem *searchItem, bool error);
};

#endif // SEARCHVIDEOSIMPL_H

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

#ifndef SEARCHVIDEOSITEMIMPL_H
#define SEARCHVIDEOSITEMIMPL_H

#include "ui_searchvideositem.h"

class SearchResultItem;

class SearchVideosItemImpl : public QWidget, private Ui::SearchVideosItemImpl
{
Q_OBJECT
	private:
		SearchResultItem *searchItem;
		bool hasPreview;
		bool canBeDownloaded;
		QString toStars(double rating);
	public:
		SearchVideosItemImpl(QWidget *parent, SearchResultItem *searchItem, QMovie *loadingMovie);
		void setSearchVideosItem(SearchResultItem *searchItem);
		void reloadPreview();
	private slots:
		void linkActivated(const QString &link);
};

#endif // SEARCHVIDEOSITEMIMPL_H

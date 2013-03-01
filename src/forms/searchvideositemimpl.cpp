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

#include "searchvideositemimpl.h"

#include <math.h>

#include "../tools.h"
#include "../searchvideos.h"
#include "../videolistcontroller.h"
#include "../videoinformation.h"

SearchVideosItemImpl::SearchVideosItemImpl(QWidget *parent, SearchResultItem *searchItem, QMovie *loadingMovie)
	: QWidget(parent)
{
	setupUi(this);
	// configure fonts
	QFont titleFont = lblTitle->font();
	QFont descriptionFont = lblDescription->font();
	QFont timeAndRatingFont = lblDuration->font();
	titleFont.setBold(true);
	// set MacOSX and Linux points sizes
#ifndef Q_OS_WIN32
	descriptionFont.setPointSize(descriptionFont.pointSize() - 1);
	timeAndRatingFont.setPointSize(timeAndRatingFont.pointSize() - 2);
#else // set windows points sizes
	titleFont.setPointSize(titleFont.pointSize() + 1);
	timeAndRatingFont.setPointSize(timeAndRatingFont.pointSize() - 1);
#endif
	lblTitle->setFont(titleFont);
	lblDescription->setFont(descriptionFont);
	lblDuration->setFont(timeAndRatingFont);
	lblRating->setFont(timeAndRatingFont);
	// configure
	setSearchVideosItem(searchItem);
	hasPreview = QFile(searchItem->getPreviewFileName(true)).size() > 0;
	// set the preview if it exists
	if (hasPreview) reloadPreview(); else imgVideo->setMovie(loadingMovie);
	// connect signals
	connect(lblDownloadVideo, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)));
}

void SearchVideosItemImpl::setSearchVideosItem(SearchResultItem *searchItem)
{
	if (searchItem == NULL) return;

	this->searchItem = searchItem;
	this->setObjectName(searchItem->getVideoId());

	canBeDownloaded = VideoInformation::instance()->isValidHost(searchItem->getVideoUrl());

	// auto configure items
	lblTitle->setText(QString(lblTitle->text()).arg(searchItem->getTitle()));
	lblDescription->setText(QString(lblDescription->text()).arg(searchItem->getDescription()));

	imgVideo->setToolTip(tr("Play <b>%1</b>").arg(searchItem->getTitle()));

	lblPlayVideo->setText(QString("<a href=\"%1\"><img src=\":/buttons/images/control_play.png\" /></a>").arg(searchItem->getVideoUrl()));

	lblDuration->setText(QString("(%1)").arg(secondsToHMS_formatted(searchItem->getDuration(), ":", true, false)));
	lblDuration->setVisible(searchItem->getDuration() > 0);

	imgRating->setText(toStars(searchItem->getRating()));
	imgRating->setToolTip(tr("<b>Rating:</b> %1").arg(floatToStr(searchItem->getRating())));
	imgRating->setVisible(searchItem->getRating() > 0);
	lblRating->setVisible(searchItem->getRating() > 0);

	if (canBeDownloaded)
		lblDownloadVideo->setText(QString("<a href=\"%1\"><img src=\":/buttons/images/film_add.png\" /></a>").arg(searchItem->getVideoUrl()));
	else // download not available
	{
		lblDownloadVideo->setText("<a href=\"%1\"><img src=\":/services/images/services/invalid.png\" /></a>");
		lblDownloadVideo->setToolTip(tr("Download not available..."));
	}
}

void SearchVideosItemImpl::reloadPreview()
{
	if (searchItem == NULL) return;
	// remove loading image
	imgVideo->clear();
	// make it screachable
	imgVideo->setScaledContents(true);
	// init preview image
	QPixmap preview;
	QString previewSrc;
	// reload preview if it exists
	if (QFile::exists(searchItem->getPreviewFileName(true)))
		preview.load(searchItem->getPreviewFileName(true));
	// loaded?
	previewSrc = preview.isNull() ? ":/search/images/no-preview.png" : searchItem->getPreviewFileName(true);
	// load image
	imgVideo->setText(QString("<a href='%1'><img src='%2' /></a>").arg(searchItem->getVideoUrl()).arg(previewSrc));
	// yes, has preview now
	hasPreview = true;
}

void SearchVideosItemImpl::linkActivated(const QString &link)
{
	if (!VideoListController::instance()->isAlreadyAdded(link))
	{
		// check if this video can be downloaded
		if (canBeDownloaded)
			VideoListController::instance()->addVideo(link);
		else // ops, its not possible add this video
			native_alert(this, QMessageBox::Information, tr("Missing plugin"),
						 tr("<p>Is not possible to download this video.</p><p>The xVST didn't find any plugin capable to get the video information.</p>"),
						 tr("Ok"));
	}
	else // already added
		native_alert(this, QMessageBox::Information, tr("Already added"),
					 tr("You already added this video. Check your downloads list."), tr("Ok"));
}

QString SearchVideosItemImpl::toStars(double rating)
{
	// rating 0..5
	QString stars = "";
	int count = floor(rating);
	// set stars
	for (int n = 0; n < count; n++)
		stars+="<img src=\":/search/images/star_full.png\" />";
	// need an half star?
	if (count < 5 && (rating - floor(rating)) >= 0.5)
		stars+="<img src=\":/search/images/star_half.png\" />";
	else if (count < 5) // need some empty stars
		for (int n = count; n < 5; n++)
			stars+="<img src=\":/search/images/star_empty.png\" />";
	return stars;
}

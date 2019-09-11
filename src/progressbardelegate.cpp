/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2018 Xesc & Technology
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
* Program URL   : https://www.xvideothief.com/
*
*/

#include "progressbardelegate.h"
#include "xprogressbar.h"

#include "videolistcontroller.h"

ProgressBarDelegate::ProgressBarDelegate(QObject *parent, VideoListController *videoList) : QItemDelegate(parent)
{
	this->videoList = videoList;
}

void ProgressBarDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option,
                                const QModelIndex & index ) const
{
	if (index.column() == 2)
	{
		// get the video item
		bool converted;
		VideoItem *videoItem = videoList->getVideoItemByID(index.data(Qt::UserRole).toInt(&converted));
		// check if this item exists
		if (videoItem != nullptr)
		{
			if (videoItem->isDownloading() || videoItem->isConverting() ||
				videoItem->isAnyKindOfPaused() || videoItem->isResuming())
			{
				// paint the progress bar
				xProgressBar progressBar(option.rect, painter);
				// set the color schema
				progressBar.setColorSchema(
					videoItem->isDownloading() ? 0 :		// downloading progress bar
					videoItem->isConverting() ? 8 :			// converting progress bar
					videoItem->isAnyKindOfPaused() ? 9 :	// paused progress bar
					4);										// resuming progress bar
				// display a text only, for paused and resuming videos
				progressBar.setDisplayText(videoItem->isAnyKindOfPaused() || videoItem->isResuming());
				// set a custom text, only for paused and resuming videos
				if (videoItem->isAnyKindOfPaused() || videoItem->isResuming())
					progressBar.setText(videoItem->getVideoStateAsString());
				// set progressbar value
				progressBar.setValue(videoItem->getProgress());
				// set the vertical span and paint the progressBar
				progressBar.setVerticalSpan(1);
				progressBar.paint();
				// ok
				return;
			}
		}
	}
	// default painting
	QItemDelegate::paint(painter, option, index);
	return;
}

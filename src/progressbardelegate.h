/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

#ifndef __PROGRESSBARDELEGATE_H__
#define __PROGRESSBARDELEGATE_H__

#include <QtWidgets>

class VideoListController;

class ProgressBarDelegate : public QItemDelegate
{
Q_OBJECT
	private:
		VideoListController *videoList;
	public:
		ProgressBarDelegate(QObject *parent = 0, VideoListController *videoList = NULL);
		void paint(QPainter * painter, const QStyleOptionViewItem & option,
			const QModelIndex & index ) const;
};

#endif // __PROGRESSBARDELEGATE_H__

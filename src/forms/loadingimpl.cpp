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

#include "loadingimpl.h"

static LoadingImpl* loadingImplInstance = NULL;

LoadingImpl::LoadingImpl(QWidget *parent) :
	QWidget(parent)
{
    setupUi(this);
	QWidget::setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint);


	QDesktopWidget *desktop = QApplication::desktop();

	int screenWidth, width;
	int screenHeight, height;
	int x, y;
	QSize windowSize;

	screenWidth = desktop->width();
	screenHeight = desktop->height();

	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();

	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;

	move(x, y);
}

LoadingImpl* LoadingImpl::instance()
{
	if (loadingImplInstance == NULL) loadingImplInstance = new LoadingImpl;
	return loadingImplInstance;
}

void LoadingImpl::setMessage(QString message)
{
	lblMessage->setText("<font color=white>" + message + "</font>");
}

void LoadingImpl::finished()
{
	QTimer::singleShot(400, this, SLOT(closeLoading()));
}

void LoadingImpl::closeLoading()
{
	close();
	// destroy instance
	delete loadingImplInstance;
	loadingImplInstance = NULL;
}

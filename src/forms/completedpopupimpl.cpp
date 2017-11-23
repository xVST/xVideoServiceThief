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

#include "completedpopupimpl.h"

#include "../tools.h"

CompletedPopupImpl::CompletedPopupImpl(QSystemTrayIcon *trayIconRef, QWidget * parent, Qt::WindowFlags f)
		: QWidget(parent, f)
{
	setupUi(this);
	// set window flags
	QWidget::setWindowFlags(Qt::ToolTip | Qt::WindowStaysOnTopHint);
	// init the timer
	displayTimer = new QTimer(this);
	connect(displayTimer, SIGNAL(timeout()), this, SLOT(displayTimerOnTimer()));
	// connect buttons
	connect(spbClose, SIGNAL(clicked()), this, SLOT(closeClicked()));
	connect(spbPlayVideo, SIGNAL(clicked()), this, SLOT(playVideoClicked()));
	connect(trayIconRef, SIGNAL(messageClicked()), this, SLOT(playVideoClicked()));
	// init state
	displayState = dsInactive;
	// get a reference to the tray icon
	this->trayIconRef = trayIconRef;
}

CompletedPopupImpl::~CompletedPopupImpl()
{
	delete displayTimer;
}

void CompletedPopupImpl::displayTimerOnTimer()
{
	if (!isVisible()) return;

	QDesktopWidget *desktop = QApplication::desktop();
	QRect availableGeometry  = desktop->availableGeometry(this);

	// display popup animation
	if (displayState == dsShowing)
		if (pos().x() > availableGeometry.width() - size().width())// - 15)
			move(pos().x() - 2, pos().y());
		else
		{
			displayState = dsWaiting;
			displayTimer->start(5000);
		}
	// hide popup animation
	else if (displayState == dsHiding)
		if (pos().x() < availableGeometry.width())
			move(pos().x() + 2, pos().y());
		else
		{
			displayState = dsWaiting;
			displayTimer->stop();
			hide();
		}
	else if (displayState == dsWaiting)
	{
		displayState = dsHiding;
		displayTimer->start(2);
	}
}

void CompletedPopupImpl::displayPopup(const QIcon &serviceIcon, const QString videoTitle, const QString videoFile)
{
#ifdef Q_OS_MACX
	// check if growl is installed
	if (QSystemTrayIcon::supportsMessages())
	{
		trayIconRef->showMessage(tr("Download completed"), videoTitle, QSystemTrayIcon::Information, 5000);
		// don't display the standard message
		return;
	}
#endif
	// set window info
	imgVideoService->hide();
	lblVideoTitle->hide();
	imgVideoService->setPixmap(serviceIcon.pixmap(QSize(16,16)));
	lblVideoTitle->setText(videoTitle);
	imgVideoService->show();
	lblVideoTitle->show();

	this->videoFile = videoFile;

	QDesktopWidget *desktop = QApplication::desktop();
	QRect availableGeometry  = desktop->availableGeometry(this);
	move(desktop->width(), availableGeometry.height() - size().height());
	this->show();

	alpha = 0;

	displayState = dsShowing;
	displayTimer->start(2);
}

void CompletedPopupImpl::closeClicked()
{
	displayState = dsHiding;
	displayTimer->start(2);
}

void CompletedPopupImpl::playVideoClicked()
{
	if (QFile::exists(videoFile))
		openFile(videoFile);
}
//

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
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef COMPLETEDPOPUPIMPL_H
#define COMPLETEDPOPUPIMPL_H

#include <QtWidgets>

#include "ui_completedpopup.h"

enum DisplayState {dsInactive, dsShowing, dsWaiting, dsHiding};

class CompletedPopupImpl : public QWidget, public Ui::CompletedPopup
{
Q_OBJECT
	private:
		QSystemTrayIcon *trayIconRef;
		QTimer *displayTimer;
		QString videoFile;
		DisplayState displayState;
		double alpha;
	public:
		CompletedPopupImpl(QSystemTrayIcon *trayIconRef, QWidget * parent = 0, Qt::WindowFlags f = 0);
		~CompletedPopupImpl();
		void displayPopup(const QIcon &, const QString, const QString);
	private slots:
		void displayTimerOnTimer();
		void closeClicked();
		void playVideoClicked();
};
#endif

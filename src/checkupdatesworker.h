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

#ifndef CHECKUPDATESWORKER_H
#define CHECKUPDATESWORKER_H

#include <QtWidgets>

static const QString URL_UPDATE_FILE = "http://xviservicethief.sourceforge.net/updates/xVST2.update";

class Updates;
class ProgramOptions;

class CheckUpdatesWorker : public QObject
{
Q_OBJECT
	private:
		Updates *updates;				//!< Responsable of check and download updates
		ProgramOptions *programOptions;	//!< Program options instance
		QLabel *caption;				//!< Caption which will display the current action
		QProgressBar *progressBar;		//!< ProgressBar used for represent the current progress
		QAbstractButton *cancelButton;	//!< Button for cancel current updates check
		QWidget *parentForm;			//!< Form which called this class
		bool closedByButton;			//!< Flag for know if has been closed by button or automatically
		bool isUser;					//!< Flag for know if this class has been called automatically or not
		/*! Wait while the updater is running */
		void waitThread();
	public:
		/*! Class constructor */
		CheckUpdatesWorker(ProgramOptions *programOptions, QWidget *parentForm, QLabel *caption,
						   QProgressBar *progressBar, QAbstractButton *cancelButton, bool isUser);
		/*! Class destructor */
		~CheckUpdatesWorker();
		/*! Start check updates */
		void checkUpdates();
	private slots:
		/*! After check for updates */
		void updatesChecked(bool hasUpdates);
		/*! Update proces cancelled */
		void cancelButtonClicked();
		/*! On updates has been cancelled */
		void updatesCancelled();
		/*! On progress update */
		void progressCheckUpdate(int progress);
		/*! On update message */
		void updateMessage(QString version, QString url);
	signals:
		/*! On finish */
		void finished(bool done, bool closedByButton);
		/*! Before display the update center window */
		void beforeDisplayUpdateCenter();
};

#endif // CHECKUPDATESWORKER_H

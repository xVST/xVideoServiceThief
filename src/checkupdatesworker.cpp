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

#include "checkupdatesworker.h"

#include "forms/updatecenterimpl.h"
#include "forms/windowsvistadetectedimpl.h"

#include "updates.h"
#include "tools.h"
#include "options.h"

CheckUpdatesWorker::CheckUpdatesWorker(ProgramOptions *programOptions, QWidget *parentForm, QLabel *caption,
									   QProgressBar *progressBar, QAbstractButton *cancelButton, bool isUser)
	: QObject()
{
	this->programOptions = programOptions;
	this->parentForm = parentForm;
	this->caption = caption;
	this->progressBar = progressBar;
	this->cancelButton = cancelButton;
	this->isUser = isUser;
	// init updater
	updates = new Updates(programOptions->getApplicationPath());
	// signals
	connect(updates, SIGNAL(progressCheckUpdate(int)), this, SLOT(progressCheckUpdate(int)));
	connect(updates, SIGNAL(updatesChecked(bool)), this, SLOT(updatesChecked(bool)));
	connect(updates, SIGNAL(updatesCancelled()), this, SLOT(updatesCancelled()));
	connect(updates, SIGNAL(updateMessage(QString, QString)), this, SLOT(updateMessage(QString, QString)));
	//
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
}

CheckUpdatesWorker::~CheckUpdatesWorker()
{
	delete updates;
}

void CheckUpdatesWorker::checkUpdates()
{
	programOptions->setLastUpdate(QDate::currentDate());
	programOptions->save();

	updates->checkForUpdates(URL_UPDATE_FILE);
}

void CheckUpdatesWorker::waitThread()
{
	while (updates->isRunning())
	{
		//qApp->processEvents(); // this causes a terrorific crash xD
	}
}

void CheckUpdatesWorker::updatesChecked(bool hasUpdates)
{
	// if has updates, then display the update center
	if (hasUpdates)
	{
		//this->setVisible(false);
		// result checker
		int result = QDialog::Accepted;
		// check if we are running windwos vista
		if (isWindowsVista() && !programOptions->getVistaUpdatesMessage())
		{
			WindowsVistaDetectedImpl windowsVistaDetectedForm(parentForm);
			result = windowsVistaDetectedForm.exec();
			// update the option "don't display this message"
			programOptions->setVistaUpdatesMessage(windowsVistaDetectedForm.chbDontDisplay->isChecked());
		}
		// open update center (this condition works only on windows vista)
		if (result == QDialog::Accepted)
		{
			emit beforeDisplayUpdateCenter();
			// dispaly
			UpdateCenterImpl updateCenterForm(updates, programOptions->getInstallAutomaticallyUpdates(), parentForm);
			result = updateCenterForm.exec();
		}
		// wait until "thread end"
		waitThread();
		// updates downloaded??? yes? then install them
		if (result == QDialog::Accepted)
		{
			updates->installUpdates();
			QApplication::closeAllWindows();
		}
		else // update center has been cancelled
		{
			closedByButton = true;
			//done(QDialog::Rejected);
			emit finished(false, closedByButton);
		}
	}
	else
	{
		// wait thread end
		waitThread();
		// display no updates message (only if is an user request)
		if (isUser)
			native_alert(parentForm, QMessageBox::Information, tr("Updates"), tr("You are using the most recent version of this program."),	tr("Ok"));
		// set as closed by button
		closedByButton = true;
		// close
		emit finished(false, closedByButton);
	}
}

void CheckUpdatesWorker::updateMessage(QString version, QString url)
{
	// wait thread end
	waitThread();
	// display special updates message
	native_alert(parentForm, QMessageBox::Information, tr("Update Center"),
				 tr("Please, download the new version of <b>xVideoServiceThief (%1)</b> from <a href='%2'>here.</a>").arg(version).arg(url), tr("Ok"));
	// set as closed by button
	closedByButton = true;
	// close
	emit finished(false, closedByButton);
}

void CheckUpdatesWorker::cancelButtonClicked()
{
	cancelButton->setEnabled(false);
	updates->cancel();
}

void CheckUpdatesWorker::updatesCancelled()
{
	caption->setText(tr("Cancelling... please wait..."));

	waitThread();

	closedByButton = true;
	//done(QDialog::Rejected);
	emit finished(false, closedByButton);
}

void CheckUpdatesWorker::progressCheckUpdate(int progress)
{
	caption->setText(tr("Checking for updates..."));
	progressBar->setMaximum(100);
	progressBar->setValue(progress);

	// imposible cancel it
	if (progress == 100)
		cancelButton->setEnabled(false);
}

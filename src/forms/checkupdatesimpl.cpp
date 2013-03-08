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

#include "checkupdatesimpl.h"

#include "../options.h"
#include "../checkupdatesworker.h"

CheckUpdatesImpl::CheckUpdatesImpl(ProgramOptions *programOptions, QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// resize form if is needed
#ifdef Q_OS_MACX
	resize(width(), 132);
#endif
	//
	closedByButton = false;
	// init check updates worker
	checkUpdatesWorker = new CheckUpdatesWorker(programOptions, this, lblUpdating, pbrUpdate, btnCancel, true);
	connect(checkUpdatesWorker, SIGNAL(finished(bool, bool)), this, SLOT(checkUpdatesWorkerFinished(bool, bool)));
	connect(checkUpdatesWorker, SIGNAL(beforeDisplayUpdateCenter()), this, SLOT(beforeDisplayUpdateCenter()));
	checkUpdatesWorker->checkUpdates();
}

CheckUpdatesImpl::~CheckUpdatesImpl()
{
	delete checkUpdatesWorker;
}

void CheckUpdatesImpl::checkUpdatesWorkerFinished(bool hasUpdates, bool closedByButton)
{
	this->closedByButton = closedByButton;
	this->done(hasUpdates ? QDialog::Accepted : QDialog::Rejected);
}

void CheckUpdatesImpl::beforeDisplayUpdateCenter()
{
	this->setVisible(false);
}
//

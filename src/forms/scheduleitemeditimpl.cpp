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

#include "scheduleitemeditimpl.h"

#include "../tools.h"

ScheduleItemEditImpl::ScheduleItemEditImpl(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
    setupUi(this);
	// resize window (macosx and linux)
#ifdef Q_OS_MACX
	resize(356, 200);
#endif
	// connect signals
	connect(btnOk, SIGNAL(clicked()), this, SLOT(btnOkClicked()));
}

void ScheduleItemEditImpl::btnOkClicked()
{
	if (timerStart->time() > timerEnd->time())
		native_alert(this, QMessageBox::Warning,
					 tr("Invalid schedule configuration"),
					 tr("The start time cannot exceed the end time."),
					 tr("Ok"));
	else if (timerStart->time() == timerEnd->time())
		native_alert(this, QMessageBox::Warning,
					 tr("Invalid schedule configuration"),
					 tr("The start time and end time cannot be equals."),
					 tr("Ok"));
	else // ok
		accept();
}

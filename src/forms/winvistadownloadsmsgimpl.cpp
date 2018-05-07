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

#include "winvistadownloadsmsgimpl.h"

#include "../options.h"

WinVistaDownloadsMsgImpl::WinVistaDownloadsMsgImpl(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// set the deault directory
	directoryVideosClicked(true);
	// connect signals
	connect(rdBtnDownloadsDir, SIGNAL(clicked(bool)), this, SLOT(directoryDownloadsClicked(bool)));
	connect(rdBtnVideosDir, SIGNAL(clicked(bool)), this, SLOT(directoryVideosClicked(bool)));
	connect(spbSelectDownloadDir, SIGNAL(clicked()), this, SLOT(selectDownloadDirClicked()));
}

bool WinVistaDownloadsMsgImpl::isInProgramFiles()
{
	QString programFiles = QString(getenv("ProgramFiles"));
	// is program files?
	return ProgramOptions::instance()->getDownloadDir().indexOf(programFiles, 0, Qt::CaseInsensitive) == 0;
}

bool WinVistaDownloadsMsgImpl::getDisplayAgain()
{
	return chbDontDisplay->isChecked();
}

QString WinVistaDownloadsMsgImpl::getDownloadsDir()
{
	return edtDirectory->text();
}

void WinVistaDownloadsMsgImpl::directoryDownloadsClicked(bool)
{
	edtDirectory->setText(QDir::toNativeSeparators(QDir::homePath() + "/Downloads/xVST"));
}

void WinVistaDownloadsMsgImpl::directoryVideosClicked(bool)
{
	edtDirectory->setText(QDir::toNativeSeparators(QDir::homePath() + "/Videos/xVST"));
}

void WinVistaDownloadsMsgImpl::selectDownloadDirClicked()
{
	QString s = QFileDialog::getExistingDirectory(this, tr("Select the download directory:"),
	            edtDirectory->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	// if is emtpy, cancel the proces
	if (s.isEmpty()) return;
	edtDirectory->setText(QDir::toNativeSeparators(s));
}

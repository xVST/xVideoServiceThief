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

#include "bugreportimpl.h"

#include "infoviewimpl.h"

#include "../tools.h"
#include "../trackerreport.h"
#include "../programversion.h"
#include "../videoitem.h"
#include "../videoinformation.h"
#include "../options.h"

BugReportImpl::BugReportImpl(ProgramOptions *programOptions, QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
#ifndef Q_OS_WIN32
	resize(540, 650);
#endif
	// program options
	this->programOptions = programOptions;
	// hide "send group"
	line_2->hide();
	lblSending->hide();
	prbSending->hide();
	// set program version
	lblxVSTVersion->setText(QString(lblxVSTVersion->text()).arg(PROGRAM_VERSION));
	// configure tracker
	trackerReport = new TrackerReport();
	// signals
	connect(spbViewInfo, SIGNAL(clicked()), this, SLOT(viewInfoClicked()));
	connect(btnSend, SIGNAL(clicked()), this, SLOT(sendReportClicked()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	connect(trackerReport, SIGNAL(trackerReportSent(QString)), this, SLOT(trackerReportSent(QString)));
}

BugReportImpl::~BugReportImpl()
{
	programOptions->setDisplayBugReport(!chbNoErrors->isChecked());
	
	delete trackerReport;
}

void BugReportImpl::fillErrorInfo(VideoItem *videoItem, VideoInformation *videoInformation)
{
	this->videoItem = videoItem;
	this->videoInformation = videoInformation;
	// set visual info
	lblVideoURL->setText(QString(lblVideoURL->text()).arg(videoItem->getURL()));
}

QString BugReportImpl::getPluginInformation(QString URL)
{
	VideoInformationPlugin *plugin = videoInformation->getPluginByHost(QUrl(URL));
	// exists?
	if (plugin != NULL)
		return QString("%1 (%2)").arg(plugin->getScriptFile(true)).arg(plugin->getVersion());
	else
		return "-";
}

void BugReportImpl::viewInfoClicked()
{
	// fill text
	QStringList info;
	info	<< "<pre>"
			<< "{<br />"
			<< "   <b>'code'</b>: '" + QString("%1").arg(videoItem->getErrorCode()) + "',<br />"
			<< "   <b>'url'</b>: '" + videoItem->getURL() + "',<br />"
			<< "   <b>'app_version'</b>: '" + PROGRAM_VERSION + "',<br />"
			<< "   <b>'os_version'</b>: '" + CURRENT_OS + "',<br />"
			<< "   <b>'plugin'</b>: '" + getPluginInformation(videoItem->getURL()) + "',<br />"
			<< "   <b>'user_name'</b>: '" + edtName->text().replace("'", "\\'") + "',<br />"
			<< "   <b>'user_email'</b>: '" + edtEmail->text().replace("'", "\\'") + "',<br />"
			<< "   <b>'user_comments'</b>: '" + rchComments->toPlainText().replace("'", "\\'") + "'<br />"
			<< "}<br />"
			<< "</pre>";

	QString infoStr;
	foreach (QString line, info) infoStr += line;

	InfoViewImpl infoView(infoStr, this);
	infoView.exec();
}

void BugReportImpl::sendReportClicked()
{
	btnSend->setEnabled(false);
	btnCancel->setEnabled(false);
	// display "send group"
	line_2->show();
	lblSending->show();
	prbSending->show();
	// fill text
	QStringList info;
	info	<< "{"
			<< "	'code': '" + QString("%1").arg(videoItem->getErrorCode()) + "',\n"
			<< "	'url': '" + videoItem->getURL() + "',\n"
			<< "	'app_version': '" + PROGRAM_VERSION + "',\n"
			<< "	'os_version': '" + CURRENT_OS + "',\n"
			<< "	'plugin': '" + getPluginInformation(videoItem->getURL()) + "',\n"
			<< "	'user_name': '" + edtName->text().replace("'", "\\'") + "',\n"
			<< "	'user_email': '" + edtEmail->text().replace("'", "\\'") + "',\n"
			<< "	'user_comments': '" + rchComments->toPlainText().replace("'", "\\'") + "'\n"
			<< "}";

	QString infoStr;
	foreach (QString line, info) infoStr += line;

	trackerReport->sendTrackerReport(infoStr);
}

void BugReportImpl::cancelClicked()
{
	done(QDialog::Rejected);
}

void BugReportImpl::trackerReportSent(QString result)
{
	line_2->hide();
	lblSending->hide();
	prbSending->hide();
	
	if (result == "OK")
	{
		native_alert(this, QMessageBox::Information, "xVideoServiceThief Tracker", tr("Thank you for your report."), tr("Ok"));
	}
	else // error
	{
		native_alert(this, QMessageBox::Warning, "xVideoServiceThief Tracker", result, tr("Ok"));
	}

	done(QDialog::Accepted);
}
//

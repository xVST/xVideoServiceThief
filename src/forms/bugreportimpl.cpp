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
	trackerReport = new TrackerReport("205061", "1037803");
	// add category
	trackerReport->addCategory("None", "100");
	// add groups
	trackerReport->addGroup("1.0a",   "804496");
	trackerReport->addGroup("1.1a",   "804497");
	trackerReport->addGroup("1.2a",   "804498");
	trackerReport->addGroup("1.3a",   "804499");
	trackerReport->addGroup("1.4a",   "804500");
	trackerReport->addGroup("1.5a",   "804501");
	trackerReport->addGroup("1.6a",   "807976");
	trackerReport->addGroup("1.6.1a", "809584");
	trackerReport->addGroup("1.7a",   "813251");
	trackerReport->addGroup("1.7.1a", "814954");
	trackerReport->addGroup("1.8a",   "820796");
	trackerReport->addGroup("1.8.1a", "822016");
	trackerReport->addGroup("1.8.2a", "825062");
	trackerReport->addGroup("2.0.0a", "898961");
	trackerReport->addGroup("2.0.1a", "899073");
	trackerReport->addGroup("2.1",    "910835");
	trackerReport->addGroup("2.2",    "930083");
	trackerReport->addGroup("2.3",    "971205");
	trackerReport->addGroup("2.3.1",  "1009922");
	trackerReport->addGroup("2.3.2",  "1025880");
	trackerReport->addGroup("2.3.3",  "1047982");
	trackerReport->addGroup("2.3.4",  "1057713");
	trackerReport->addGroup("2.3.5",  "1073045");
	trackerReport->addGroup("2.4",    "1099410");
	trackerReport->addGroup("2.4.1",  "1238437");
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
	InfoViewImpl infoView(videoItem, getPluginInformation(videoItem->getURL()), edtName->text(),
						  edtEmail->text(), rchComments->toPlainText(), this);
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
	// resize form
	// fill text
	QStringList info;
	info 	<< "Video Information:\n"
			<< "Error Code: " + QString("%1").arg(videoItem->getErrorCode()) + "\n"
			<< "Video URL: " + videoItem->getURL() + "\n"
			<< "xVST Version: " + PROGRAM_VERSION + " (" + CURRENT_OS + ")" + "\n"
			<< "Plugin Version: " + getPluginInformation(videoItem->getURL()) + "\n"
			<< "FLV URL: " + videoItem->getVideoInformation().URL + "\n"
			<< "FLV Title: " + videoItem->getVideoInformation().title + "\n\n"
			<< "Sender Information:\n"
			<< "User name: " + edtName->text() + "\n"
			<< "User eMail: " + edtEmail->text() + "\n"
			<< "User Comments: " + rchComments->toPlainText();

	QString infoStr;
	foreach (QString line, info)
		infoStr += line;

	QString videoServiceName = videoInformation->getHostCaption(videoItem->getURL());

	trackerReport->sendTrackerReport("None", PROGRAM_VERSION_SHORT, 
									 QString("xVST: %1").arg(videoServiceName), 
									 QString(infoStr).replace("&", "%26"));
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
	
	if (result.isEmpty())
	{
		native_alert(this, QMessageBox::Information, "SourceForge.net Tracker", tr("Thank you for your report."), tr("Ok"));
	}
	else // display the web message
	{
		QString msg1 = copy(result, 0, result.indexOf("<p>")).trimmed();
		QString msg2 = copyBetween(result, "<p>", "<br>").trimmed();
		QString msg3 = copyBetween(result, "<a href='", "'>").trimmed();
		//
		native_alert(this, QMessageBox::Information, QString("SourceForge.net Tracker: %1").arg(msg1),
					 QString("%1 <a href=\"%2\">%2</a>").arg(msg2).arg(msg3), tr("Ok"));
	}
	done(QDialog::Accepted);
}
//

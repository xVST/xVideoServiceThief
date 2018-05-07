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

#include "optionsimpl.h"

#include "downloadlogimpl.h"
#include "checkupdatesimpl.h"
#include "newlanguagesimpl.h"
#include "scheduleitemeditimpl.h"
#include "upspasswordimpl.h"

#include "../tools.h"
#include "../options.h"
#include "../videolistcontroller.h"
#include "../sessionmanager.h"
#include "../videoinformation.h"
#include "../videoconvert.h"
#include "../languages.h"
#include "../schedule.h"
#include "../updates.h"

OptionsImpl::OptionsImpl(ProgramOptions *programOptions, SessionManager *sessionManager,
			 VideoListController *videoList, int lastOptionsPage,
			 QWidget * parent, Qt::WindowFlags f)
		: QDialog(parent, f)
{
	setupUi(this);
	this->programOptions = programOptions;
	this->sessionManager = sessionManager;
	this->videoList = videoList;
	this->videoInformation = videoList->getVideoInformation();
	this->lastPageViewed = lastOptionsPage;
	languageManager = new LanguageManager;
	schedule = new ScheduleController(programOptions->getOptionsPath(), programOptions->getOptionsFormat());
	schedule->load();
	// resize form if is needed
#ifndef Q_OS_WIN32
	resize(865, 500);
	// resize menu
	trvMenu->setMaximumWidth(180);
#endif
#ifdef Q_OS_MACX
	// change lsvServices1 & lsvServices2 alternateBase color (better look&feel)
	QPalette palette = lsvServices1->palette();
	QColor color("#efefef");
	palette.setColor(QPalette::Disabled, QPalette::AlternateBase, color);
	palette.setColor(QPalette::Active, QPalette::AlternateBase, color);
	palette.setColor(QPalette::Inactive, QPalette::AlternateBase, color);
	palette.setColor(QPalette::Disabled, QPalette::Base, palette.base().color());
	lsvServices1->setPalette(palette);
	lsvServices2->setPalette(palette);
	lsvSchedules->setPalette(palette);
#endif
	// prepare schedules columns
	QStringList headers;
	headers << tr(" Enabled ") << tr(" Start time ") << tr(" End time ") << tr("Comments");
	// add the schedule headers
	lsvSchedules->setHeaderLabels(headers);
	// set aligns to schedule headers
	lsvSchedules->headerItem()->setTextAlignment(0, Qt::AlignHCenter | Qt::AlignVCenter);
	lsvSchedules->headerItem()->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
	lsvSchedules->headerItem()->setTextAlignment(2, Qt::AlignHCenter | Qt::AlignVCenter);
	//signals
	connect(btnOk, SIGNAL(clicked()), this, SLOT(btnOkClicked())); //btn Ok (clicked)
	connect(spbSelectDownloadDir, SIGNAL(clicked()), this, SLOT(spbSelectDownloadDirPressed()));
	connect(spbSelectFFmpegLib, SIGNAL(clicked()), this, SLOT(spbSelectFFmpegLibPressed()));
	connect(edtFFmpegLib, SIGNAL(textChanged(const QString &)), this, SLOT(ffmpegTextChanged(const QString &)));
	connect(btnViewLog, SIGNAL(clicked()), this, SLOT(btnViewLogClicked()));
	connect(btnClearLog, SIGNAL(clicked()), this, SLOT(btnClearLogClicked()));
	connect(chbSaveRestore, SIGNAL(clicked(bool)), this, SLOT(chbSaveRestoreStateClicked(bool)));
	connect(spbRemove, SIGNAL(clicked()), this, SLOT(spbRemovePressed()));
	connect(spbRemoveAll, SIGNAL(clicked()), this, SLOT(spbRemoveAllPressed()));
	connect(spbAddAll, SIGNAL(clicked()), this, SLOT(spbAddAllPressed()));
	connect(spbAdd, SIGNAL(clicked()), this, SLOT(spbAddPressed()));
	connect(lsvServices1, SIGNAL(itemSelectionChanged()), this, SLOT(servicesItemSelectionChanged()));
	connect(lsvServices2, SIGNAL(itemSelectionChanged()), this, SLOT(servicesItemSelectionChanged()));
	connect(lsvLanguages, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(langCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(btnUseThis, SIGNAL(clicked()), this, SLOT(btnUseThisClicked()));
	connect(lsvLanguages, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(langItemDoubleClicked(QTreeWidgetItem *, int)));
	connect(btnCheckNow, SIGNAL(clicked()), this, SLOT(btnCheckNowClicked()));
	connect(btnAddNewLanguages, SIGNAL(clicked()), this, SLOT(btnAddNewLanguagesClicked()));
	connect(chbInternalFFmpeg, SIGNAL(stateChanged(int)), this, SLOT(internalFFmpegStateChanged(int)));
	connect(chbDownloadVideosAuto, SIGNAL(clicked(bool)), this, SLOT(chbDownloadVideosAutoClicked(bool)));
	connect(chbScheduleEnabled, SIGNAL(clicked(bool)), this, SLOT(chbScheduleEnabledClicked(bool)));
	connect(spbAddSchedule, SIGNAL(clicked()), this, SLOT(spbAddNewSchedulePressed()));
	connect(spbEditSchedule, SIGNAL(clicked()), this, SLOT(spbEditSchedulePressed()));
	connect(spbRemoveSchedule, SIGNAL(clicked()), this, SLOT(spbRemoveSchedulePressed()));
	connect(lsvSchedules, SIGNAL(itemSelectionChanged()), this, SLOT(lsvSchedulesItemSelectionChanged()));
	connect(lsvSchedules, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(lsvSchedulesDoubleClicked(QModelIndex)));
	connect(chbDisableAdultSupport, SIGNAL(clicked(bool)), this, SLOT(chbDisableAdultSupportClicked(bool)));
	connect(chbProxySystemConfig, SIGNAL(stateChanged(int)), this, SLOT(chbProxySystemConfigStateChanged(int)));
	// create menu
	createMenu();
	// add info
	fillInitialData();
	// add languages
	fillLanguages();
	// add schedules
	fillSchedules();
	// add plugins
	fillPluginsLists();
	// set values
	setInitialOptionsValues();
	// can update?
	btnCheckNow->setEnabled(Updates::canUpdate(programOptions->getToolsPath())); //getOptionsPath()));
	// internal FFmpeg...
#ifdef Q_OS_MACX
	// check if internal ffmpeg is installed
	if (!programOptions->getIfInternalFFmpegIsInstalled())
	{
		chbInternalFFmpeg->hide();
	}
	else// update ffmpeg path and select button
	{
		// if use internal ffmpeg then, hide path and button
		if (programOptions->getUseInternalFFmpeg())
		{
			edtFFmpegLib->hide();
			spbSelectFFmpegLib->hide();
		}
	}
#else
	// hide "chbInternalFFmpeg"
	chbInternalFFmpeg->hide();
#endif
}

OptionsImpl::~OptionsImpl()
{
	delete schedule;
	delete languageManager;
}

int OptionsImpl::getLastPageViewed()
{
	return lastPageViewed;
}

void OptionsImpl::createMenu()
{
	// hide the menu header
	trvMenu->header()->hide();
	// connect slots
	connect(trvMenu, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(menuCurrentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	// add the menu items
	QTreeWidgetItem *newItem;

	// basic
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Basic"));
	newItem->setIcon(0, QIcon(":/options/images/tools.png"));
	newItem->setSelected(true);

	// language
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Language"));
	newItem->setIcon(0, QIcon(":/options/images/language.png"));

	// session
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Session"));
	newItem->setIcon(0, QIcon(":/options/images/session.png"));

	// updates
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Updates"));
	newItem->setIcon(0, QIcon(":/options/images/updates.png"));

	// ups!
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Ups!"));
	newItem->setIcon(0, QIcon(":/options/images/ups.png"));

	// Schedule
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Schedule"));
	newItem->setIcon(0, QIcon(":/options/images/schedule.png"));

	// proxy
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Internet configuration"));
	newItem->setIcon(0, QIcon(":/options/images/proxy.png"));

	// Errors
	newItem = new QTreeWidgetItem(trvMenu);
	newItem->setText(0, tr("Tracker"));
	newItem->setIcon(0, QIcon(":/options/images/errorReport.png"));

	if (lastPageViewed != -1)
	{
		pgOptions->setCurrentIndex(lastPageViewed);
		trvMenu->setCurrentItem(trvMenu->topLevelItem(lastPageViewed));
	}
	chbRememberView->setChecked(lastPageViewed != -1);
}

void OptionsImpl::fillInitialData()
{
	// fill conversion options
	cmbOutputFormat->addItems(VideoConverter::getOutputFormatAsStrings());
	cmbVideoResolution->addItems(VideoConverter::getVideoResolutionAsStrings());
	cmbAudioSampling->addItems(VideoConverter::getAudioSampleRatioAsStrings());
	cmbVideoFrameRate->addItems(VideoConverter::getVideoFrameRateAsStrings());
	cmbOutputQuality->addItems(VideoConverter::getOutputQualityAsStrings());

	QStringList itemsToAdd;
	// set items to update every
	itemsToAdd.clear();
	itemsToAdd	<< tr("Day") << tr("2 Days") << tr("3 Days") << tr("4 Days")
				<< tr("5 Days") << tr("6 Days") << tr("Week");
	cmbUpdateEvery->addItems(itemsToAdd);

	// time remaining calculation items
	itemsToAdd.clear();
	itemsToAdd << tr("Using the download percentage") << tr("Using the download speed");
	cmbTimeRemaining->addItems(itemsToAdd);

	// set proxy type items
	itemsToAdd.clear();
	itemsToAdd << tr("Http Proxy") << tr("Socks5 Proxy");
	cmbProxyType->addItems(itemsToAdd);
}

void OptionsImpl::fillLanguages()
{
	lsvLanguages->clear();

	tmpLangFile = programOptions->getLanguageFile(false);
	languageManager->loadLangFiles(programOptions->getLanguagesPath());
	lsvLanguages->header()->hide();

	for (int n = 0; n < languageManager->getLanguagesCount(); n++)
	{
		QTreeWidgetItem *newItem = new QTreeWidgetItem(lsvLanguages);
		newItem->setText(0, languageManager->getLanguage(n)->getId());
		newItem->setSizeHint(0, QSize(18,18));
		QFileInfo langFile(languageManager->getLanguage(n)->getFile());
		if (langFile.fileName() == tmpLangFile)
		{
			newItem->setIcon(0, QIcon(":/buttons/images/script_go.png"));
			// set as bold
			QFont oriFont = newItem->font(0);
			oriFont.setBold(true);
			newItem->setFont(0, oriFont);
			// select it
			lsvLanguages->setCurrentItem(newItem);
		}
		else
			newItem->setIcon(0, QIcon(":/buttons/images/script.png"));
	}
}

void OptionsImpl::fillSchedules()
{
	for(int n = 0; n < schedule->getSchedulesCount(); n++)
	{
		ScheduleItem *scheduleItem = schedule->getScheduleItem(n);
		QTreeWidgetItem *treeItem = new QTreeWidgetItem(lsvSchedules);
		treeItem->setText(0, YesNoToString(scheduleItem->isEnabled()));
		treeItem->setText(1, scheduleItem->getStartTime().toString("hh:mm"));
		treeItem->setText(2, scheduleItem->getEndTime().toString("hh:mm"));
		treeItem->setText(3, scheduleItem->getComment());
		// set texts alignments
		treeItem->setTextAlignment(0, Qt::AlignHCenter | Qt::AlignVCenter);
		treeItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
		treeItem->setTextAlignment(2, Qt::AlignHCenter | Qt::AlignVCenter);
	}
}

void OptionsImpl::fillPluginsLists()
{
	// set plugins list
	lsvServices1->header()->hide();
	lsvServices2->header()->hide();
	QStringList plugins = videoInformation->getPluginsCompleteList();

	for (int n = 0; n < plugins.count(); n++)
	{
		QStringList info = plugins.at(n).split("|");

		QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices1);
		item->setData(0, Qt::UserRole, info.at(0)); // store the host id
		item->setText(0, info.at(1)); // caption
		item->setIcon(0, QIcon(videoInformation->getHostImage(info.at(0))));
		item->setSizeHint(0, QSize(18,18));
	}
	servicesItemSelectionChanged();
	showAdultSites(!programOptions->getBlockAdultContent());
}

void OptionsImpl::setInitialOptionsValues()
{
	edtDownloadsDir->setText(programOptions->getDownloadDir());
	chbDisplayPopup->setChecked(programOptions->getDisplayPopup());
	spinBoxMaxDownloads->setValue(programOptions->getMaxActiveDownloads());

	edtFFmpegLib->setText(programOptions->getFfmpegLibLocation());

	gpbVideoConversion->setChecked(programOptions->getConvertVideos());
	cmbOutputFormat->setCurrentIndex(programOptions->getConversionConf().outputFormat);
	cmbVideoResolution->setCurrentIndex(programOptions->getConversionConf().videoResolution);
	cmbAudioSampling->setCurrentIndex(programOptions->getConversionConf().audioSampleRatio);
	cmbVideoFrameRate->setCurrentIndex(programOptions->getConversionConf().videoFrameRate);
	cmbOutputQuality->setCurrentIndex(programOptions->getConversionConf().outputQuality);
	chbDeleteOriginalVideo->setChecked(programOptions->getDeleteVideosOnConvert());

	chbSaveRestore->setChecked(programOptions->getSaveRestoreSessions());
	chbDontrestoreDownloads->setChecked(programOptions->getDontRememberDownloadedVideos());
	chbSaveLog->setChecked(programOptions->getSaveLogDownloadedVideos());

	chbDontrestoreDownloads->setEnabled(chbSaveRestore->isChecked());

	chbDisableAdultSupport->setChecked(programOptions->getBlockAdultContent());

	// get the blocked items
	QStringList hosts = programOptions->getBlockedWebsitesList().split("|", QString::SkipEmptyParts);
	for (int i = 0; i < lsvServices1->topLevelItemCount(); ++i)
		if (hosts.indexOf(lsvServices1->topLevelItem(i)->data(0, Qt::UserRole).toString(), 0) != -1)
			lsvServices1->topLevelItem(i)->setSelected(true);
	spbAddPressed();

	chbProxySystemConfig->setChecked(programOptions->getUseSystemProxyConfig());
	gpbProxy->setChecked(programOptions->getUseProxy());
	edtUserName->setText(programOptions->getProxyUserName());
	edtPassword->setText(programOptions->getProxyPassword());
	edtAddress->setText(programOptions->getProxyAddress());
	spinBoxPort->setValue(programOptions->getProxyPort());

	if (programOptions->getProxyType() != QNetworkProxy::HttpProxy &&
	    programOptions->getProxyType() != QNetworkProxy::Socks5Proxy)
		cmbProxyType->setCurrentIndex(0);
	else
		cmbProxyType->setCurrentIndex(programOptions->getProxyType());

	chbCheckOnStartup->setChecked(programOptions->getCheckForUpdatesOnStartup());
	cmbUpdateEvery->setCurrentIndex(programOptions->getCheckForUpdatesEvery() - 1);
	chbInstallAutomatically->setChecked(programOptions->getInstallAutomaticallyUpdates());

	gpbDisplayErrorReport->setChecked(programOptions->getDisplayBugReport());

	chbInternalFFmpeg->setChecked(programOptions->getUseInternalFFmpeg());

	chbDownloadVideosAuto->setChecked(programOptions->getDownloadAutomatically());
	chbScheduleEnabled->setChecked(programOptions->getScheduleEnabled());
	chbDownloadVideosAutoClicked(chbDownloadVideosAuto->isChecked());

	chbDeleteFileOnError->setChecked(programOptions->getDeleteFailedDownloads());
	chbTimeOut->setChecked(programOptions->getEnableTimeOut());
	spinBoxTimeOut->setValue(programOptions->getTimeOut());
	spinBoxMaxRetries->setValue(programOptions->getMaxRetries());
}

void OptionsImpl::setOptionsValues()
{
	programOptions->setCanSendUpdateSignal(false);

	programOptions->setDownloadDir(edtDownloadsDir->text());
	programOptions->setDisplayPopup(chbDisplayPopup->isChecked());
	programOptions->setMaxActiveDownloads(spinBoxMaxDownloads->value());

	programOptions->setFfmpegLibLocation(edtFFmpegLib->text());

	programOptions->setConvertVideos(gpbVideoConversion->isChecked());
	VideoConversionConfig convConf;
	convConf.outputFormat = static_cast<OutputFormat>(cmbOutputFormat->currentIndex());
	convConf.videoResolution = static_cast<VideoResolution>(cmbVideoResolution->currentIndex());
	convConf.audioSampleRatio = static_cast<AudioSampleRatio>(cmbAudioSampling->currentIndex());
	convConf.videoFrameRate = static_cast<VideoFrameRate>(cmbVideoFrameRate->currentIndex());
	convConf.outputQuality = static_cast<OutputQuality>(cmbOutputQuality->currentIndex());
	programOptions->setConversionConf(convConf);
	programOptions->setDeleteVideosOnConvert(chbDeleteOriginalVideo->isChecked());

	programOptions->setSaveRestoreSessions(chbSaveRestore->isChecked());
	programOptions->setDontRememberDownloadedVideos(chbDontrestoreDownloads->isChecked());
	programOptions->setSaveLogDownloadedVideos(chbSaveLog->isChecked());

	QString hosts = "";
	for (int i = 0; i < lsvServices2->topLevelItemCount(); ++i)
		hosts += "|" + lsvServices2->topLevelItem(i)->data(0, Qt::UserRole).toString();

	programOptions->setBlockAdultContent(chbDisableAdultSupport->isChecked());
	programOptions->setBlockedWebsitesList(hosts);

	programOptions->setUseSystemProxyConfig(chbProxySystemConfig->isChecked());
	programOptions->setUseProxy(gpbProxy->isChecked());
	programOptions->setProxyUserName(edtUserName->text());
	programOptions->setProxyPassword(edtPassword->text());
	programOptions->setProxyAddress(edtAddress->text());
	programOptions->setProxyPort(spinBoxPort->value());
	programOptions->setProxyType(cmbProxyType->currentIndex());

	programOptions->setCanSendUpdateSignal(true);

	programOptions->setLanguageFile(tmpLangFile);

	programOptions->setCheckForUpdatesOnStartup(chbCheckOnStartup->isChecked());
	programOptions->setCheckForUpdatesEvery(cmbUpdateEvery->currentIndex() + 1);
	programOptions->setInstallAutomaticallyUpdates(chbInstallAutomatically->isChecked());

	lastPageViewed = chbRememberView->isChecked() ? pgOptions->currentIndex() : -1;

	programOptions->setDisplayBugReport(gpbDisplayErrorReport->isChecked());

	programOptions->setUseInternalFFmpeg(chbInternalFFmpeg->isChecked());

	programOptions->setDownloadAutomatically(chbDownloadVideosAuto->isChecked());
	programOptions->setScheduleEnabled(chbScheduleEnabled->isChecked());
	schedule->save();

	programOptions->setDeleteFailedDownloads(chbDeleteFileOnError->isChecked());
	programOptions->setEnableTimeOut(chbTimeOut->isChecked());
	programOptions->setTimeOut(spinBoxTimeOut->value());
	programOptions->setMaxRetries(spinBoxMaxRetries->value());
}

void OptionsImpl::dragEnterEvent(QDragEnterEvent* /*event*/)
{
	qDebug() << "dragEnterEvent";
}

void OptionsImpl::dropEvent(QDropEvent* /*event*/)
{
	qDebug() << "dropEvent";
}

QString OptionsImpl::YesNoToString(bool value)
{
	return value ? tr("Yes") : tr("No");
}

void OptionsImpl::hideAdultSitesFromList(QTreeWidget *list, bool visible)
{
	for (int n = 0; n < list->topLevelItemCount(); n++)
	{
		QString pluginId = list->topLevelItem(n)->data(0, Qt::UserRole).toString();
		VideoInformationPlugin *plugin = videoInformation->getRegisteredPlugin(pluginId);
		// exists?
		if (plugin != NULL)
		{
			if (plugin->hasAdultContent() && !visible)
				list->topLevelItem(n)->setHidden(true);
			else // make it visible
				list->topLevelItem(n)->setHidden(false);
		}
	}
}

void OptionsImpl::showAdultSites(bool visible)
{
	hideAdultSitesFromList(lsvServices1, visible);
	hideAdultSitesFromList(lsvServices2, visible);
	// disable speed buttons
	spbAdd->setEnabled(false);
	spbAddAll->setEnabled(false);
	spbRemove->setEnabled(false);
	spbRemoveAll->setEnabled(false);
}

void OptionsImpl::chbSaveRestoreStateClicked(bool checked)
{
	chbDontrestoreDownloads->setEnabled(checked);
}

void OptionsImpl::chbDownloadVideosAutoClicked(bool checked)
{
	chbScheduleEnabled->setEnabled(checked);
	chbScheduleEnabledClicked(chbScheduleEnabled->isChecked() && checked);
}

void OptionsImpl::chbScheduleEnabledClicked(bool checked)
{
	lsvSchedules->setEnabled(checked);
	spbAddSchedule->setEnabled(checked);
	spbEditSchedule->setEnabled(checked);
	spbRemoveSchedule->setEnabled(checked);
	// update button sates
	if (checked) lsvSchedulesItemSelectionChanged();
}

void OptionsImpl::spbAddNewSchedulePressed()
{
	ScheduleItemEditImpl addScheduleForm(this, Qt::Sheet);
	if (showModalDialog(&addScheduleForm) == QDialog::Accepted)
	{
		schedule->addNewSchedule(addScheduleForm.chbActive->isChecked(),
								 addScheduleForm.timerStart->time(),
								 addScheduleForm.timerEnd->time(),
								 addScheduleForm.edtComment->text());
		QTreeWidgetItem *item = new QTreeWidgetItem(lsvSchedules);
		item->setText(0, YesNoToString(addScheduleForm.chbActive->isChecked()));
		item->setText(1, addScheduleForm.timerStart->time().toString("hh:mm"));
		item->setText(2, addScheduleForm.timerEnd->time().toString("hh:mm"));
		item->setText(3, addScheduleForm.edtComment->text());
		// set text alignments
		item->setTextAlignment(0, Qt::AlignHCenter | Qt::AlignVCenter);
		item->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
		item->setTextAlignment(2, Qt::AlignHCenter | Qt::AlignVCenter);
	}
}

void OptionsImpl::spbEditSchedulePressed()
{
	// get ScheduleItem
	ScheduleItem *scheduleItem = schedule->getScheduleItem(lsvSchedules->currentIndex().row());
	// display edit form
	ScheduleItemEditImpl addScheduleForm(this, Qt::Sheet);
	// prepare window
	addScheduleForm.timerStart->setTime(scheduleItem->getStartTime());
	addScheduleForm.timerEnd->setTime(scheduleItem->getEndTime());
	addScheduleForm.edtComment->setText(scheduleItem->getComment());
	addScheduleForm.chbActive->setChecked(scheduleItem->isEnabled());
	// show modal
	if (showModalDialog(&addScheduleForm) == QDialog::Accepted)
	{
		// update schedule info
		scheduleItem->setStartTime(addScheduleForm.timerStart->time());
		scheduleItem->setEndTime(addScheduleForm.timerEnd->time());
		scheduleItem->setComment(addScheduleForm.edtComment->text());
		scheduleItem->setEnabled(addScheduleForm.chbActive->isChecked());
		// update visual info
		QTreeWidgetItem *item = lsvSchedules->currentItem();
		item->setText(0, YesNoToString(scheduleItem->isEnabled()));
		item->setText(1, scheduleItem->getStartTime().toString("hh:mm"));
		item->setText(2, scheduleItem->getEndTime().toString("hh:mm"));
		item->setText(3, scheduleItem->getComment());
	}
}

void OptionsImpl::spbRemoveSchedulePressed()
{
	if (native_alert(this, QMessageBox::Question, tr("Remove schedule item"), tr("Wish you remove the selected schedule item?"), tr("Yes"), tr("No")) == 0)
	{
		schedule->removeSchedule(lsvSchedules->currentIndex().row());
		delete lsvSchedules->currentItem();
	}
}

void OptionsImpl::lsvSchedulesItemSelectionChanged()
{
	bool enabled = lsvSchedules->currentItem() != NULL;
	spbEditSchedule->setEnabled(enabled);
	spbRemoveSchedule->setEnabled(enabled);
}

void OptionsImpl::lsvSchedulesDoubleClicked(QModelIndex)
{
	spbEditSchedulePressed();
}

void OptionsImpl::chbDisableAdultSupportClicked(bool checked)
{
	UPSPasswordImpl upsPasswordForm(this, Qt::Sheet);

	// if we want UNLOCK
	if (!checked)
	{
		// change window texts
		upsPasswordForm.lblTitle->setText(tr("<b>Enter the security password to unlock adult sites.</b>"));
		upsPasswordForm.lblPassword->setText(tr("Unlocking password:"));
		// display password dialog and wait for user selection
		if (showModalDialog(&upsPasswordForm) == QDialog::Accepted)
		{
            QString password_MD5 = QCryptographicHash::hash(upsPasswordForm.edtPassword->text().toLocal8Bit(), QCryptographicHash::Md5).toHex();
			// compare passwords
			if (password_MD5 != programOptions->getBlockAdultContentPassword())
			{
				native_alert(this, QMessageBox::Critical, tr("Invalid UPS! password"),
							 tr("You entered an invalid UPS! password and the adults contents will continue locked."), tr("Ok"));
				// check again this option...
				chbDisableAdultSupport->setChecked(true);
			} // password is ok
		}
		else // user did cancel (so check this option again)
			chbDisableAdultSupport->setChecked(true);
	}
	else // entering new password
	{
		// display password dialog and wait for user selection
		if (showModalDialog(&upsPasswordForm) == QDialog::Accepted)
		{
            QString password_MD5 = QCryptographicHash::hash(upsPasswordForm.edtPassword->text().toLocal8Bit(), QCryptographicHash::Md5).toHex();
			programOptions->setBlockAdultContentPassword(password_MD5);
		}
		else
			chbDisableAdultSupport->setChecked(false);
	}
	// update visible sites
	showAdultSites(!chbDisableAdultSupport->isChecked());
}

void OptionsImpl::chbProxySystemConfigStateChanged(int state)
{
	gpbProxy->setEnabled(state == Qt::Unchecked);
}

void OptionsImpl::menuCurrentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	pgOptions->setCurrentIndex(trvMenu->indexOfTopLevelItem(current));
	// we have a current ietm?
	if (current != NULL)
	{
		QFont oriFont = current->font(0);
		oriFont.setBold(true);
		current->setFont(0, oriFont);
	}
	// we have a prevous item?
	if (previous != NULL)
	{
		QFont oriFont = previous->font(0);
		oriFont.setBold(false);
		previous->setFont(0, oriFont);
	}
}

void OptionsImpl::langCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* /*previous*/)
{
	if (current != NULL)
	{
		Language *language =languageManager->getLanguage(lsvLanguages->indexOfTopLevelItem(current));

		lblLanguage->setText(language->getId());
		lblVersion->setText(language->getVersion());
		lblAuthor->setText(language->getAuthor());
		lblContact->setText(QString("<a href=\"%1\">%2</a>").arg(language->getContact()).arg(language->getContact()));
		rchDescription->setHtml(language->getDescription());

		btnUseThis->setEnabled(language->getFile() != tmpLangFile);
	}
}

void OptionsImpl::btnUseThisClicked()
{
	Language *language =languageManager->getLanguage(lsvLanguages->indexOfTopLevelItem(lsvLanguages->currentItem()));
	tmpLangFile = language->getFile();
	btnUseThis->setEnabled(false);

	for (int n = 0; n < languageManager->getLanguagesCount(); n++)
	{
		Language *language = languageManager->getLanguage(n);
		QTreeWidgetItem *item = lsvLanguages->topLevelItem(n);

		if (language->getFile() == tmpLangFile)
			item->setIcon(0, QIcon(":/buttons/images/script_go.png"));
		else
			item->setIcon(0, QIcon(":/buttons/images/script.png"));
		// set as bold?
		QFont oriFont = item->font(0);
		oriFont.setBold(language->getFile() == tmpLangFile);
		item->setFont(0, oriFont);
	}
	// disply update message
	native_alert(this, QMessageBox::Information, tr("Language Setup"),
				 tr("In order to apply the new selected language, the program must be restarted."), tr("Ok"));
}

void OptionsImpl::langItemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/)
{
	if (btnUseThis->isEnabled())
		btnUseThisClicked();
}

void OptionsImpl::btnCheckNowClicked()
{
	if (videoList->isWorking())
	{
		native_alert(this, QMessageBox::Information, tr("Updates"),
					 tr("Another process is currently working, please stop it or wait until the end of process."),
					 tr("Ok"));
		return;
	}

	btnCheckNow->setEnabled(false);

	CheckUpdatesImpl checkUpdatesForm(programOptions, this, Qt::Sheet);
	checkUpdatesForm.exec();

	btnCheckNow->setEnabled(true);
}

void OptionsImpl::btnAddNewLanguagesClicked()
{
#ifdef Q_OS_MACX
	NewLanguagesImpl *newLanguagesForm = new NewLanguagesImpl(programOptions, this, Qt::Sheet);
	newLanguagesForm->show();
	// capture when sheet is closed
	connect(newLanguagesForm, SIGNAL(accepted()), this, SLOT(newLanguagesAccepted()));
#else // windows and linux
	NewLanguagesImpl newLanguagesForm(programOptions, this);
	newLanguagesForm.exec();
	// reload languages
	fillLanguages();
#endif
}

void OptionsImpl::newLanguagesAccepted()
{
	// destroy sender (NewLanguagesImpl)
	delete this->sender();
	// update languages list
	fillLanguages();
}

void OptionsImpl::internalFFmpegStateChanged(int state)
{
	if (!chbInternalFFmpeg->isVisible()) return;

	// disable ffmpeg controls
	edtFFmpegLib->setVisible(state == Qt::Unchecked);
	spbSelectFFmpegLib->setVisible(state == Qt::Unchecked);

	// auto-set ffmpeg path if its checked
	if (chbInternalFFmpeg->isChecked())
		edtFFmpegLib->setText(programOptions->getFfmpegLibLocation());
}

void OptionsImpl::btnOkClicked()
{
	setOptionsValues();
	done(QDialog::Accepted);
}

void OptionsImpl::spbSelectDownloadDirPressed()
{
	QString s = QFileDialog::getExistingDirectory(this,
												  tr("Select the download directory:"),
												  edtDownloadsDir->text(),
												  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	// if is emtpy, cancel the proces
	if (s.isEmpty()) return;
	edtDownloadsDir->setText(QDir::toNativeSeparators(s));
}

void OptionsImpl::spbSelectFFmpegLibPressed()
{
	QString s = QFileDialog::getOpenFileName(this, tr("Select the ffmpeg lib:"),
		    edtFFmpegLib->text(),
#ifdef Q_OS_WIN32
		    "ffmpeg lib (ffmpeg.exe)");
#else
		    "ffmpeg lib (ffmpeg)");
#endif
	// if is emtpy, cancel the proces
	if (s.isEmpty()) return;
	edtFFmpegLib->setText(QDir::toNativeSeparators(s));
}

void OptionsImpl::ffmpegTextChanged(const QString &text)
{
	QColor exists(qApp->palette().brush(QPalette::Base).color());
	QColor notExist(255, 170, 127);

	QPalette p = edtFFmpegLib->palette();
	p.setColor(QPalette::Base, QFile::exists(text.trimmed()) ? exists : notExist);
	edtFFmpegLib->setPalette(p);
}

void OptionsImpl::btnViewLogClicked()
{
	DownloadLogImpl downloadLog(videoList, this);
	downloadLog.buildLog(sessionManager->getLogEntries(), videoInformation);
	downloadLog.exec();
}

void OptionsImpl::btnClearLogClicked()
{
	if (native_alert(this, QMessageBox::Question, tr("Clear Log"), tr("Are you sure to clear the downloads/conversions historic file?"), tr("Yes"), tr("No")) == 0)
		sessionManager->clearLog();
}

void OptionsImpl::spbRemovePressed()
{
	for (int i = lsvServices2->topLevelItemCount() - 1; i >= 0; --i)
		if (lsvServices2->topLevelItem(i)->isSelected())
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices1);
			item->setData(0, Qt::UserRole, lsvServices2->topLevelItem(i)->data(0, Qt::UserRole));
			item->setText(0, lsvServices2->topLevelItem(i)->text(0));
			item->setIcon(0, lsvServices2->topLevelItem(i)->icon(0));
			item->setSizeHint(0, QSize(18,18));

			delete lsvServices2->topLevelItem(i);
		}
	// update event
	servicesItemSelectionChanged();
}

void OptionsImpl::spbRemoveAllPressed()
{
	for (int i = 0; i < lsvServices2->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices1);
		item->setData(0, Qt::UserRole, lsvServices2->topLevelItem(i)->data(0, Qt::UserRole));
		item->setText(0, lsvServices2->topLevelItem(i)->text(0));
		item->setIcon(0, lsvServices2->topLevelItem(i)->icon(0));
		item->setSizeHint(0, QSize(18,18));
	}
	lsvServices2->clear();
	lsvServices1->sortItems(0, Qt::AscendingOrder);
	servicesItemSelectionChanged();
	showAdultSites(!chbDisableAdultSupport->isChecked());
}

void OptionsImpl::spbAddAllPressed()
{
	for (int i = 0; i < lsvServices1->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices2);
		item->setData(0, Qt::UserRole, lsvServices1->topLevelItem(i)->data(0, Qt::UserRole));
		item->setText(0, lsvServices1->topLevelItem(i)->text(0));
		item->setIcon(0, lsvServices1->topLevelItem(i)->icon(0));
		item->setSizeHint(0, QSize(18,18));
	}
	lsvServices1->clear();
	lsvServices2->sortItems(0, Qt::AscendingOrder);
	servicesItemSelectionChanged();
	showAdultSites(!chbDisableAdultSupport->isChecked());
}

void OptionsImpl::spbAddPressed()
{
	for (int i = lsvServices1->topLevelItemCount() - 1; i >= 0; --i)
		if (lsvServices1->topLevelItem(i)->isSelected())
		{
			QTreeWidgetItem *item = new QTreeWidgetItem(lsvServices2);
			item->setData(0, Qt::UserRole, lsvServices1->topLevelItem(i)->data(0, Qt::UserRole));
			item->setText(0, lsvServices1->topLevelItem(i)->text(0));
			item->setIcon(0, lsvServices1->topLevelItem(i)->icon(0));
			item->setSizeHint(0, QSize(18,18));

			delete lsvServices1->topLevelItem(i);
		}
	servicesItemSelectionChanged();
}

void OptionsImpl::servicesItemSelectionChanged()
{
	// disable buttons
	spbAdd->setEnabled(false);
	spbRemove->setEnabled(false);
	// add button
	for (int i = 0; i < lsvServices1->topLevelItemCount(); ++i)
		if (lsvServices1->topLevelItem(i)->isSelected())
		{
			spbAdd->setEnabled(true);
			i = lsvServices1->topLevelItemCount();
		}
	// remove button
	for (int i = 0; i < lsvServices2->topLevelItemCount(); ++i)
		if (lsvServices2->topLevelItem(i)->isSelected())
		{
			spbRemove->setEnabled(true);
			i = lsvServices2->topLevelItemCount();
		}
	// add/rem all buttons
	spbAddAll->setEnabled(lsvServices1->topLevelItemCount() != 0);
	spbRemoveAll->setEnabled(lsvServices2->topLevelItemCount() != 0);
	// sort items
	lsvServices1->sortItems(0, Qt::AscendingOrder);
	lsvServices2->sortItems(0, Qt::AscendingOrder);
}
//

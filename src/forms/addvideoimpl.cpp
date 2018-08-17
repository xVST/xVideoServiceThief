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

#include "addvideoimpl.h"

#include "customdownloadtitleimpl.h"
#include "multiurlsimpl.h"

#include "../tools.h"
#include "../options.h"
#include "../videolistcontroller.h"
#include "../videoinformation.h"
#include "../videoconvert.h"
#include "../videoitem.h"

AddVideoImpl::AddVideoImpl(ProgramOptions *programOptions, VideoInformation *videoInformation,
                           QWidget *parent, Qt::WindowFlags f)
		: QDialog(parent, f)
{
	setupUi(this);
	this->programOptions = programOptions;
	this->videoInformation = videoInformation;
	isCustomDownloadFlag = false;
	// hide the conversion options
	originalSize = size();
#ifndef Q_OS_MACX
	gpbVideoConversion->setVisible(false);
#endif
	chbOverrideConversion->setVisible(QFile::exists(programOptions->getFfmpegLibLocation()));
	resize(width(), 50);
	// set custom download text and font size
	linkTemplate = lblCustomLink->text();
#ifndef Q_OS_WIN32
	QFont customDownloadFont = lblCustomLink->font();
	customDownloadFont.setPointSize(customDownloadFont.pointSize() - 2);
	lblCustomLink->setFont(customDownloadFont);
#endif
	lblCustomLink->hide();
	// prepare conversion options
	fillConversionOptions();
	// connect ok button
	connect(btnOk, SIGNAL(clicked()), this, SLOT(btnOkClicked())); //btn Ok (clicked)
	connect(edtURL, SIGNAL(textChanged(const QString &)), this, SLOT(edtURLChanged(const QString &))); //edtURL changed
	connect(spbPasteURL, SIGNAL(clicked()), this, SLOT(spbPasteURLClicked()));
	connect(spbSelectFile, SIGNAL(clicked()), this, SLOT(spbSelectFileClicked()));
	connect(chbOverrideConversion, SIGNAL(clicked()), this, SLOT(chbOverrideConversionClicked()));
	connect(lblCustomLink, SIGNAL(linkActivated(QString)), this, SLOT(linkActivated(QString)));
}

void AddVideoImpl::fillConversionOptions()
{
	// set conversion data
	cmbOutputFormat->addItems(VideoConverter::getOutputFormatAsStrings());
	cmbVideoResolution->addItems(VideoConverter::getVideoResolutionAsStrings());
	cmbAudioSampling->addItems(VideoConverter::getAudioSampleRatioAsStrings());
	cmbVideoFrameRate->addItems(VideoConverter::getVideoFrameRateAsStrings());
	cmbOutputQuality->addItems(VideoConverter::getOutputQualityAsStrings());

	// set the current video conversion config
	gpbVideoConversion->setChecked(programOptions->getConvertVideos());
	cmbOutputFormat->setCurrentIndex(programOptions->getConversionConf().outputFormat);
	cmbVideoResolution->setCurrentIndex(programOptions->getConversionConf().videoResolution);
	cmbAudioSampling->setCurrentIndex(programOptions->getConversionConf().audioSampleRatio);
	cmbVideoFrameRate->setCurrentIndex(programOptions->getConversionConf().videoFrameRate);
	cmbOutputQuality->setCurrentIndex(programOptions->getConversionConf().outputQuality);
}

OverridedVideoConversionConfig AddVideoImpl::getOverridedConversionConfig()
{
	VideoConversionConfig convConf;
	convConf.outputFormat = static_cast<OutputFormat>(cmbOutputFormat->currentIndex());
	convConf.videoResolution = static_cast<VideoResolution>(cmbVideoResolution->currentIndex());
	convConf.audioSampleRatio = static_cast<AudioSampleRatio>(cmbAudioSampling->currentIndex());
	convConf.videoFrameRate = static_cast<VideoFrameRate>(cmbVideoFrameRate->currentIndex());
	convConf.outputQuality = static_cast<OutputQuality>(cmbOutputQuality->currentIndex());
	// should it be converted?
	OverridedVideoConversionConfig result;
	result.convertVideo = gpbVideoConversion->isChecked();
	result.videoConversionConfig = convConf;
	return result;
}

bool AddVideoImpl::isCustomDownload()
{
	return isCustomDownloadFlag;
}

void AddVideoImpl::loadUrlsFromFile(QString file)
{
	QFile urlsFile(file);
	if (urlsFile.open(QFile::ReadOnly))
	{
		selectedUrls.clear();
		// open the file with those possible urls
		QTextStream urlsStream(&urlsFile);
		while (!urlsStream.atEnd())
			selectedUrls.append(urlsStream.readLine());
		// close file
		urlsFile.close();
		// remove duplicated lines
		selectedUrls.removeDuplicates();
		// check if are valid URLs (invalid urls will be deleted)
		for (int n = selectedUrls.count() - 1; n >= 0; n--)
		{
			QString url = selectedUrls.at(n);
			// init url parsing
			if (!validURL(url) || !videoInformation->isValidHost(url) || videoInformation->isBlockedHost(url))
				selectedUrls.removeAt(n);
		}
		// get a copy with original loaded urls
		originalUrls = selectedUrls;
		// ok
		if (!selectedUrls.isEmpty()) // valid urls detected
		{
			updateEdtUrlColor(true);
			// display config urls link
			lblCustomLink->setText(QString(linkTemplate).arg("select_urls").arg(tr("select URLs to download")));
			lblCustomLink->setVisible(true);
			// enable ok button
			btnOk->setEnabled(true);
		}
		else // no valid urls detected
		{
			updateEdtUrlColor(false);
			// hide config urls link
			lblCustomLink->setVisible(false);
			// disable ok button
			btnOk->setEnabled(false);
		}
		// update informative text
		updateSelectedUrlsText();
		// update icon
        imgService->setPixmap(scaledQPixmap(QPixmap(":/buttons/images/urls_file.svg"), QSize(16, 16)));
		// any change cancels the "custom download" flag
		isCustomDownloadFlag = false;
	}
}

void AddVideoImpl::updateSelectedUrlsText()
{
	if (originalUrls.isEmpty())
	{
		lblVideoService->setText(tr("No valid URLs detected in this file"));
	}
	else // update loaded urls
	{
		QString text = tr("%1 valid URLs detected in this file").arg(originalUrls.count());
		// check if is a partial selection
		text += originalUrls.count() != selectedUrls.count() ? tr(" (%1 selected)").arg(selectedUrls.count()) : "";
		// update text
		lblVideoService->setText(text);
	}
}

QString AddVideoImpl::getCustomDownloadTitle()
{
	return customDownloadTitle;
}

QStringList AddVideoImpl::getSelectedURLs()
{
	return selectedUrls;
}

void AddVideoImpl::btnOkClicked()
{
	if (VideoListController::instance()->isAlreadyAdded(edtURL->text()))
		native_alert(this, QMessageBox::Information, tr("Already added"),
					 tr("You already added this video. Check your downloads list."), tr("Ok"));
	else // all ok
		done(QDialog::Accepted);
}

void AddVideoImpl::updateEdtUrlColor(bool ok)
{
	// set color
	QColor valid(qApp->palette().brush(QPalette::Base).color());
	QColor invalid(255, 170, 127);

	QPalette p = edtURL->palette();
	p.setColor(QPalette::Base, ok ? valid : invalid);
	edtURL->setPalette(p);
}

void AddVideoImpl::edtURLChanged(const QString &text)
{
	// check if is a real file from our hard-disk (may be a file with urls)
	if (QFile::exists(text))
	{
		loadUrlsFromFile(text);
		// don't continue parsing the text... is a special case...
		return;
	}

	// init url parsing
	bool ok = validURL(text) && videoInformation->isValidHost(text);
	QString blockMsg = "";
	QString isCustomVideo = "";

	if (ok)
	{
		BlockedState bs = bsNotBlocked;
		ok = !videoInformation->isBlockedHost(text, bs);

		if (bs == bsBlocked)
			blockMsg = tr(" - Blocked site");
		else if (bs == bsAdultContent)
			blockMsg = tr(" - Adult content is not allowed");
	}

	btnOk->setEnabled(ok);

	// set custom download visibility
	lblCustomLink->setText(QString(linkTemplate).arg("custom_url").arg(tr("mark as custom download")));
	lblCustomLink->setVisible(!videoInformation->isValidHost(text) && validURL(text, true));

	// update edit color
	updateEdtUrlColor(ok);

	// set host info
    lblVideoService->setText(videoInformation->getHostCaption(text) + blockMsg + isCustomVideo);
    imgService->setPixmap(scaledQPixmap(videoInformation->getHostImage(text, true), QSize(16, 16)));

	// any change cancels the "custom download" flag
	isCustomDownloadFlag = false;

	// remove any possible url loaded from a file
	selectedUrls.clear();
	// add url (unique )
	selectedUrls.append(text);
}

void AddVideoImpl::spbPasteURLClicked()
{
	edtURL->setText(QApplication::clipboard()->text().trimmed());
}

void AddVideoImpl::spbSelectFileClicked()
{
#ifdef Q_OS_MACX
	QFileDialog fileDialog(this, Qt::Sheet);
	// configure file dialog
	fileDialog.setFileMode(QFileDialog::ExistingFile);
	// open select file dialog
	if (showModalDialog(&fileDialog) == QDialog::Accepted)
		edtURL->setText(fileDialog.selectedFiles().first());
#else
	QString filePath = QFileDialog::getOpenFileName(this, tr("Select URL's file"));
	// if user did "accept"
	if (!filePath.isEmpty()) edtURL->setText(filePath);
#endif
}

void AddVideoImpl::chbOverrideConversionClicked()
{
#ifndef Q_OS_MACX
	if (!gpbVideoConversion->isVisible())
	{
		gpbVideoConversion->setVisible(true);
		// calcule size diference
		int preWidth = (width() > originalSize.width() ? width() - originalSize.width() : originalSize.width() - width())/2;
		int preHeight = (height() > originalSize.height() ? height() - originalSize.height() : originalSize.height() - height())/2;
		// move the form to update the new size
		move(x() - preWidth, y() - preHeight);
	}
#endif
	// enable or disable the groupbox
	gpbVideoConversion->setEnabled(chbOverrideConversion->isChecked());
}

void AddVideoImpl::linkActivated(const QString &href)
{
	if (href == "custom_url") // set as custom url
	{
		// display the custom title dialog
		CustomDownloadTitleImpl customTitleDialog(this, Qt::Sheet);
		if (showModalDialog(&customTitleDialog) == QDialog::Accepted)
		{
			customDownloadTitle = customTitleDialog.edtTitle->text();
			isCustomDownloadFlag = true;
			// change service image
			lblVideoService->setText(tr("User custom video download"));
            imgService->setPixmap(scaledQPixmap(QPixmap(":/services/images/services/custom_video.svg"), QSize(16, 16)));
			// remove red colors
			updateEdtUrlColor(true);
			// enable ok button
			btnOk->setEnabled(true);
			// hide custom link
			lblCustomLink->hide();
		}
	}
	else if (href == "select_urls") // selecting urls
	{
		MultiURLsImpl multiURLs(videoInformation, selectedUrls, originalUrls, this, Qt::Sheet);
		if (showModalDialog(&multiURLs) == QDialog::Accepted)
		{
			// get the checked urls
			selectedUrls.clear();
			selectedUrls.append(multiURLs.getSelectedURLs());
			// update informative text
			updateSelectedUrlsText();
			// disable the ok button if no urls selected
			btnOk->setEnabled(!selectedUrls.isEmpty());
		}
	}
}
//

/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

#include "updatecenterimpl.h"

#include "windowsvistadetectedimpl.h"

#include "../updates.h"
#include "../tools.h"
#include "../programversion.h"

UpdateCenterImpl::UpdateCenterImpl(Updates *updates, bool autoDownloadAndInstall, QWidget * parent, Qt::WindowFlags f)
		: QDialog(parent, f)
{
	setupUi(this);
	// resize for macosx
#ifndef Q_OS_WIN32
	resize(630, 366);
#endif
	closedByButton = false;
	// version
	lblxVSTVersion->setText(QString(lblxVSTVersion->text()).arg(PROGRAM_VERSION));
	// set update class
	this->updates = updates;
	// configure updates list
	QStringList headers;
	headers << tr(" File ") << tr(" Version ") << tr(" Size ") << tr(" Progress ");
	// add the headers
	lsvUpdates->setHeaderLabels(headers);
	// change headers sizes
	QFontMetrics fm = fontMetrics();
	QHeaderView *header = lsvUpdates->header();
	// resize
	header->resizeSection(1, fm.width(headers.at(0) + "9.99.999 alpha"));
	header->resizeSection(2, fm.width(headers.at(1) + " 1024,99Mb "));
	// configure resize mode
	header->setHighlightSections(false);
	header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
	// set header text aligment
	QTreeWidgetItem * headerItem = lsvUpdates->headerItem();
	headerItem->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
	headerItem->setTextAlignment(2, Qt::AlignRight   | Qt::AlignVCenter);
	headerItem->setTextAlignment(3, Qt::AlignHCenter | Qt::AlignVCenter);
	// fill data
	fillUpdates();
	// signals
	connect(lsvUpdates, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*, int)));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(btnCancelClicked()));
	connect(btnUpdate, SIGNAL(clicked()), this, SLOT(btnUpdateClicked()));
	// updater
	connect(updates, SIGNAL(downloadingUpdate(int, int, int)), this, SLOT(downloadingUpdate(int, int, int)));
	connect(updates, SIGNAL(downloadUpdateFinished(int)), this, SLOT(downloadUpdateFinished(int)));
	connect(updates, SIGNAL(downloadUpdateError(int)), this, SLOT(downloadUpdateError(int)));
	connect(updates, SIGNAL(downloadsFinished()), this, SLOT(downloadsFinished()));
	connect(updates, SIGNAL(readyToInstallUpdates()), this, SLOT(readyToInstallUpdates()));
	connect(updates, SIGNAL(failedToInstallUpdates()), this, SLOT(failedToInstallUpdates()));
	// if auto download & install updates, then...
	if (autoDownloadAndInstall)
	{
		// disable update button
		lsvUpdates->setEnabled(false);
		btnUpdate->setEnabled(false);
		// start automatically to download updates (in 500ms)
		QTimer::singleShot(500, this, SLOT(btnUpdateClicked()));
	}
}

void UpdateCenterImpl::closeEvent(QCloseEvent *event)
{
	if (!closedByButton)
	{
		event->ignore();
		btnCancelClicked();
	}
}

void UpdateCenterImpl::fillUpdates()
{
	for (int n = 0; n < updates->getUpdatesCount(); n++)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(lsvUpdates);

		item->setTextAlignment(1, Qt::AlignHCenter | Qt::AlignVCenter);
		item->setTextAlignment(2, Qt::AlignRight   | Qt::AlignVCenter);
		item->setTextAlignment(3, Qt::AlignHCenter | Qt::AlignVCenter);

		item->setText(0, updates->getUpdateItem(n)->getCaption());
		item->setText(1, updates->getUpdateItem(n)->getVersion());
		item->setText(2, fileSizeToString(updates->getUpdateItem(n)->getSize()));
		item->setText(3, "-");

		item->setSizeHint(0, QSize(18,18));

		item->setCheckState(0, Qt::Checked);
	}
}

void UpdateCenterImpl::itemChanged(QTreeWidgetItem* /*item*/, int /*column*/)
{
	if (lsvUpdates->isEnabled())
	{
		bool enabled = false;

		for (int n = 0; n < lsvUpdates->topLevelItemCount(); n++)
			if (lsvUpdates->topLevelItem(n)->checkState(0) == Qt::Checked)
				enabled = true;

		btnUpdate->setEnabled(enabled);
	}
}

void UpdateCenterImpl::btnCancelClicked()
{
	closedByButton = true;
	updates->cancel();
	done(QDialog::Rejected);
}

void UpdateCenterImpl::btnUpdateClicked()
{
	// select checked updates
	for (int n = 0; n < lsvUpdates->topLevelItemCount(); n++)
		updates->getUpdateItem(n)->setChecked(lsvUpdates->topLevelItem(n)->checkState(0) == Qt::Checked);

	lsvUpdates->setEnabled(false);
	btnUpdate->setEnabled(false);
	updates->downloadUpdates();
}

void UpdateCenterImpl::downloadingUpdate(int updateIndex, int pogress, int totalProgress)
{
	lsvUpdates->topLevelItem(updateIndex)->setText(3, QString("%1%").arg(pogress));
	pgbUpdate->setValue(totalProgress);
	lblUpdateSate->setText(tr("<b>Update state:</b> Downloading %1")
							.arg(lsvUpdates->topLevelItem(updateIndex)->text(0)));
	lblDownloadedSize->setText(tr("%1 (%2)")
								.arg(fileSizeToString(updates->getCurrentDownloaded()))
								.arg(fileSizeToString(updates->getTotalToDownload())));
}

void UpdateCenterImpl::downloadUpdateFinished(int updateIndex)
{
	lsvUpdates->topLevelItem(updateIndex)->setText(3, tr("Done"));
}

void UpdateCenterImpl::downloadUpdateError(int updateIndex)
{
	lsvUpdates->topLevelItem(updateIndex)->setText(3, tr("Error"));
}

void UpdateCenterImpl::downloadsFinished()
{
	btnCancel->setEnabled(false);
}

void UpdateCenterImpl::readyToInstallUpdates()
{
	closedByButton = true;
	done(QDialog::Accepted);
}

void UpdateCenterImpl::failedToInstallUpdates()
{
	native_alert(this, QMessageBox::Critical, tr("Updates center error"),
				 tr("Some errors has ocurred on try download the new update(s)."), tr("Ok"));
	// enable clancel button
	btnCancel->setEnabled(true);
	btnCancel->setDefault(true);
	btnCancel->setText(tr("Close"));
}
//

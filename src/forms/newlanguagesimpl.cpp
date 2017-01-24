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

#include "newlanguagesimpl.h"

#include "../options.h"
#include "../languages.h"
#include "../newlanguages.h"
#include "../tools.h"
#include "../updates.h"

NewLanguagesImpl::NewLanguagesImpl(ProgramOptions *programOptions, QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// resize form if is needed
#ifdef Q_OS_MACX
	resize(566, 402);
#endif
	lsvInstalled->header()->hide();
	lsvToInstall->header()->hide();
	// create new languages controller
	newLanguages = new NewLanguagesController(programOptions);
	// connect signals
	connect(newLanguages, SIGNAL(installedLanguagesClear()), this, SLOT(installedLanguagesClear()));
	connect(newLanguages, SIGNAL(toInstallLanguagesClear()), this, SLOT(toInstallLanguagesClear()));
	connect(newLanguages, SIGNAL(installedLanaguageAdded(Language*)), this, SLOT(installedLanaguageAdded(Language*)));
	connect(newLanguages, SIGNAL(installedLanguageRemoved(Language*, bool)), this, SLOT(installedLanguageRemoved(Language*, bool)));
	connect(newLanguages, SIGNAL(afterCheckNewLanguages()), this, SLOT(afterCheckNewLanguages()));
	connect(newLanguages, SIGNAL(toInstallLanguageAdded(Update*)), this, SLOT(toInstallLanguageAdded(Update*)));
	connect(newLanguages, SIGNAL(beforeInstallNewLanguage()), this, SLOT(beforeInstallNewLanguage()));
	connect(newLanguages, SIGNAL(beforeUpdateNewLanguages()), this, SLOT(beforeUpdateNewLanguages()));
	connect(newLanguages, SIGNAL(afterInstallNewLanguage(Update*,bool)), this, SLOT(afterInstallNewLanguage(Update*,bool)));
	connect(newLanguages, SIGNAL(downloadProgress(int,int)), this, SLOT(downloadProgress(int,int)));
	//
	connect(btnInstall, SIGNAL(clicked()), this, SLOT(btnInstallClicked()));
	connect(btnUninstall, SIGNAL(clicked()), this, SLOT(btnUninstallClicked()));
	connect(lsvInstalled, SIGNAL(itemSelectionChanged()), this, SLOT(lsvInstalledItemSelectionChanged()));
	connect(lsvToInstall, SIGNAL(itemSelectionChanged()), this, SLOT(lsvToInstallItemSelectionChanged()));
	// start
	newLanguages->initialize();
}

NewLanguagesImpl::~NewLanguagesImpl()
{
    delete newLanguages;
}

void NewLanguagesImpl::lsvInstalledItemSelectionChanged()
{
	btnUninstall->setEnabled(lsvInstalled->selectedItems().count() > 0 && !newLanguages->isInstalling());
}

void NewLanguagesImpl::lsvToInstallItemSelectionChanged()
{
	btnInstall->setEnabled(lsvToInstall->selectedItems().count() > 0 && !newLanguages->isInstalling());
}

void NewLanguagesImpl::btnUninstallClicked()
{
	if (native_alert(this, QMessageBox::Question, tr("Uninstall language"), tr("Wish you uninstall the lanuage?"), tr("Yes"), tr("No")) == 0)
		newLanguages->uninstallLanguage(lsvInstalled->currentIndex().row());
}

void NewLanguagesImpl::btnInstallClicked()
{
	if (native_alert(this, QMessageBox::Question, tr("Install language"), tr("Wish you install this new language?"), tr("Yes"), tr("No")) == 0)
		newLanguages->installLanguage(lsvToInstall->currentIndex().row());
}

void NewLanguagesImpl::installedLanguagesClear()
{
	lsvInstalled->clear();
}

void NewLanguagesImpl::toInstallLanguagesClear()
{
	lsvToInstall->clear();
}

void NewLanguagesImpl::installedLanaguageAdded(Language *language)
{
	QTreeWidgetItem *newItem = new QTreeWidgetItem(lsvInstalled);
	newItem->setText(0, language->getId());
	newItem->setSizeHint(0, QSize(18,18));
	newItem->setIcon(0, QIcon(":/buttons/images/script.png"));
}

void NewLanguagesImpl::installedLanguageRemoved(Language *language, bool removed)
{
	if (!removed)
		if (isWindowsVista())
			native_alert(this, QMessageBox::Critical,
						 tr("Language uninstall error"),
						 tr("Some errors has ocurred on try uninstall the selected <b>%1</b> language.<br><br><b>Note:</b> On Windows Vista you should execute the xVST as administrator before uninstall languages.").arg(language->getId()),
						 tr("Ok"));
		else
			native_alert(this, QMessageBox::Critical,
						 tr("Language uninstall error"),
						 tr("Some errors has ocurred on try uninstall the selected <b>%1</b> language.").arg(language->getId()),
						 tr("Ok"));
	else
		native_alert(this, QMessageBox::Information,
					 tr("Language uninstall"),
					 tr("Language <b>%1</b> uninstalled.").arg(language->getId()),
					 tr("Ok"));
	// disable unnstall button
	btnUninstall->setEnabled(false);
}

void NewLanguagesImpl::afterCheckNewLanguages()
{
	lblAction->hide();
	pgbAction->hide();
}

void NewLanguagesImpl::toInstallLanguageAdded(Update *language)
{
	QTreeWidgetItem *newItem = new QTreeWidgetItem(lsvToInstall);
	newItem->setText(0, language->getCaption());
	newItem->setSizeHint(0, QSize(18,18));
	newItem->setIcon(0, QIcon(":/buttons/images/script.png"));
}

void NewLanguagesImpl::beforeInstallNewLanguage()
{
	lblAction->setText(tr("Installing new language..."));
	lblAction->show();
	pgbAction->setValue(0);
	pgbAction->setMaximum(0);
	pgbAction->show();
	// disable button
	btnInstall->setEnabled(false);
	btnUninstall->setEnabled(false);
}

void NewLanguagesImpl::afterInstallNewLanguage(Update *update, bool error)
{
	if (error)
		if (isWindowsVista())
			native_alert(this, QMessageBox::Critical,
						 tr("Language install error"),
						 tr("Some errors has ocurred on try install the selected <b>%1</b> language.<br><br><b>Note:</b> On Windows Vista you should execute the xVST as administrator before install new languages.").arg(update->getCaption()),
						 tr("Ok"));
		else
			native_alert(this, QMessageBox::Critical,
						 tr("Language install error"),
						 tr("Some errors has ocurred on try uninstall the selected <b>%1</b> language.").arg(update->getCaption()),
						 tr("Ok"));
	else // no error
		native_alert(this, QMessageBox::Information,
					 tr("Language install"),
					 tr("New language <b>%1</b> installed.").arg(update->getCaption()),
					 tr("Ok"));
	lblAction->hide();
	pgbAction->hide();
}

void NewLanguagesImpl::beforeUpdateNewLanguages()
{
	btnInstall->setEnabled(false);
}

void NewLanguagesImpl::downloadProgress(int pos, int max)
{
	pgbAction->setMaximum(max);
	pgbAction->setValue(pos);
}

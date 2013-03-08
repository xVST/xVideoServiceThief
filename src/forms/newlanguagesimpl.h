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

#ifndef NEWLANGUAGESIMPL_H
#define NEWLANGUAGESIMPL_H

#include <QtWidgets>

#include "ui_newlanguages.h"

class NewLanguagesController;
class ProgramOptions;
class Language;
class Update;

class NewLanguagesImpl : public QDialog, public Ui::NewLanguages
{
Q_OBJECT
	private:
		NewLanguagesController *newLanguages;	//! Responsable of check, download and install new languages
	public:
		NewLanguagesImpl(ProgramOptions *programOptions, QWidget * parent = 0, Qt::WindowFlags f = 0 );
		~NewLanguagesImpl();
	private slots:
		void lsvInstalledItemSelectionChanged();
		void lsvToInstallItemSelectionChanged();
		void btnUninstallClicked();
		void btnInstallClicked();
		void installedLanguagesClear();
		void toInstallLanguagesClear();
		void installedLanaguageAdded(Language *language);
		void installedLanguageRemoved(Language *language, bool removed);
		void afterCheckNewLanguages();
		void toInstallLanguageAdded(Update *language);
		void beforeInstallNewLanguage();
		void beforeUpdateNewLanguages();
		void afterInstallNewLanguage(Update *update, bool error);
		void downloadProgress(int pos, int max);
};

#endif // NEWLANGUAGESIMPL_H

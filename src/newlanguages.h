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
* Program URL   : https://www.xvideothief.com/
*
*/

#ifndef NEWLANGUAGES_H
#define NEWLANGUAGES_H

#include <QtWidgets>

static const QString URL_LANGUAGES_FILE = "https://www.xvideothief.com/updates/xVST2.languages";

class Http;
class Update;
class Language;
class ProgramOptions;
class LanguageManager;

/*! Get new languages and install them */
class NewLanguagesController : public QThread
{
Q_OBJECT
	private:
		ProgramOptions *programOptions;		//!< Instance of program options
		LanguageManager *languageManager;	//!< Language manager, used for local installed languages
		QList<Update *> *newLanguages;		//!< List with all aviable languages to install
		QString newLanguagesFile;			//!< Update file used to fill the updates array
		Http *http;							//!< Http instance
		Update *currentLanguage;				//!< Current update (new language) which we are installing
		/*! Thread executation */
		void run();
		/*! Fill installed languages list */
		void fillInstalledLanguages();
		/*! Update the list of languages ready to install */
		void updateLanguagesToInstall();
		/*! Parse a block */
		void parseBlock(QString block);
		/*! Clear all stored new languages */
		void clearAllNewLanguages();
		/*! Update new languages */
		void updateNewLanguages();
		/*! Initialize the Http object (if is needed) */
		void initializeHttp();
		/*! Deinitalize the Http object (if is needed) */
		void deinitHttp();
	public:
		/*! Class construction */
		NewLanguagesController(ProgramOptions *programOptions);
		/* Class destruction */
		~NewLanguagesController();
		/*! Initialize class */
		void initialize();
		/*! Uninstall a selected language */
		void uninstallLanguage(int index);
		/*! Install a selected language */
		void installLanguage(int index);
		/*! Get if is installing a new language */
		bool isInstalling() const;
	private slots:
		/*! On finish a download */
		void downloadFinished(const QFileInfo destFile);
		/*! On download error */
		void downloadError(int error);
		/*! Private Download progress */
		void privateDownloadProgress(int pos, int max);
	signals:
		/*! On clear installed languages */
		void installedLanguagesClear();
		/*! On clear to install languages */
		void toInstallLanguagesClear();
		/*! Add a installed language */
		void installedLanaguageAdded(Language *language);
		/*! Removed an installed language */
		void installedLanguageRemoved(Language *language, bool removed);
		/*! After check updates */
		void afterCheckNewLanguages();
		/*! Add a installable language */
		void toInstallLanguageAdded(Update *language);
		/*! Before update the new languages list */
		void beforeUpdateNewLanguages();
		/*! Before start a install */
		void beforeInstallNewLanguage();
		/*! After install a language */
		void afterInstallNewLanguage(Update *update, bool error);
		/*! Download progress */
		void downloadProgress(int pos, int max);
};

#endif // NEWLANGUAGES_H

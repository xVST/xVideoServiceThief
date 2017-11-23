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
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef __LANGUAGES_H__
#define __LANGUAGES_H__

#include <QtWidgets>

/*! Language file description */
class Language : public QObject
{
Q_OBJECT
	private:
		QString file;			//!< Language file
		QString langFile;		//!< QM file
		QString id;				//!< Language id (english, català, español...)
		QString version;		//!< Current language file version
		QString author;			//!< Language file author
		QString contact;		//!< Language file author contact
		QString description;	//!< Language file description
	public:
		QString getFile();
		void setFile(QString value);
		QString getLangFile();
		void setLangFile(QString value);
		QString getId();
		void setId(QString value);
		QString getVersion();
		void setVersion(QString value);
		QString getAuthor();
		void setAuthor(QString value);
		QString getContact();
		void setContact(QString value);
		QString getDescription();
		void setDescription(QString value);
		QString getLangLocale();
};

/*! Language files manager */
class LanguageManager : public QObject
{
Q_OBJECT
	private:
		QList<Language *> *languagesList;	//!< List with all detected languages
		/*! Clear the language list */
		void clear();
		/*! Get if is a valid index */
		bool isValidIndex(const int index);
		/*! Load an individual language file */
		void loadLangFile(QString langFile);
	public:
		/*! Class constructor */
		LanguageManager();
		/*! Class destructor */
		~LanguageManager();
		/*! Get a language item */
		Language* getLanguage(const int index);
		/*! Get the languages items count */
		int getLanguagesCount();
		/*! Detect all installed languages */
		void loadLangFiles(QString langDir);
		/*! Get if this language id exsits (is installed) */
		bool isLanguageInstalled(QString languageId);
		/*! Get the language which corresponds to current system configuration */
		Language *getAutoLanguage();
		/*! Get the language file name which corresponds to current system configuration */
		QString getAutoLanguageFileName();
		/*! Install a language */
		static QString get_qm_languageFile(QString langFile);
		/*! Return a Langage item info */
		static Language* getLanguageInfo(QString langFile);
		/*! Get the default user language */
		static QString getDefaultUserLanguage(QString langDir);
};
#endif // __LANGUAGES_H__

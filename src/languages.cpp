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

#include "languages.h"

#include "tools.h"

LanguageManager::LanguageManager()
{
	languagesList = new QList<Language *>;
}

LanguageManager::~LanguageManager()
{
	clear();
	delete languagesList;
}

void LanguageManager::clear()
{
	while (!languagesList->isEmpty())
		languagesList->removeFirst();
}

bool LanguageManager::isValidIndex(const int index)
{
	return index >= 0 && index < getLanguagesCount();
}

void LanguageManager::loadLangFile(QString langFile)
{
	if (QFile::exists(langFile))
	{
		languagesList->append(new Language);
		Language *language = languagesList->last();

		QFileInfo file(langFile);
		QSettings langInfo(langFile, QSettings::IniFormat);

		language->setFile(file.fileName());
		language->setLangFile(langInfo.value("language/file", "").toString());
		language->setId(langInfo.value("language/id", "").toString());
		language->setVersion(langInfo.value("language/version", "").toString());
		language->setAuthor(langInfo.value("language/author", "").toString());
		language->setContact(langInfo.value("language/contact", "").toString());
		language->setDescription(langInfo.value("language/description", "").toString());
	}
}

Language* LanguageManager::getLanguage(const int index)
{
	if (isValidIndex(index))
		return languagesList->at(index);
	else
		return NULL;
}

int LanguageManager::getLanguagesCount()
{
	return languagesList->count();
}

void LanguageManager::loadLangFiles(QString langDir)
{
	languagesList->clear();

	QStringList languageFiles;
	QDir languagesDir(langDir);

	languageFiles = languagesDir.entryList(QDir::Files, QDir::Name);
	languageFiles = languageFiles.filter(".language");

	foreach (QString langFile, languageFiles)
		if (langFile.endsWith(".language"))
			loadLangFile(QDir::toNativeSeparators(langDir + "/" + langFile));
}

bool LanguageManager::isLanguageInstalled(QString languageId)
{
	for (int n = 0; n < getLanguagesCount(); n++)
		if (getLanguage(n)->getId() == languageId)
			return true;
	// not installed
	return false;
}

Language* LanguageManager::getAutoLanguage()
{
	// get the user system configuration
	QLocale locale;
	QString systemQM = QString("xVST_%1.qm").arg(copy(locale.name(), 0, locale.name().indexOf("_")));
	// find the
	for (int n = 0; n < languagesList->count(); n++)
		if (languagesList->at(n)->getLangFile() == systemQM)
			return languagesList->at(n);
	// return the default language
	return NULL;
}

QString LanguageManager::getAutoLanguageFileName()
{
	Language *language = getAutoLanguage();
	// return language definition file
	return language != NULL ? language->getFile() : "english_uk.language";
}

QString LanguageManager::get_qm_languageFile(QString langFile)
{
	QFileInfo langFileInfo(langFile);
	QSettings langInfo(langFile, QSettings::IniFormat);
	QString qmFile = langInfo.value("language/file", "null").toString();

	if (qmFile.toLower() != "null" && QFile::exists(langFileInfo.path() + "/" + qmFile))
		return langFileInfo.path() + "/" + qmFile;
	else
		return "null";
}

Language* LanguageManager::getLanguageInfo(QString langFile)
{
	Language *result = NULL;

	if (QFile::exists(langFile))
	{
		result = new Language;

		QFileInfo file(langFile);
		QSettings langInfo(langFile, QSettings::IniFormat);

		result->setFile(file.fileName());
		result->setLangFile(langInfo.value("language/file", "").toString());
		result->setId(langInfo.value("language/id", "").toString());
		result->setVersion(langInfo.value("language/version", "").toString());
		result->setAuthor(langInfo.value("language/author", "").toString());
		result->setContact(langInfo.value("language/contact", "").toString());
		result->setDescription(langInfo.value("language/description", "").toString());
	}

	return result;
}

QString LanguageManager::getDefaultUserLanguage(QString langDir)
{
	LanguageManager lm;
	lm.loadLangFiles(langDir);
	return lm.getAutoLanguageFileName();
}

// Language file description

QString Language::getFile()
{
	return file;
}

void Language::setFile(QString value)
{
	file = value;
}

QString Language::getLangFile()
{
	return langFile;
}

void Language::setLangFile(QString value)
{
	langFile = value;
}

QString Language::getId()
{
	return id;
}

void Language::setId(QString value)
{
	id = value;
}

QString Language::getVersion()
{
	return version;
}

void Language::setVersion(QString value)
{
	version = value;
}

QString Language::getAuthor()
{
	return author;
}

void Language::setAuthor(QString value)
{
	author = value;
}

QString Language::getContact()
{
	return contact;
}

void Language::setContact(QString value)
{
	contact = value;
}

QString Language::getDescription()
{
	return description;
}

void Language::setDescription(QString value)
{
	description = value;
}

QString Language::getLangLocale()
{
	return copyBetween(langFile, "xVST_", ".qm");
}

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

#ifndef SEARCHVIDEOSSCRIPTCLASS_H
#define SEARCHVIDEOSSCRIPTCLASS_H

#include <QtCore>
#include <QtScript>

class SearchResults;

class SearchResultsScriptPrototype : public QObject, public QScriptable
{
Q_OBJECT
	private:
		SearchResults *thisSearchResults() const;
	public:
		SearchResultsScriptPrototype(QObject *parent = 0);
		~SearchResultsScriptPrototype();
	public slots:
		void addSearchResult(QString videoUrl, QString imageUrl, QString title, QString description, int duration, double rating);
		void setSummary(QString value);
		QString getUserLanguage();
};

class SearchResultsScriptClass : public QObject, public QScriptClass
{
Q_OBJECT
	private:
		QScriptValue proto;
		QScriptValue ctor;
		static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);
		static QScriptValue toScriptValue(QScriptEngine *eng, const SearchResults &searchResults);
		static void fromScriptValue(const QScriptValue &obj, SearchResults &searchResults);
	public:
		SearchResultsScriptClass(QScriptEngine *engine);
		~SearchResultsScriptClass();
		QScriptValue constructor();
		QString name() const;
		QScriptValue prototype() const;
		QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object, const QScriptString &name, uint id);
		QScriptValue newInstance();
		QScriptValue newInstance(const SearchResults &searchResults);
};

#endif // SEARCHVIDEOSSCRIPTCLASS_H

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

#ifndef HTTPSCRIPTCLASS_H
#define HTTPSCRIPTCLASS_H

#include <QtCore>
#include <QtScript>

class Http;

class HttpScriptPrototype : public QObject, public QScriptable
{
Q_OBJECT
	private:
		Http *thisHttp() const;
		bool url_tolerantMode;
	public:
		HttpScriptPrototype(QObject *parent = 0);
		~HttpScriptPrototype();
	public slots:
		QString downloadWebpage(const QString URL, bool isUtf8 = true);
		QString downloadWebpageEx(const QString URL, bool url_tolerantMode, bool isUtf8 = true);
		QString downloadWebpagePost(const QString URL, QString parameters, bool isUtf8 = true);
		QString downloadWebpagePostEx(const QString URL, QString parameters, bool url_tolerantMode, bool isUtf8 = true);
		QString downloadWebpageHeaders(const QString URL, QString separator = "\n");
		void addCookie(QString cookie);
		void addCookies(QString cookies, QString separator = "|");
		QString getCookies(QString separator = "|");
		void clearCookies();
		void setCookiesEnabled(bool value);
		void addHeaderParameter(QString key, QString value);
		void clearHeaderParameters();
		void setMaxRetries(int value);
		void setTimeOutOption(bool value);
		void setTimeOut(int value);
		void setMaxAutoJumps(int value);
		void setUserAgent(QString value);
		int getLastError();
		int getLastStopReason();
};

class HttpScriptClass : public QObject, public QScriptClass
{
Q_OBJECT
	private:
		QScriptValue proto;
		QScriptValue ctor;
		static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);
		static QScriptValue toScriptValue(QScriptEngine *eng, const Http &http);
		static void fromScriptValue(const QScriptValue &obj, Http &http);
	public:
		HttpScriptClass(QScriptEngine *engine);
		~HttpScriptClass();
		QScriptValue constructor();
		QString name() const;
		QScriptValue prototype() const;
		QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object, const QScriptString &name, uint id);
		QScriptValue newInstance();
		QScriptValue newInstance(const Http &http);
};

#endif // HTTPSCRIPTCLASS_H

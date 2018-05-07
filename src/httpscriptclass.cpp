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

#include "httpscriptclass.h"

#include "http.h"

Q_DECLARE_METATYPE(Http)
Q_DECLARE_METATYPE(Http*)
Q_DECLARE_METATYPE(HttpScriptClass*)

/* Http script prototype */

HttpScriptPrototype::HttpScriptPrototype(QObject *parent) : QObject(parent)
{
}

HttpScriptPrototype::~HttpScriptPrototype()
{
}

Http *HttpScriptPrototype::thisHttp() const
{
	return qscriptvalue_cast<Http*>(thisObject().data());
}

QString HttpScriptPrototype::downloadWebpage(const QString URL, bool isUtf8)
{
	return thisHttp()->downloadWebpage(QUrl(URL), isUtf8);
}

QString HttpScriptPrototype::downloadWebpageEx(const QString URL, bool url_tolerantMode, bool isUtf8)
{
	return thisHttp()->downloadWebpage(QUrl(URL, url_tolerantMode ? QUrl::TolerantMode : QUrl::StrictMode), isUtf8);
}

QString HttpScriptPrototype::downloadWebpagePost(const QString URL, QString parameters, bool isUtf8)
{
	return thisHttp()->downloadWebpagePost(QUrl(URL), parameters, isUtf8);
}

QString HttpScriptPrototype::downloadWebpagePostEx(const QString URL, QString parameters, bool url_tolerantMode, bool isUtf8)
{
	return thisHttp()->downloadWebpagePost(QUrl(URL, url_tolerantMode ? QUrl::TolerantMode : QUrl::StrictMode), parameters, isUtf8);
}

QString HttpScriptPrototype::downloadWebpageHeaders(const QString URL, QString separator)
{
	return thisHttp()->head(QUrl(URL)).replace("\n", separator);
}

void HttpScriptPrototype::addCookie(QString cookie)
{
	thisHttp()->addCookie(cookie);
}

void HttpScriptPrototype::addCookies(QString cookies, QString separator)
{
	thisHttp()->addCookies(cookies, separator);
}

QString HttpScriptPrototype::getCookies(QString separator)
{
	return thisHttp()->getCookies(separator);
}

void HttpScriptPrototype::clearCookies()
{
	thisHttp()->clearCookies();
}

void HttpScriptPrototype::setCookiesEnabled(bool value)
{
	thisHttp()->setCookiesEnabled(value);
}

void HttpScriptPrototype::addHeaderParameter(QString key, QString value)
{
	thisHttp()->addHeaderParameter(key, value);
}

void HttpScriptPrototype::clearHeaderParameters()
{
	thisHttp()->clearHeaderParameters();
}

void HttpScriptPrototype::setMaxRetries(int value)
{
	thisHttp()->setMaxRetries(value);
}

void HttpScriptPrototype::setTimeOutOption(bool value)
{
	thisHttp()->setTimeOutOption(value);
}

void HttpScriptPrototype::setTimeOut(int value)
{
	thisHttp()->setTimeOut(value);
}

void HttpScriptPrototype::setMaxAutoJumps(int value)
{
	thisHttp()->setMaxAutoJumps(value);
}

void HttpScriptPrototype::setUserAgent(QString value)
{
	thisHttp()->setUserAgent(value);
}

int HttpScriptPrototype::getLastError()
{
	return thisHttp()->getLastError();
}

int HttpScriptPrototype::getLastStopReason()
{
	return thisHttp()->getLastStopReason();
}

/* Http script class */

HttpScriptClass::HttpScriptClass(QScriptEngine *engine) : QObject(engine), QScriptClass(engine)
{
	qScriptRegisterMetaType<Http>(engine, toScriptValue, fromScriptValue);

	proto = engine->newQObject(new HttpScriptPrototype(this),
							   QScriptEngine::QtOwnership,
							   QScriptEngine::SkipMethodsInEnumeration
							   | QScriptEngine::ExcludeSuperClassMethods
							   | QScriptEngine::ExcludeSuperClassProperties);
	QScriptValue global = engine->globalObject();
	proto.setPrototype(global.property("Object").property("prototype"));

	ctor = engine->newFunction(construct);
	ctor.setData(qScriptValueFromValue(engine, this));

	engine->globalObject().setProperty("Http", this->constructor());
}

HttpScriptClass::~HttpScriptClass()
{
}

QScriptValue HttpScriptClass::construct(QScriptContext *ctx, QScriptEngine *)
{
	HttpScriptClass *cls = qscriptvalue_cast<HttpScriptClass*>(ctx->callee().data());
	if (!cls) return QScriptValue();
	return cls->newInstance();
}

QScriptValue HttpScriptClass::toScriptValue(QScriptEngine *eng, const Http &http)
{
	QScriptValue ctor = eng->globalObject().property("Http");
	HttpScriptClass *cls = qscriptvalue_cast<HttpScriptClass*>(ctor.data());
	if (!cls) return eng->newVariant(qVariantFromValue(http));
	return cls->newInstance(http);
}

void HttpScriptClass::fromScriptValue(const QScriptValue &obj, Http &http)
{
	http = qscriptvalue_cast<Http>(obj.data());
}

QScriptValue HttpScriptClass::constructor()
{
	return ctor;
}

QString HttpScriptClass::name() const
{
	return QLatin1String("Http");
}

QScriptValue HttpScriptClass::prototype() const
{
	return proto;
}

QScriptValue::PropertyFlags HttpScriptClass::propertyFlags(const QScriptValue&, const QScriptString&, uint)
{
	return QScriptValue::Undeletable;
}

QScriptValue HttpScriptClass::newInstance()
{
	return newInstance(Http());
}

QScriptValue HttpScriptClass::newInstance(const Http &http)
{
	QScriptValue data = engine()->newVariant(qVariantFromValue(http));
	return engine()->newObject(this, data);
}

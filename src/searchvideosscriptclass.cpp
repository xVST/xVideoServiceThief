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

#include "searchvideosscriptclass.h"

#include "searchvideos.h"

Q_DECLARE_METATYPE(SearchResults)
Q_DECLARE_METATYPE(SearchResults*)
Q_DECLARE_METATYPE(SearchResultsScriptClass*)

// SearchResultsScriptPrototype class

SearchResultsScriptPrototype::SearchResultsScriptPrototype(QObject *parent) : QObject(parent)
{
}

SearchResultsScriptPrototype::~SearchResultsScriptPrototype()
{
}

SearchResults *SearchResultsScriptPrototype::thisSearchResults() const
{
	return qscriptvalue_cast<SearchResults*>(thisObject().data());
}

void SearchResultsScriptPrototype::addSearchResult(QString videoUrl, QString imageUrl, QString title,
												   QString description, int duration, double rating)
{
	thisSearchResults()->addSearchResult(videoUrl, imageUrl, title, description, duration, rating);
}

void SearchResultsScriptPrototype::setSummary(QString value)
{
	thisSearchResults()->setSummary(value);
}

QString SearchResultsScriptPrototype::getUserLanguage()
{
	return thisSearchResults()->getUserLanguage();
}

// SearchResultsScriptClass class

SearchResultsScriptClass::SearchResultsScriptClass(QScriptEngine *engine) : QObject(engine), QScriptClass(engine)
{
	qScriptRegisterMetaType<SearchResults>(engine, toScriptValue, fromScriptValue);

	proto = engine->newQObject(new SearchResultsScriptPrototype(this),
							   QScriptEngine::QtOwnership,
							   QScriptEngine::SkipMethodsInEnumeration
							   | QScriptEngine::ExcludeSuperClassMethods
							   | QScriptEngine::ExcludeSuperClassProperties);
	QScriptValue global = engine->globalObject();
	proto.setPrototype(global.property("Object").property("prototype"));

	ctor = engine->newFunction(construct);
	ctor.setData(qScriptValueFromValue(engine, this));

	engine->globalObject().setProperty("SearchResults", this->constructor());
}

SearchResultsScriptClass::~SearchResultsScriptClass()
{
}

QScriptValue SearchResultsScriptClass::construct(QScriptContext *ctx, QScriptEngine *)
{
	SearchResultsScriptClass *cls = qscriptvalue_cast<SearchResultsScriptClass*>(ctx->callee().data());
	if (!cls) return QScriptValue();
	return cls->newInstance();
}

QScriptValue SearchResultsScriptClass::toScriptValue(QScriptEngine *eng, const SearchResults &searchResults)
{
	QScriptValue ctor = eng->globalObject().property("SearchResults");
	SearchResultsScriptClass *cls = qscriptvalue_cast<SearchResultsScriptClass*>(ctor.data());
	if (!cls) return eng->newVariant(qVariantFromValue(searchResults));
	return cls->newInstance(searchResults);
}

void SearchResultsScriptClass::fromScriptValue(const QScriptValue &obj, SearchResults &searchResults)
{
	searchResults = qscriptvalue_cast<SearchResults>(obj.data());
}

QScriptValue SearchResultsScriptClass::constructor()
{
	return ctor;
}

QString SearchResultsScriptClass::name() const
{
	return QLatin1String("SearchResults");
}

QScriptValue SearchResultsScriptClass::prototype() const
{
	return proto;
}

QScriptValue::PropertyFlags SearchResultsScriptClass::propertyFlags(const QScriptValue&, const QScriptString&, uint)
{
	return QScriptValue::Undeletable;
}

QScriptValue SearchResultsScriptClass::newInstance()
{
	return newInstance(SearchResults());
}

QScriptValue SearchResultsScriptClass::newInstance(const SearchResults &searchResults)
{
	QScriptValue data = engine()->newVariant(qVariantFromValue(searchResults));
	return engine()->newObject(this, data);
}

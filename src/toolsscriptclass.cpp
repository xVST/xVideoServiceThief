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

#include "toolsscriptclass.h"

#include "tools.h"
#include"options.h"
#include "youtubedl.h"

ToolsScriptClass::ToolsScriptClass(QScriptEngine *engine)
{
	// regist getUrlHost(url) function
	QScriptValue _getUrlHost = engine->newFunction(func_getUrlHost);
	engine->globalObject().setProperty("getUrlHost", _getUrlHost);

	// regist getUrlParam(url,param) function
	QScriptValue _getUrlParam = engine->newFunction(func_getUrlParam);
	engine->globalObject().setProperty("getUrlParam", _getUrlParam);

	// regist cleanUrlParam(url) function
	QScriptValue _cleanUrl = engine->newFunction(func_cleanUrl);
	engine->globalObject().setProperty("cleanUrl", _cleanUrl);

	// regist copyBetween(str,from,to) function
	QScriptValue _copyBetween = engine->newFunction(func_copyBetween);
	engine->globalObject().setProperty("copyBetween", _copyBetween);

	// regist getToken(str,separator,token) function
	QScriptValue _getToken = engine->newFunction(func_getToken);
	engine->globalObject().setProperty("getToken", _getToken);

	// regist getTokenCount(str,separator) function
	QScriptValue _getTokenCount = engine->newFunction(func_getTokenCount);
	engine->globalObject().setProperty("getTokenCount", _getTokenCount);

	// regist strRemove(str,pos,len) function
	QScriptValue _strRemove = engine->newFunction(func_strRemove);
	engine->globalObject().setProperty("strRemove", _strRemove);

	// regist strCopy(str,pos,len) function
	QScriptValue _strCopy = engine->newFunction(func_strCopy);
	engine->globalObject().setProperty("strCopy", _strCopy);

	// regist strReplace(str,before,after,[caseSensitive]) function
	QScriptValue _strReplace = engine->newFunction(func_strReplace);
	engine->globalObject().setProperty("strReplace", _strReplace);

	// regist strIndexOf(str,subStr,[from],[caseSensitive]) function
	QScriptValue _strIndexOf = engine->newFunction(func_strIndexOf);
	engine->globalObject().setProperty("strIndexOf", _strIndexOf);

	// regist strLastIndexOf(str,subStr,[from],[caseSensitive]) function
	QScriptValue _strLastIndexOf = engine->newFunction(func_strLastIndexOf);
	engine->globalObject().setProperty("strLastIndexOf", _strLastIndexOf);

	// regist strFormat(str,...) function
	QScriptValue _strFormat = engine->newFunction(func_strFormat);
	engine->globalObject().setProperty("strFormat", _strFormat);

	// regist splitString(str,separator,[splitBehavior],[caseSensitive]) function
	QScriptValue _splitString = engine->newFunction(func_splitString);
	engine->globalObject().setProperty("splitString", _splitString);

	// regist trimString(str) function
	QScriptValue _trimString = engine->newFunction(func_trimString);
	engine->globalObject().setProperty("trimString", _trimString);

	// regist simplifyString(str) function
	QScriptValue _simplifyString = engine->newFunction(func_simplifyString);
	engine->globalObject().setProperty("simplifyString", _simplifyString);

	// regist getMd4(str) function
	QScriptValue _getMd4 = engine->newFunction(func_getMd4);
	engine->globalObject().setProperty("getMd4", _getMd4);

	// regist getMd5(str) function
	QScriptValue _getMd5 = engine->newFunction(func_getMd5);
	engine->globalObject().setProperty("getMd5", _getMd5);

	// regist getSha1(str) function
	QScriptValue _getSha1 = engine->newFunction(func_getSha1);
	engine->globalObject().setProperty("getSha1", _getSha1);

	// regist runYoutubeDL(url) function
	QScriptValue _runYoutubeDL = engine->newFunction(func_runYoutubeDL);
	engine->globalObject().setProperty("runYoutubeDL", _runYoutubeDL);
}

ToolsScriptClass::~ToolsScriptClass()
{
}

QScriptValue ToolsScriptClass::func_getUrlHost(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		// get params
		QString url = context->argument(0).toString();
		// return the asked item from url
		return engine->newVariant(QVariant(QUrl(url).host()));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getUrlParam(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 2)
	{
		// get params
		QString url = context->argument(0).toString();
		QString item = context->argument(1).toString();
		// return the asked item from url
		return engine->newVariant(QVariant(QUrlQuery(url).queryItemValue(item)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_cleanUrl(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		// get params
		QString url = context->argument(0).toString();
		// return the asked item from url
		return engine->newVariant(QVariant(cleanURL(url)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_copyBetween(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 3)
	{
		// get params
		QString str = context->argument(0).toString();
		QString from = context->argument(1).toString();
		QString to = context->argument(2).toString();
		// return the asked item from url
		return engine->newVariant(QVariant(copyBetween(str, from, to)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getToken(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 3)
	{
		// get params
		QString str = context->argument(0).toString();
		QString separator = context->argument(1).toString();
		int token = context->argument(2).toInteger();
		// return the asked item from url
		return engine->newVariant(QVariant(getToken(str, separator, token)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getTokenCount(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 2)
	{
		// get params
		QString str = context->argument(0).toString();
		QString separator = context->argument(1).toString();
		// return the asked item from url
		return engine->newVariant(QVariant(getTokenCount(str, separator)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strRemove(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 3)
	{
		// get params
		QString str = context->argument(0).toString();
		int pos = context->argument(1).toInteger();
		int len = context->argument(2).toInteger();
		// return the asked item from url
		return engine->newVariant(QVariant(str.remove(pos, len)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strCopy(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 3)
	{
		// get params
		QString str = context->argument(0).toString();
		int pos = context->argument(1).toInteger();
		int len = context->argument(2).toInteger();
		// return the asked item from url
		return engine->newVariant(QVariant(copy(str, pos, len)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strReplace(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() >= 3)
	{
		// get params
		QString str = context->argument(0).toString();
		QString before = context->argument(1).toString();
		QString after = context->argument(2).toString();
		// is casesensitive by default
		Qt::CaseSensitivity sensitive = Qt::CaseSensitive;
		if (context->argumentCount() == 4)
			sensitive = context->argument(3).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
		// return the asked item from url
		return engine->newVariant(QVariant(QString(str).replace(before, after, sensitive)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strIndexOf(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() >=2)
	{
		// get params
		QString str = context->argument(0).toString();
		QString subStr = context->argument(1).toString();
		// default index is 0
		int from = 0;
		if (context->argumentCount() >= 3)
			from = context->argument(2).toInteger();
		// is casesensitive by default
		Qt::CaseSensitivity sensitive = Qt::CaseSensitive;
		if (context->argumentCount() >= 4)
			sensitive = context->argument(3).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
		// return the asked item from url
		return engine->newVariant(QVariant(QString(str).indexOf(subStr, from, sensitive)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strLastIndexOf(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() >=2)
	{
		// get params
		QString str = context->argument(0).toString();
		QString subStr = context->argument(1).toString();
		// default index is -1
		int from = -1;
		if (context->argumentCount() >= 3)
			from = context->argument(2).toInteger();
		// is casesensitive by default
		Qt::CaseSensitivity sensitive = Qt::CaseSensitive;
		if (context->argumentCount() >= 4)
			sensitive = context->argument(3).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
		// return the asked item from url
		return engine->newVariant(QVariant(QString(str).lastIndexOf(subStr, from, sensitive)));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_strFormat(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() > 0)
	{
		QString str = context->argument(0).toString();
		// replace occurrences
		for (int n = context->argumentCount() - 1; n > 0; n--)
			str.replace("%" + QString("%1").arg(n) , context->argument(n).toString());
		// return final string
		return engine->newVariant(QVariant(str));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_splitString(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() >=2)
	{
		QString str = context->argument(0).toString();
		QString separator = context->argument(1).toString();
		// keep empty parts by default
		QString::SplitBehavior behavior = QString::KeepEmptyParts;
		if (context->argumentCount() == 3)
			behavior = context->argument(2).toBool() ? QString::KeepEmptyParts : QString::SkipEmptyParts;
		// is casesensitive by default
		Qt::CaseSensitivity sensitive = Qt::CaseSensitive;
		if (context->argumentCount() == 4)
			sensitive = context->argument(3).toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive;
		// return array with string elements
		return qScriptValueFromSequence(engine, str.split(separator, behavior, sensitive));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_trimString(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		QString str = context->argument(0).toString();
		// return final string
		return engine->newVariant(QVariant(str.trimmed()));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_simplifyString(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		QString str = context->argument(0).toString();
		// return final string
		return engine->newVariant(QVariant(str.simplified()));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getMd4(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		QString str = context->argument(0).toString();
		// create crypto class
		QString md4 = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md4).toHex();
		// return calculated hash
		return engine->newVariant(QVariant(md4));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getMd5(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		QString str = context->argument(0).toString();
		// create crypto class
		QString md5 = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Md5).toHex();
		// return calculated hash
		return engine->newVariant(QVariant(md5));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_getSha1(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() == 1)
	{
		QString str = context->argument(0).toString();
		// create crypto class
		QString sha1 = QCryptographicHash::hash(str.toLocal8Bit(), QCryptographicHash::Sha1).toHex();
		// return calculated hash
		return engine->newVariant(QVariant(sha1));
	}
	else // invalid arguments count
		return QScriptValue();
}

QScriptValue ToolsScriptClass::func_runYoutubeDL(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() == 1)
	{
		QString url = context->argument(0).toString();
		// create process
        YoutubeDL youtubeDL(ProgramOptions::instance()->getToolsPath(), ProgramOptions::instance()->getDownloadDir());
        QJsonDocument jsonDocument = youtubeDL.getVideoInformation(url);
        // generate back your json
        QString json(jsonDocument.toJson(QJsonDocument::Compact));
        // scape the json string
        json.replace("\\", "\\\\").replace("\"", "\\\"");
        // evaluate the js program
        return engine->evaluate("JSON.parse('" + json + "')");
	}
	else // invalid arguments count
    {
        return QScriptValue();
    }
}

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

#ifndef TOOLSSCRIPTCLASS_H
#define TOOLSSCRIPTCLASS_H

#include <QtCore>
#include <QtScript>

class ToolsScriptClass : public QObject
{
	private:
		static QScriptValue func_getUrlHost(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getUrlParam(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_cleanUrl(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_copyBetween(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getToken(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getTokenCount(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strRemove(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strCopy(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strReplace(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strIndexOf(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strLastIndexOf(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_strFormat(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_splitString(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_trimString(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_simplifyString(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getMd4(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getMd5(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_getSha1(QScriptContext *context, QScriptEngine *engine);
		static QScriptValue func_runYoutubeDL(QScriptContext *context, QScriptEngine *engine);
	public:
		ToolsScriptClass(QScriptEngine *engine);
		~ToolsScriptClass();
};

#endif // TOOLSSCRIPTCLASS_H

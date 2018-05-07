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

#ifndef WHATSNEWIMPL_H
#define WHATSNEWIMPL_H

#include <QtWidgets>

#include "ui_whatsnew.h"

#if defined STATIC_BUILD && defined Q_OS_WIN32
	#include <QAxWidget>
#else
	#ifndef Q_OS_MACX
        #include <QWebEngineView>
	#else
		class WebViewWidget;
	#endif
#endif

class WhatsNewImpl : public QDialog, public Ui::WhatsNew
{
Q_OBJECT
	private:
#if defined STATIC_BUILD && defined Q_OS_WIN32
	QAxWidget *webView;
#else
	#ifndef Q_OS_MACX
		QWebEngineView *webView;
#else
		WebViewWidget *webView;
	#endif
#endif
	public:
		WhatsNewImpl(QWidget *parent = 0, Qt::WindowFlags f = 0);
		~WhatsNewImpl();
};

#endif // WHATSNEWIMPL_H

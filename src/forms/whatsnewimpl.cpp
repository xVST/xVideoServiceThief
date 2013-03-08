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

#include "whatsnewimpl.h"

#include "../programversion.h"

#include <time.h>

//#ifdef Q_OS_MACX
//#include "../webkit_mac/WebKitClass.h"
//#endif

WhatsNewImpl::WhatsNewImpl(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// create the native Cocoa WebView object (Mac OS X only)
//#ifdef Q_OS_MACX
//	webView = new WebViewWidget(this);
//	webView->setSizeHint(QSize(656,370));
//#else // uses the Qt WebKit wrap
	webView = new QWebView(this);
//#endif
	// add this new object into our frame
	webViewLayout->addWidget(webView);
	webView->adjustSize();
	// add the version id
	QUrlQuery urlQuery;
	urlQuery.addQueryItem("v", PROGRAM_VERSION_SHORT);
	// prevent cache
	srand(time(NULL));
	urlQuery.addQueryItem("nocache", QString("%1").arg(rand() % 999999));
	// creates the whats new url
	QUrl url("http://xviservicethief.sourceforge.net/whatsnew/display.php");
	url.setQuery(urlQuery);
	// open whats new page
	webView->load(url);
}

WhatsNewImpl::~WhatsNewImpl()
{
	delete webView;
}

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
    // create the native IExplorer instance (Windows only)
#if defined STATIC_BUILD && defined Q_OS_WIN32
	//webView = new WebViewWidget(this);
    //webView->setSizeHint(QSize(656,370));
    webView = new QAxWidget(this);
    webView->setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
    webViewFrame->setFrameShadow(QFrame::Plain);
    webViewFrame->setFrameShape(QFrame::NoFrame);
#else // uses the Qt WebKit wrap
	webView = new QWebView(this);
#endif
	// add this new object into our frame
	webViewLayout->addWidget(webView);
	webView->adjustSize();
#if defined STATIC_BUILD && defined Q_OS_WIN32
    webView->dynamicCall("Navigate(const QString&)", "http://xviservicethief.sourceforge.net/whatsnew/display.php");
#else
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
#endif
}

WhatsNewImpl::~WhatsNewImpl()
{
	delete webView;
}

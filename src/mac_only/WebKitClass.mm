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

#include "WebKitClass.h"

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

// Native Cocoa WebKit object

WebViewNative::WebViewNative(QWidget *parent) : QMacCocoaViewContainer(nullptr, parent)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// creates a WebView (WebKit native object)
	WebView *webView = [[WebView alloc] init];
	setCocoaView(webView);
	// set default size
	setSizeHint(QSize(556, 320));
	// release objects
	[webView release];
	[pool release];
}

void WebViewNative::setSizeHint(QSize size)
{
	webViewSize = size;
}

QSize WebViewNative::sizeHint() const
{
	return webViewSize;
}

void WebViewNative::openURL(QString url)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// get the WebView object
	WebView *webView = static_cast<WebView*>(cocoaView());
	// convert the QString to NSString
	NSString *urlText = [[NSString alloc] initWithCString:url.toUtf8() encoding:NSASCIIStringEncoding];
	// load the webpage
	[[webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:urlText]]];
	// release objects
	[urlText release];
	[pool release];
}

// WebView Widget wraper for Qt

WebViewWidget::WebViewWidget(QWidget *parent) : QWidget(parent)
{
	webView = new WebViewNative(this);
}

WebViewWidget::~WebViewWidget()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// release the native WebView object
	delete webView;
	// release pool
	[pool release];
}

void WebViewWidget::setSizeHint(QSize size)
{
	webView->setSizeHint(size);
}

QSize WebViewWidget::sizeHint() const
{
	return webView->sizeHint();
}

void WebViewWidget::load(QUrl url)
{
	webView->openURL(url.toString());
}

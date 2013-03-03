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

#ifndef WEBKITCLASS_H
#define WEBKITCLASS_H

#include <QtWidgets>
#include <QMacCocoaViewContainer>

class WebViewNative : public QMacCocoaViewContainer
{
Q_OBJECT
	private:
		QSize webViewSize;
	public:
		/*! WebView cocoa object constructor */
		WebViewNative(QWidget *parent = 0);
		/*! Set the WebView size */
		void setSizeHint(QSize size);
		/*! Return the default WebView size */
		QSize sizeHint() const;
		/*! Open an specific url */
		void openURL(QString url);
};

class WebViewWidget : public QWidget
{
Q_OBJECT
	private:
		WebViewNative *webView;	//!< WebView cocoa object instance
	public:
		/*! WebView Qt widget constructor */
		WebViewWidget(QWidget *parent = 0);
		/*! WebView Qt widget destructor */
		~WebViewWidget();
		/*! Set the WebView size */
		void setSizeHint(QSize size);
		/*! Return the default WebView size */
		QSize sizeHint() const;
		/*! Open an specific url */
		void load(QUrl url);
};

#endif //WEBKITCLASS_H

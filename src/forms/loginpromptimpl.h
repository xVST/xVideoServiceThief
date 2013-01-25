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

#ifndef LOGINPROMPTIMPL_H
#define LOGINPROMPTIMPL_H

#include <QtGui>

#include "ui_loginprompt.h"

struct ServiceLoginInformation;

class VideoInformationPlugin;

class LoginPromptImpl : public QDialog, private Ui::LoginPrompt
{
Q_OBJECT
	public:
		LoginPromptImpl(VideoInformationPlugin *videoInformationPlugin, ServiceLoginInformation serviceLoginInf,
						bool lastLoginFailed = false, QWidget *parent = 0, Qt::WFlags f = 0);
		ServiceLoginInformation getServiceLoginInfo();
		bool getStayLoggedInSession();
		bool getRememberPassword();
	private slots:
		void edtFieldTextChanged(QString);
};

#endif // LOGINPROMPTIMPL_H

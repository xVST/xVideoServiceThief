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

#include "loginpromptimpl.h"

#include "../serviceskeychain.h"
#include "../videoinformation.h"

LoginPromptImpl::LoginPromptImpl(VideoInformationPlugin *videoInformationPlugin, ServiceLoginInformation serviceLoginInf,
								 bool lastLoginFailed, QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
	// connect slots
	connect(edtUserName, SIGNAL(textChanged(QString)), this, SLOT(edtFieldTextChanged(QString)));
	connect(edtPassword, SIGNAL(textChanged(QString)), this, SLOT(edtFieldTextChanged(QString)));
	// configure info label
	if (lastLoginFailed)
	{
		QString loginInfo = QString(tr("Login failed, please enter your valid %1 user account information:")).arg(videoInformationPlugin->getCaption());
		lblLoginInfo->setText(QString("<font color=red>%1</fond>").arg(loginInfo));
	}
	else // first login
	{
		QString loginInfo = QString(tr("Please, enter your %1 user account information:")).arg(videoInformationPlugin->getCaption());
		lblLoginInfo->setText(loginInfo);
	}
	// bold it
	lblLoginInfo->setText(QString("<b>%1</b>").arg(lblLoginInfo->text()));
	// set initial login info
	edtUserName->setText(serviceLoginInf.userName);
	edtPassword->setText(!lastLoginFailed ? serviceLoginInf.password : "");
	// set focus to username
	if (edtUserName->text().isEmpty()) edtUserName->setFocus();
	else if (edtPassword->text().isEmpty()) edtPassword->setFocus();
	else edtUserName->setFocus();
}

ServiceLoginInformation LoginPromptImpl::getServiceLoginInfo()
{
	return serviceLoginInformation(edtUserName->text(), edtPassword->text());
}

bool LoginPromptImpl::getStayLoggedInSession()
{
	return chbStaySigned->isChecked();
}

bool LoginPromptImpl::getRememberPassword()
{
	return chbRememberPassword->isChecked();
}

void LoginPromptImpl::edtFieldTextChanged(QString)
{
	btnLogin->setEnabled(!edtUserName->text().isEmpty() && !edtPassword->text().isEmpty());
}

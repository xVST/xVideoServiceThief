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
* Program URL   : https://www.xvideothief.com/
*
*/

#include "infoviewimpl.h"

#include "../programversion.h"
#include "../videoitem.h"
#include "../videoinformation.h"

InfoViewImpl::InfoViewImpl(QString info, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
#ifdef Q_OS_MACX
	resize(500, 300);
#endif
	// fill text
	rchInformation->setHtml(info);
}

//


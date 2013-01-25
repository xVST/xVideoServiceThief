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

#include "customalphablendimpl.h"
//
CustomAlphaBlendImpl::CustomAlphaBlendImpl( QWidget * parent, Qt::WFlags f)
		: QDialog(parent, f)
{
	setupUi(this);
	// connect buttons
	connect(btnOk, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	// connect the SldeBar
	connect(hsAlphaBlend, SIGNAL(valueChanged(int)), this, SLOT(hsAlphaBlendvalueChanged(int)));
}

void CustomAlphaBlendImpl::setInitialAlphaBlend(float value)
{
	hsAlphaBlend->setValue(static_cast<int>(value * 100));
	hsAlphaBlendvalueChanged(hsAlphaBlend->value());
}

double CustomAlphaBlendImpl::getAlphaBlend()
{
	if (hsAlphaBlend->value() <= 0)
		return 0.01;
	else
		return static_cast<double>(hsAlphaBlend->value()) / 100;
}

void CustomAlphaBlendImpl::okClicked()
{
	accept();
}

void CustomAlphaBlendImpl::cancelClicked()
{
	reject();
}

void CustomAlphaBlendImpl::hsAlphaBlendvalueChanged(int value)
{
	if (value == 0) value = 1;
	lblCurrentValue->setText(tr("Current: <b>%1%</b>").arg(value));
}
//


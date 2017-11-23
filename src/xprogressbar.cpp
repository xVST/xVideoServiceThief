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

#include "xprogressbar.h"

xProgressBar::xProgressBar(QRect rect, QPainter *painter, int schemaIndex)
{
	// assign internal data
	this->schemaIndex = schemaIndex;
	this->rect = rect;
	this->painter = painter;
	this->progressValue = 0.00;
	this->text = "";
	// set the progress bar colors
	setColor();
	// configure span
	vSpan = 0;
	hSpan = 0;
	// text color
	textColor = QColor("white");
}

void xProgressBar::setColor()
{
	/* TEMPORAL SCHEMA DEFINITION */

	switch (schemaIndex)
	{
		/* blue schema */
		case 0:
			// background
			backgroundBorderColor.setRgb(143, 180, 219);
			backgroundColor.setRgb(198, 209, 221);
			// progress
			gradBorderColor.setRgb(35, 96, 167);
			gradColor1.setRgb(100, 136, 252);
			gradColor2.setRgb(165, 183, 240);
			// ok
			break;

		/* green schema */
		case 1:
			// background
			backgroundBorderColor.setRgb(3, 194, 26);
			backgroundColor.setRgb(76, 214, 93);
			// progress
			gradBorderColor.setRgb(8, 77, 16);
			gradColor1.setRgb(0, 137, 16);
			gradColor2.setRgb(78, 194, 81);
			// ok
			break;

		/* red schema */
		case 2:
			// background
			backgroundBorderColor.setRgb(255, 62, 62);
			backgroundColor.setRgb(248, 175, 175);
			// progress
			gradBorderColor.setRgb(151, 0, 0);
			gradColor1.setRgb(251, 54, 54);
			gradColor2.setRgb(246, 118, 118);
			// ok
			break;

		/* gray schema */
		case 3:
			// background
			backgroundBorderColor.setRgb(116, 177, 160);
			backgroundColor.setRgb(231, 231, 231);
			// progress
			gradBorderColor.setRgb(106, 106, 106);
			gradColor1.setRgb(168, 168, 168);
			gradColor2.setRgb(197, 197, 197);
			// set text color (white is not a good option)
			textColor = QColor(58, 58, 58);
			// ok
			break;

		/* yellow schema */
		case 4:
			// background
			backgroundBorderColor.setRgb(227, 204, 79);
			backgroundColor.setRgb(255, 236, 130);
			// progress
			gradBorderColor.setRgb(215, 182, 0);
			gradColor1.setRgb(233, 197, 0);
			gradColor2.setRgb(255, 236, 130);
			// set text color (white is not a good option)
			textColor = QColor(215, 71, 0);
			// ok
			break;

		/* black schema */
		case 5:
			// background
			backgroundBorderColor.setRgb(99, 99, 99);
			backgroundColor.setRgb(134, 134, 134);
			// progress
			gradBorderColor.setRgb(0, 0, 0);
			gradColor1.setRgb(38, 38, 38);
			gradColor2.setRgb(113, 113, 113);
			// ok
			break;

		/* purple schema */
		case 6:
			// background
			backgroundBorderColor.setRgb(234, 127, 223);
			backgroundColor.setRgb(255, 164, 246);
			// progress
			gradBorderColor.setRgb(150, 0, 134);
			gradColor1.setRgb(218, 0, 195);
			gradColor2.setRgb(255, 121, 241);
			// ok
			break;

		/* maroon schema */
		case 7:
			// background
			backgroundBorderColor.setRgb(255, 174, 49);
			backgroundColor.setRgb(255, 204, 132);
			// progress
			gradBorderColor.setRgb(159, 94, 0);
			gradColor1.setRgb(223, 134, 6);
			gradColor2.setRgb(248, 170, 59);
			// ok
			break;

		/* clean schema */
		case 8:
			// background
			backgroundBorderColor.setRgb(143, 180, 219);
			backgroundColor.setRgb(198, 209, 221);
			// progress
			gradBorderColor.setRgb(209, 128, 24);
			gradColor1.setRgb(246, 199, 138);
			gradColor2.setRgb(255, 227, 190);
			// ok
			break;

		/* light gray */
		case 9:
			// background
			backgroundBorderColor.setRgb(194, 194, 194);
			backgroundColor.setRgb(232, 233, 233);
			// progress
			gradBorderColor.setRgb(176, 176, 176);
			gradColor1.setRgb(201, 201, 201);
			gradColor2.setRgb(223, 223, 223);
			// set text color (white is not a good option)
			textColor = QColor(58, 58, 58);
			// ok
			break;
	}
}

void xProgressBar::paint()
{
	// paint the progressBar background
	painter->setBrush(backgroundColor);
	painter->setPen(backgroundBorderColor);
	painter->drawRect(rect.x() + hSpan, rect.y() + vSpan, rect.width() - 1 - hSpan, rect.height() - 1 - vSpan * 2);

	// define gradient
	QLinearGradient linearGrad(rect.x(), rect.y(), rect.x(), rect.y() + rect.height() - 1);

	linearGrad.setColorAt(0.00, gradColor1);
	linearGrad.setColorAt(0.16, gradColor2);
	linearGrad.setColorAt(1.00, gradColor1);
	painter->setPen(gradBorderColor);

	// calculate progress value
	int preWidth = static_cast<int>((rect.width() - 1 - hSpan)*(progressValue/100));
	int progressWidth = rect.width() - preWidth;

	// paint the progress (only if have somethin to paint...)
	if (progressWidth != rect.width() - hSpan)
	{
		painter->setBrush(linearGrad);
		painter->drawRect(rect.x() + hSpan, rect.y() + vSpan, rect.width() - progressWidth - hSpan, rect.height() - 1 - vSpan * 2);
	}

	// paint text?
	if (displayText)
	{
		painter->setPen(textColor);

		if (text.isEmpty())
		{
			QLocale locale;
			painter->drawText(rect, Qt::AlignCenter, locale.toString(progressValue, 'f', 2) + "%");
		}
		else
			painter->drawText(rect, Qt::AlignCenter, text);
	}
}

void xProgressBar::setColorSchema(const int value)
{
	schemaIndex = value;
	// set the progress bar colors
	setColor();
}

void xProgressBar::setValue(const float value)
{
	progressValue = value < 0 ? 0 : value > 100 ? 100 : value;
}

void xProgressBar::setText(const QString value)
{
	text = value;
}

void xProgressBar::setDisplayText(const bool display)
{
	displayText = display;
}

void xProgressBar::setVerticalSpan(const int value)
{
	vSpan = value;
}

void xProgressBar::setHorizontalSpan(const int value)
{
	hSpan = value;
}

QPixmap xProgressBar::getPixmap(QSize size, float progressValue, bool displayText, int schemaIndex)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);

	xProgressBar progressBar(QRect(0, 0, size.width(), size.height()), &painter, schemaIndex);
	progressBar.setValue(progressValue);
	progressBar.setDisplayText(displayText);
	progressBar.paint();

	return pixmap;
}

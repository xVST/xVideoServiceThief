/*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - %year% Xesc & Technology
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
*/

function RegistVideoService()
{
	this.version = "%version%";
	this.minVersion = "3.0.0";
	this.author = "Xesc & Technology %year%";
	this.website = "%url%";
	this.ID = "%id%";
	this.caption = "%title%";
	this.adultContent = %adultContent%;
	this.musicSite = %musicSite%;
	this.useYoutubeDL = true;
}

function getVideoServiceRegexMatchers()
{
	return %matchers%;
}

function getVideoServiceIcon()
{
	return %favicon%;
}

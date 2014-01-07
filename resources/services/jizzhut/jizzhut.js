/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2014 Xesc & Technology
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

function RegistVideoService()
{
	this.version = "1.0.2";
	this.minVersion = "2.0.0a";
	this.author = "Xesc & Technology 2014";
	this.website = "http://www.jizzhut.com/";
	this.ID = "jizzhut.com";
	this.caption = "Jizz-hut";
	this.adultContent = true;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_XML = "http://cdnt.tnaflix.com/";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	// download html
	var html = http.downloadWebpage(url);
	// get video information
	result.title = copyBetween(html, "<title>", "</title");
	result.URL = copyBetween(html, 'encodeURIComponent("', '"');
	// return the video information
	return result;
}

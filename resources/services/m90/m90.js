/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2011 Xesc & Technology
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
	this.version = "1.0.0";
	this.minVersion = "2.0.0a";
	this.author = "crapmaster & Xesc & Technology 2011";
	this.website = "http://www.m90.org";
	this.ID = "m90.org";
	this.caption = "m90";
	this.adultContent = true;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	// video information
	var result = new VideoDefinition();

	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);

	// get video title
	var tmp = copyBetween(html, '<!-- <div class="yellowArea headBox"> -->', '<!-- </div> -->');
	result.title = copyBetween(tmp, '<h3>', '</h3>');

	// get the video  url
	var videoUrl = copyBetween(html, "so.addVariable('file','", "');");

	result.URL=cleanUrl(videoUrl);

	// return the video information
	return result;
}

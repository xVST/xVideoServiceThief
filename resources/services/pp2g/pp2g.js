/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2009 Xesc & Technology
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
	this.author = "Xesc & Technology 2009";
	this.website = "http://www.pp2g.tv/";
	this.ID = "pp2g.tv";
	this.caption = "PP2G TV";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const FLV_URL = "http://videos.pp2g.tv/f%1.flv";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);
	// get video information
	result.title = copyBetween(html, 'class="TitleContainer bgSection">', '<');
	// get video id
	var videoId = copyBetween(html, "'flashvars','", "'");
	videoId = copyBetween(videoId, "r=", ";");
	// build flv url
	result.URL = strFormat(FLV_URL, videoId);
	// return the video information
	return result;
}

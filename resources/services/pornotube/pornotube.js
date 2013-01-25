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
	this.website = "http://www.pornotube.com/";
	this.ID = "pornotube.com";
	this.caption = "PornoTube";
	this.adultContent = true;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const AGE_VALIDATION = "bMonth=01&bDay=1&bYear=1975&verifyAge=true&submit=View+All+Content";
	const GET_FLV_INFO_URL = "http://www.pornotube.com/player/player.php?%1";
	const GET_FLV_URL = "http://%1.pornotube.com/%2/%3.flv";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	// add refer header
	http.addHeaderParameter("Referer", url);
	// download html
	var html = http.downloadWebpagePost(url, AGE_VALIDATION);
	// get video title
	result.title = copyBetween(html, '<span class="blue">', "<");
	// get the video ID
	var videoID = copyBetween(html, "player/v.swf?v=", '"');
	// get the video Information
	http.clearHeaderParameters();
	info = http.downloadWebpage(strFormat(GET_FLV_INFO_URL, videoID));
	// get the video ID, User ID and domain
	var flvId = copyBetween(info, "mediaId=", "&");
	var userId = copyBetween(info, "userId=", "&");
	var domain = copyBetween(info, "mediaDomain=", "&");
	// build url
	result.URL = cleanUrl(strFormat(GET_FLV_URL, domain, userId, flvId));
	// return the video information
	return result;
}

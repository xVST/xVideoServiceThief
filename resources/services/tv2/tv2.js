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
	this.website = "http://tv2.hu/";
	this.ID = "tv2.hu";
	this.caption = "TV2";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_XML = "http://webcast.tv2.hu/dynamic/v2.php?video_id=%1&t=iframe&ap=1";
	// init result
	var result = new VideoDefinition();
	// create http class
	var http = new Http();
	// check if is "tv2.hu", if yes then download the html and get the video id
	if (getUrlHost(url) == "tv2.hu")
	{
		// download html
		var html = http.downloadWebpage(url);
		// get the "webcast.tv2.hu" url
		url = copyBetween(html, 'frameborder="0" src="', '">');
	}
	// get videoId
	var videoId = getUrlParam(url, "video_id");
	// download xml
	var xml = http.downloadWebpage(strFormat(URL_XML, videoId));
	// remove the first <video></video> xml node
	var length = xml.toString().indexOf("</video>");
	xml = strRemove(xml, 0, length);
	// get video title
	result.title = copyBetween(xml, "<title>", "</title>");
	result.title = copyBetween(result.title, "<![CDATA[", "]]>");
	// get video url
	result.URL = copyBetween(xml, "<uri", "</uri>");
	result.URL = copyBetween(result.URL, "<![CDATA[", "]]>");
	// return the video information
	return result;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x06,0x00,0x00,0x00,0x1f,0xf3,0xff,
		0x61,0x00,0x00,0x00,0x3f,0x49,0x44,0x41,0x54,0x38,0xcb,0x63,0xf8,0xcf,0xcb,0xf8,
		0x1f,0x1f,0x66,0x20,0x04,0xc8,0xd6,0x48,0x53,0x03,0x88,0xd6,0x4c,0xac,0x01,0x78,
		0x0d,0x27,0x14,0x88,0x04,0x5d,0x47,0x57,0x03,0x48,0x8a,0x05,0x62,0x02,0x15,0x2c,
		0x47,0x89,0x66,0x82,0x06,0x10,0x13,0x73,0x24,0x27,0x24,0x0c,0x75,0x74,0x49,0x48,
		0x83,0xdb,0x00,0x00,0x5d,0xed,0x06,0x10,0x44,0xdb,0x1a,0x3c,0x00,0x00,0x00,0x00,
		0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

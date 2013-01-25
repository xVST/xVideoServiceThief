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
	this.website = "http://www.liveleak.com/";
	this.ID = "liveleak.com";
	this.caption = "LiveLeak";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_GET_XML = "http://www.liveleak.com/playlist.php?file_embed_tag=%1";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);
	// get video title
	result.title = copyBetween(html, "<title>LiveLeak.com -", "</title>");
	// get the video parameters
	var file_embed_tag = copyBetween(html, "embed_tag=", "&autostart=true");
	// download the video info
	var xml = http.downloadWebpage(strFormat(URL_GET_XML, file_embed_tag));
	// get flv url
	result.URL = cleanUrl(copyBetween(xml, "<location>", "</location>"));
	// return the video information
	return result;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x06,0x00,0x00,0x00,0x1f,0xf3,0xff,
		0x61,0x00,0x00,0x00,0x35,0x49,0x44,0x41,0x54,0x38,0x8d,0x63,0x98,0xc9,0xc0,0xf0,
		0x97,0x12,0xcc,0x40,0x48,0xc1,0xff,0xff,0xff,0x71,0x62,0xb8,0x01,0xc8,0x02,0xd8,
		0x0c,0xc0,0x65,0xd8,0xa8,0x01,0xa3,0x06,0xd0,0xc6,0x00,0x74,0x49,0x74,0x03,0xb0,
		0x19,0x4c,0x1d,0x03,0x28,0xce,0x4c,0x94,0x60,0x00,0xad,0xe2,0x80,0x67,0x2e,0xd7,
		0x52,0x21,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

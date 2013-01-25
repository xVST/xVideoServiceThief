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
	this.website = "http://www.livevideo.com/";
	this.ID = "livevideo.com";
	this.caption = "LiveVideo";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_GET_XML = "http://www.livevideo.com/media/getflashvideo.ashx?cid=%1&path=%2&t=%3&mid=%4&et=%5&f=flash8";
	const EXTRA_INFO = "LVX*7x8yzwe";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);
	// get video info
	result.title = copyBetween(html, "<title>", "</title>");
	// remove extra information form title
	result.title = strReplace(result.title, "- LiveVideo.com", "");
	result.title = strReplace(result.title, "- Channel", "");
	// get video ID (from the URL)
	var cid = getToken(url, "/", getTokenCount(url, "/") - 2);
	// get T parameter
	var T = copyBetween(html, "rel=\"videothumbnail\"", "/>");
	T = copyBetween(T, "livevideo.com", "\"");
	// get path parameter
	var path = getToken(T, "/", 2) + "/" + getToken(T, "/", 3);
	// get mid parameter
	var mid = getToken(T, "/", 4);
	mid = strCopy(mid, 0, strIndexOf(mid, "_"));
	// get et parameter
	var et = copyBetween(html, "et%3d", "'");
	// generate the xml url
	var xmlUrl = strFormat(URL_GET_XML, cid, path, T, mid, et);
	// download the xml file
	var xml = http.downloadWebpage(xmlUrl + "&h=" + getMd5(xmlUrl + EXTRA_INFO) + "&?");
	// get the FLV url
	result.URL = cleanUrl(copyBetween(xml, "video_id=", "&"));
	// get cookies
	result.cookies = http.getCookies("|");
	// return the video information
	return result;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x06,0x00,0x00,0x00,0x1f,0xf3,0xff,
		0x61,0x00,0x00,0x00,0x55,0x49,0x44,0x41,0x54,0x38,0xcb,0x63,0x60,0xa0,0x01,0xf8,
		0x0f,0xc5,0xc4,0x8a,0x13,0x65,0x08,0x49,0x9a,0xd1,0x35,0x91,0xa5,0x99,0x6c,0x9b,
		0xff,0x13,0x89,0x29,0xd2,0x8c,0xd3,0x10,0xb0,0xc4,0x86,0x1b,0x2f,0xfe,0x9f,0x78,
		0xf1,0x03,0x05,0xdf,0xf8,0x80,0xc0,0x04,0x0d,0xc0,0xa7,0x99,0x64,0x03,0xd0,0x35,
		0x7f,0xf8,0x41,0x82,0x01,0xd8,0x34,0x13,0x6d,0x00,0x2e,0xcd,0x44,0x19,0x80,0x4f,
		0x33,0x51,0x06,0xd0,0x2b,0x2d,0x50,0x0f,0x00,0x00,0xc9,0x85,0x05,0x2e,0xfe,0x26,
		0xdd,0xd5,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

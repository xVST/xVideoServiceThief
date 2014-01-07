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
	this.version = "1.0a";
	this.minVersion = "2.0.0a";
	this.author = "Xesc & Technology 2014";
	this.website = "http://www.myspace.com/";
	this.ID = "myspace.com";
	this.caption = "MySpace";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_GET_XML_MUSIC = "http://www.myspace.com/music/services/player?action=getSong&songId=%1";
	const URL_GET_XML_VIDEO = "http://mediaservices.myspace.com/services/rss.ashx?type=video&videoID=%1";
	// detect if is a video or music and configure the XML url
	var isMusic = strIndexOf(url, "myspace.com/video/") == -1;
	var URL_GET_XML = isMusic ? URL_GET_XML_MUSIC : URL_GET_XML_VIDEO;
	// get video/audio id
	var id = splitString(url, isMusic ? "-" : "/");
	id = id[id.length - 1];
	// init result
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var xml = http.downloadWebpage(strFormat(URL_GET_XML, id));
	// parse info
	result = isMusic ? parseMusicXML(xml, result) : parseVideoXML(xml, result);
	// update extension
	result.extension = splitString(result.URL, ".");
	result.extension = "." + result.extension[result.extension.length - 1];
	// return the video information
	return result;
}

function parseVideoXML(xml, videoDefinition)
{
	var item = copyBetween(xml, "<item>", "</item>");
	// get music info
	videoDefinition.title = copyBetween(item, "<title>", "</title>");
	videoDefinition.URL = copyBetween(item, '<media:content url="', '"');
	// replace some special chars
	videoDefinition.title = strReplace(videoDefinition.title, "&amp;quot;", '"');
	// get artist (if exists)
	var artist = copyBetween(xml, "<myspace:artistName>", "</myspace:artistName>");
	if (artist != "") videoDefinition.title = artist + " - " + videoDefinition.title;
	// return updated info
	return videoDefinition;
}

function parseMusicXML(xml, videoDefinition)
{
	// get music info
	videoDefinition.title = copyBetween(xml, "<title>", "</title>");
	videoDefinition.URL = copyBetween(xml, "<rtmp>", "</rtmp>");
	// get artist (if exists)
	var artist = copyBetween(xml, '<artist name="', '"');
	if (artist != "") videoDefinition.title = artist + " - " + videoDefinition.title;
	// finish config
	videoDefinition.isAudioFile = true;
	// return updated info
	return videoDefinition;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x06,0x00,0x00,0x00,0x1f,0xf3,0xff,
		0x61,0x00,0x00,0x00,0x84,0x49,0x44,0x41,0x54,0x78,0xda,0xcd,0x53,0x41,0x0a,0x00,
		0x21,0x08,0x8c,0x4e,0x9d,0xfa,0x4c,0xef,0xe8,0x1b,0x81,0xef,0xeb,0xda,0xdf,0xdc,
		0x9d,0x60,0x20,0xa8,0x8b,0xb6,0x87,0x0d,0x44,0x13,0x67,0x52,0xd3,0xa0,0xaa,0xa1,
		0xd6,0xaa,0x1e,0x01,0x76,0x82,0x4b,0x29,0xaf,0x15,0x4c,0x02,0x0c,0xb0,0x20,0x80,
		0xc3,0x23,0x7f,0x23,0x18,0x63,0xa8,0x88,0x50,0xd3,0x46,0xad,0x53,0x33,0x8e,0xb1,
		0x1b,0x01,0x4f,0xef,0x7d,0xb3,0x5b,0x6b,0x53,0xa7,0x94,0x00,0x3e,0x12,0x30,0xf0,
		0x64,0xaf,0x3e,0x6a,0x33,0x01,0x5f,0xe6,0xdd,0x46,0x80,0x3e,0xa0,0x27,0x2c,0x2b,
		0xc6,0x68,0x26,0x58,0xfd,0xfe,0x6f,0xc4,0x71,0xcf,0x01,0xb2,0x40,0xfa,0x39,0xe7,
		0x6f,0x07,0xe9,0x7a,0x99,0xae,0xd7,0xf9,0x01,0x7f,0x7c,0xb6,0xbb,0xa4,0x1c,0x9b,
		0x71,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

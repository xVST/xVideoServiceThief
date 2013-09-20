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
	this.version = "1.1.0";
	this.minVersion = "2.0.0a";
	this.author = "anon";
	this.website = "http://screen.yahoo.com/";
	this.ID = "screen.yahoo.com";
	this.caption = "Yahoo Video";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	var url_get_json ="http://video.query.yahoo.com/v1/public/yql?q=SELECT streams FROM yahoo.media.video.streams WHERE id=\"%1\" AND plrs=\"\" AND protocol=\"http\" AND region=\"%2\";&env=prod&format=json";
	// init result
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);
	// get video title
	var title = copyBetween(html, "<title>", "</title>");
	//if we have the ?format=embed parameter added, the | is missing
	if (title.toString().indexOf(" |") != -1)
		title = copyBetween(title, "", " |");
	result.title = title;
	// get the node_id
	var node_id = copyBetween(html, "\"id\":\"", "\"");
	// get the region
	var region = copyBetween(html, "region: \'", "\'");
	// download json
	var jsonstr = http.downloadWebpage(strFormat(url_get_json, node_id, region));
	var json = JSON.parse(jsonstr);
	var streams = json.query.results.mediaObj[0].streams;
	var streamindex = streams.length - 1; //the streams are sorted by quality. highest is last.
	// get video host and path
	var host = streams[streamindex].host; //copyBetween(json, "\"host\":\"", "\"");
	var path = streams[streamindex].path; //copyBetween(json, "\"path\":\"", "\"")
	// set video URL
	result.URL = host + path; //take the video with highest quality
	result.extension = extensionFromVideoType(streams[streamindex].mime_type);
	// return the video information
	return result;
}

function extensionFromVideoType(vtype)
{
	if (vtype == "video/x-flv") return ".flv";
	if (vtype == "video/mp4") return ".mp4";
	if (vtype == "video/webm") return ".webm";
	// default extension
	return ".flv";
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x06,0x00,0x00,0x00,0x1f,0xf3,0xff,
		0x61,0x00,0x00,0x00,0x45,0x49,0x44,0x41,0x54,0x78,0xda,0xed,0x91,0x31,0x0e,0x00,
		0x20,0x0c,0x02,0xf9,0xff,0xa7,0x6b,0x17,0x17,0x92,0x23,0x61,0x35,0x36,0xb2,0x68,
		0xb9,0x1b,0xd4,0x1f,0x69,0xf6,0x50,0x0a,0x8c,0x15,0x6f,0x40,0x80,0x12,0x7a,0x30,
		0x30,0xec,0x01,0xc4,0xcb,0x70,0x47,0x10,0x2e,0x33,0x20,0x5b,0xb2,0xbd,0x58,0x9c,
		0xfa,0x5b,0x0d,0x9a,0xe5,0x10,0xb3,0x3f,0x33,0x07,0xa6,0xb3,0x7e,0x82,0xcd,0x2f,
		0x11,0x53,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}
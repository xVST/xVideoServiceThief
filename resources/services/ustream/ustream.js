/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2010 Xesc & Technology
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
	this.author = "Xesc & Technology 2010";
	this.website = "http://www.ustream.tv/";
	this.ID = "ustream.tv";
	this.caption = "UStream";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{	
	const URL_GET_FLV = "http://ustream.vo.llnwd.net/pd5/%1.flv";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	
	var post = http.downloadWebpagePost("http://216.52.240.138/gateway.php", getGatewayPostParams(url));
	
	var html = http.downloadWebpage(url);
	// get video title
	result.title = copyBetween(html, '<h2 id="VideoTitle">', '</h2>');
	// check if a live http url
	if (html.toString().indexOf("ustream.vars.liveHttpUrl") != -1)
	{
		result.URL = strReplace(copyBetween(html, 'ustream.vars.liveHttpUrl="', '"'), "\\/", "/");
		// get video extension
		result.extension = ".mp4";
	}
	else // is not live http url, we must "play" around to get the flv
	{
		var imgUrl = copyBetween(html, '<link rel="image_src" href="', '"');
		// get url path
		var urlPath = copyBetween(imgUrl, "videopic/", "_320x240");
		// build final url
		result.URL = strFormat(URL_GET_FLV, urlPath);
	}
	// return the video information
	return result;
}

function getGatewayPostParams(url)
{
	// create special chars alias
	var NUL = String.fromCharCode(0);
	var SOH = String.fromCharCode(1);
	var STX = String.fromCharCode(2);
	var ETX = String.fromCharCode(3);
	var EOT = String.fromCharCode(4);
	var BEL = String.fromCharCode(7);
	var BS  = String.fromCharCode(8);
	var SI  = String.fromCharCode(15);
	var CAN = String.fromCharCode(24);

	var C2  = String.fromCharCode(194);
	
	// video id
	var videoId = "9276623";
	
	// post data stream
	var postData = NUL + NUL + NUL + NUL + NUL + SOH + NUL + SI + "Viewer.getVideo" + NUL + STX;	
	postData    += "/" + "1" + NUL + NUL + NUL + C2 + NUL + NUL + NUL + SOH + ETX + NUL + BS + "autoplay";
	postData    += SOH + SOH + NUL + EOT + "rpin" + STX + NUL + CAN + "rpin.0.09268754328445232";
	postData    += NUL + BEL + "pageUrl" + STX + NUL + "&" + url + NUL + BEL + "videoId" + STX + NUL + BEL + videoId;
	postData    += NUL + BEL + "brandId" + STX + NUL + SOH + "1" + NUL + NUL;

	print(postData.length);

	return postData;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x04,0x03,0x00,0x00,0x00,0xed,0xdd,0xe2,
		0x52,0x00,0x00,0x00,0x12,0x50,0x4c,0x54,0x45,0x5d,0x96,0xd0,0x67,0x9b,0xd3,0x6b,
		0x9e,0xd4,0x76,0xa3,0xd7,0x85,0xab,0xdb,0xff,0xff,0xff,0x8b,0x2a,0x8f,0xf4,0x00,
		0x00,0x00,0x35,0x49,0x44,0x41,0x54,0x08,0x5b,0x63,0x08,0x09,0x0d,0x75,0x71,0x71,
		0x09,0x0d,0x65,0x70,0xa1,0x9c,0xe1,0x2c,0x08,0x61,0x30,0x30,0x30,0xa0,0x31,0x02,
		0xc0,0x0c,0x27,0x56,0x20,0x01,0xc2,0x0e,0x40,0x22,0x14,0x2c,0xca,0x00,0x66,0x30,
		0x30,0x04,0x80,0xc4,0x43,0x03,0x58,0x01,0x29,0xb0,0x21,0xa9,0x04,0xf1,0xf7,0x20,
		0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

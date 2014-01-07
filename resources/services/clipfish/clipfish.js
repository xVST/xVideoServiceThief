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
	this.version = "2.0.2";
	this.minVersion = "2.0.0a";
	this.author = "Xesc & Technology 2014";
	this.website = "http://www.clipfish.de/";
	this.ID = "clipfish.de";
	this.caption = "Clipfish";
	this.adultContent = false;
	this.musicSite = false;
}

function getVideoInformation(url)
{
	const URL_GET_XML = "http://www.clipfish.de/video_n.php?vid=%1";
	// video information
	var result = new VideoDefinition();
	// download webpage
	var http = new Http();
	var html = http.downloadWebpage(url);
	// get the video title
	result.title = copyBetween(html, "<span class=\"cf-headline cf-bold\">", "</span>");
	// get the video id
	var vidId = copyBetween(html, "var video = {id: ", " ");
	// download xml
	var xml = http.downloadWebpage(strFormat(URL_GET_XML, vidId));
	// build final url
	result.URL = copyBetween(xml, "=", "&");
	// return the video information
	return result;
}

//http://www.clipfish.de/suche/?search=Will+Smith
//http://www.clipfish.de/suche/Will+Smith/video/bestertreffer/2/listenansicht/?search=Will%20Smith


function searchVideos(keyWord, pageIndex)
{
	const URL_SEARCH = "http://www.clipfish.de/suche/%1/video/bestertreffer/%2/listenansicht/?search=%1";
	const HTML_SEARCH_START = '<ul class="cf-video-list cf-list-linear">'; //"<!-- GENERIC BOX BEGIN -->";
	const HTML_SEARCH_FINISH = '<!-- GENERIC BOX END -->';
	const HTML_SEARCH_SEPARATOR = '<li id="cf-video-item_';
	const HTML_ACCESS_DENIED = "http://bilder.clipfish.de/v2/access_denied.jpg";
	// replace all spaces for "+"
	keyWord = strReplace(keyWord, " ", "+");
	// init search results object
	var searchResults = new SearchResults();
	// init http object
	var http = new Http();
	var html = http.downloadWebpage(strFormat(URL_SEARCH, keyWord, pageIndex));
	// access denied?
	if (strIndexOf(htmlResults, HTML_ACCESS_DENIED) != -1) return searchResults;
	// get the search summary
	var tmp = copyBetween(html, '>Deine Suche nach', 'Treffer ergeben.');
	var summary = "Deine Suche nach " + cleanwhitespace(cleartags(tmp, 1)) + ' Treffer ergeben.';
	var tmp = copyBetween(html, "Suchergebnisse", "</span></span>");
	var summary = summary + " (" + cleanwhitespace(cleartags(tmp, 1)) + ")";
	searchResults.setSummary(summary);
	// get results html block
	var htmlResults = copyBetween(html, HTML_SEARCH_START, HTML_SEARCH_FINISH);
	if (strIndexOf(htmlResults, HTML_SEARCH_SEPARATOR) != -1) htmlResults = strRemove(htmlResults, 0, strIndexOf(htmlResults, HTML_SEARCH_SEPARATOR));
	// if we found some results then...
	if (htmlResults != "")
	{
		var block = "";
		// iterate over results
		while ((block = copyBetween(htmlResults, HTML_SEARCH_SEPARATOR, HTML_SEARCH_SEPARATOR)) != "")
		{
			parseResultItem(searchResults, block);
			htmlResults = strRemove(htmlResults, 0, block.toString().length + HTML_SEARCH_SEPARATOR.toString().length);
		}
		// get last result
		parseResultItem(searchResults, htmlResults);
	}
	// return search results
	return searchResults;
}

function parseResultItem(searchResults, html)
{
	const VIDEO_URL = "http://www.clipfish.de";
	// vars
	var tmp, videoUrl, imageUrl, title, description, duration, rating;
	// get video url
	videoUrl = VIDEO_URL + copyBetween(html, '<a href="', '"');
	// get title and image url
	title = trim(copyBetween(html, 'title="', '"'), " ");
	imageUrl = copyBetween(html, '<img src="', '"');
	//if (strIndexOf(imageUrl, "default.jpg") == -1) // if is not a "default.jpg"...
	//	imageUrl = copyBetween(tmp, 'thumb="', '"');
	// get video description
	tmp = copyBetween(html, '<div class="cf-video-list-item-info cf-gray">', '</div>');
	description = trim(cleartags(copyBetween(tmp, "<p>", "</p>"), 0), " ");
	description = (strIndexOf(description, "Video-L&auml;nge:") == -1 ? description : "Keine Beschreibung vorhanden.");
	// get video duration
	duration = convertToSeconds(trim(cleartags(copyBetween(tmp, "Video-L&auml;nge:", '</span>'), 0), " "));
	// get rating
	rating = getrating(copyBetween(html, '<div class="cf-voting-fishes">', '</div>'));
	// add to results list
	searchResults.addSearchResult(videoUrl, imageUrl, title, description, duration, rating);
}

function getrating(text)
{
	var rating = 0
	var i = 1
	while (i < 6)
	{
		var part = getToken(text, '<img', i);
		if (strIndexOf(part, "cf-voting-fish-full") != -1)
		{
			rating = rating + 1
		}
		if (strIndexOf(part, "cf-voting-fish-half") != -1)
		{
			rating = rating + 0.5
		}
		i++
	}
	return rating;
}

function cleanwhitespace(text)
{
	text = trim(text, " ");
	text = trim(text, "	");
	text = cleandoublevalue(text, " ");
	text = cleandoublevalue(text, "	");
	text = strReplace(text, "\n", "");
	text = strReplace(text, "\r", "");
	return text;
}

function trim(text, value)
{
	while (strIndexOf(text, value) == 0)
	{
		text = strRemove(text, 0, 1);
	}
	while (strLastIndexOf(text, value, -1) == text.toString().length - 1 && text != "")
	{
		text = strRemove(text, text.toString().length - 1, 1);
	}
	return text;
}

function cleandoublevalue(text, value)
{
	while (strIndexOf(text, value + value) != -1)
	{
		text = strReplace(text, value + value, value);
	}
	return text;
}

function cleartags(text, killopen)
{
	var cleartext = text
	if (killopen == 1)
	{
		if (strIndexOf(cleartext, ">") < strIndexOf(cleartext, "<"))
		{
			cleartext = strRemove(cleartext, 0, strIndexOf(cleartext, ">") + 1);
		}
		if (strLastIndexOf(cleartext, ">", -1) < strLastIndexOf(cleartext, "<", -1))
		{
			var tmp = strLastIndexOf(cleartext, "<", -1);
			cleartext = strRemove(cleartext, tmp, cleartext.toString().length - tmp);
		}
	}
	while (strIndexOf(cleartext, ">") != -1 && strIndexOf(cleartext, "<") != -1)
	{
		var start = strIndexOf(cleartext, "<");
		var stop = strIndexOf(cleartext, ">");
		cleartext = strRemove(cleartext, start, stop - start + 1);
	}
	return cleartext;
}

function convertToSeconds(text)
{
	// how many ":" exists?
	var count = getTokenCount(text, ":");
	// get mins and seconds
	var h = new Number(h = count == 3 ? getToken(text, ":", 0) * 3600 : 0);
	var m = new Number(getToken(text, ":", count - 2) * 60);
	var s = new Number(getToken(text, ":", count - 1));
	// convert h:m:s to seconds
	return h + m + s;
}

function getVideoServiceIcon()
{
	return new Array(
		0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
		0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x08,0x03,0x00,0x00,0x00,0x28,0x2d,0x0f,
		0x53,0x00,0x00,0x00,0x33,0x50,0x4c,0x54,0x45,0xc9,0x11,0x27,0xcc,0x20,0x35,0xd0,
		0x2f,0x42,0xd3,0x3e,0x50,0xd7,0x4d,0x5d,0xda,0x5c,0x6b,0xdd,0x6b,0x78,0xe1,0x7a,
		0x86,0xe4,0x88,0x93,0xe7,0x96,0xa0,0xeb,0xa5,0xae,0xee,0xb4,0xbb,0xf1,0xc3,0xc9,
		0xf5,0xd2,0xd6,0xf8,0xe1,0xe4,0xfc,0xf0,0xf1,0xff,0xff,0xff,0xa6,0xa4,0x90,0x07,
		0x00,0x00,0x00,0x71,0x49,0x44,0x41,0x54,0x18,0x57,0x8d,0xcd,0x3b,0x12,0xc3,0x30,
		0x0c,0x03,0x51,0x30,0x82,0x48,0xea,0x63,0x0a,0xf7,0x3f,0xad,0x1b,0x57,0x19,0x67,
		0x26,0x5b,0xbe,0x66,0xa1,0xaf,0xf0,0x0a,0x47,0x25,0x49,0x2b,0x33,0x2f,0x48,0xc5,
		0xcf,0x6e,0xd2,0x18,0xec,0x81,0x09,0x0d,0x63,0x6d,0x48,0x4c,0x6b,0x48,0x61,0xc2,
		0x77,0x4c,0x94,0x98,0xe8,0x25,0xe1,0x22,0xe9,0xcb,0x40,0x0f,0xee,0x9f,0x97,0x37,
		0x38,0x2d,0x1e,0x28,0xd2,0x56,0x5c,0xcd,0x95,0xa9,0x4c,0x61,0x63,0x44,0xa0,0x03,
		0xe6,0x7c,0x20,0x46,0x61,0xce,0x30,0x07,0x8d,0x42,0x99,0x24,0xcc,0xc4,0xda,0x7e,
		0x46,0xff,0x63,0x7b,0x03,0x65,0xa9,0x0b,0xce,0xc9,0xd8,0xcc,0xe9,0x00,0x00,0x00,
		0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82);
}

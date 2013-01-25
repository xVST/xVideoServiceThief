/*  FLVStreamer
 *  Copyright (C) 2009 Andrej Stepanchuk, The Flvstreamer Team
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FLVStreamer; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "log.h"
#include "parseurl.h"

#include "rtmp.h"

char *str2lower(char *str, int len)
{
	char *res = (char *)malloc(len+1);
	char *p;
	
	for(p=res; p<res+len; p++, str++) {
		*p = tolower(*str);
	}
	
	*p = 0;
	
	return res;
}

int chr2hex(char c)
{
	if(c <= 57 && c >= 48)
		return c-48;
	else if(c <= 102 && c >= 97)
		return c-97+10;
	
	return -1;
}

int hex2bin(char *str, char **hex)
{
	if(!str || !hex)
		return 0;
	
	int len = strlen(str);
	
	if(len % 2 != 0)
		return 0;
	
	int ret = len/2;
	
	*hex = (char *)malloc(ret);
	if((*hex)==0)
		return 0;
	
	char *hexptr = *hex;
	char *lwo = str2lower(str, len);
	char *lw = lwo;
	
	len /= 2;
	
	while(len) {
		int d1 = chr2hex(*lw); lw++;
		int d2 = chr2hex(*lw); lw++;
		
		if(d1<0 || d2<0) {
			free(*hex);
			free(lwo);
			*hex=NULL;
			return -1;
		}
		
		*hexptr = (unsigned char)(d1*16+d2);
		hexptr++;
		len--;
	}
	
	free(lwo);
	return ret;
}

bool ParseUrl(char *url, int *protocol, char **host, unsigned int *port, char **playpath, char **app)
{
	assert(url != 0 && protocol != 0 && host != 0 && port != 0 && playpath != 0 && app != 0);
	
	Log(LOGDEBUG, "Parsing...");
	
	*protocol = 0; // default: RTMP
	
	// Old School Parsing
	char *lw = str2lower(url, 6);
	char *temp;
	
	// look for usual :// pattern
	char *p = strstr(url, "://");
	int len = (int)(p-url);
	if(p == 0) {
		Log(LOGWARNING, "RTMP URL: No :// in url!");
		free(lw);
		return false;
	}
	
	if(len == 4 && strncmp(lw, "rtmp", 4)==0)
		*protocol = RTMP_PROTOCOL_RTMP;
	else if(len == 5 && strncmp(lw, "rtmpt", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMPT;
	else if(len == 5 && strncmp(lw, "rtmps", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMPS;
	else if(len == 5 && strncmp(lw, "rtmpe", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMPE;
	else if(len == 5 && strncmp(lw, "rtmfp", 5)==0)
		*protocol = RTMP_PROTOCOL_RTMFP;
	else if(len == 6 && strncmp(lw, "rtmpte", 6)==0)
		*protocol = RTMP_PROTOCOL_RTMPTE;
	else {
		Log(LOGWARNING, "Unknown protocol!\n");
		goto parsehost;
	}
	
	Log(LOGDEBUG, "Parsed protocol: %d", *protocol);
	
parsehost:
	free(lw);
	
	// lets get the hostname
	p+=3;
	
	// check for sudden death
	if(*p==0) {
		Log(LOGWARNING, "No hostname in URL!");		
		return false;
	}
	
	int iEnd   = strlen(p);
	int iCol   = iEnd+1; 
	int iQues  = iEnd+1;
	int iSlash = iEnd+1;
	
	if((temp=strstr(p, ":"))!=0)
		iCol = temp-p;
	if((temp=strstr(p, "?"))!=0)
		iQues = temp-p;
	if((temp=strstr(p, "/"))!=0)
		iSlash = temp-p;
	
	int min = iSlash < iEnd ? iSlash : iEnd+1;
	min = iQues   < min ? iQues   : min;
	
	int hostlen = iCol < min ? iCol : min;
	
	if(min < 256) {
		*host = (char *)malloc((hostlen+1)*sizeof(char));
		strncpy(*host, p, hostlen);
		(*host)[hostlen]=0;
		
		Log(LOGDEBUG, "Parsed host    : %s", *host);
	} else {
		Log(LOGWARNING, "Hostname exceeds 255 characters!");
	}
	
	p+=hostlen; iEnd-=hostlen;
	
	// get the port number if available
	if(*p == ':') {
		p++; iEnd--;
		
		int portlen = min-hostlen-1;
		if(portlen < 6) {
			char portstr[6];
			strncpy(portstr,p,portlen);
			portstr[portlen]=0;
			
			*port = atoi(portstr);
			if(*port == 0)
				*port = 1935;
			
			Log(LOGDEBUG, "Parsed port    : %d", *port);
		} else {
			Log(LOGWARNING, "Port number is longer than 5 characters!");
		}
		
		p+=portlen; iEnd-=portlen;
	}
	
	if(*p != '/') {
		Log(LOGWARNING, "No application or playpath in URL!");
		return true;
	}
	p++; iEnd--;
	
	// parse application
	//
	// rtmp://host[:port]/app[/appinstance][/...]
	// application = app[/appinstance]
	int iSlash2 = iEnd+1; // 2nd slash
	int iSlash3 = iEnd+1; // 3rd slash
	
	if((temp=strstr(p, "/"))!=0)
		iSlash2 = temp-p;
	
	if((temp=strstr(p, "?"))!=0)
		iQues = temp-p;
	
	if(iSlash2 < iEnd)
		if((temp=strstr(p+iSlash2+1, "/"))!=0)
			iSlash3 = temp-p;
	
	//Log(LOGDEBUG, "p:%s, iEnd: %d\niSlash : %d\niSlash2: %d\niSlash3: %d", p, iEnd, iSlash, iSlash2, iSlash3);
	
	int applen = iEnd+1; // ondemand, pass all parameters as app
	int appnamelen = 8; // ondemand length
	
	if(iQues < iEnd && strstr(p, "slist=")) { // whatever it is, the '?' and slist= means we need to use everything as app and parse plapath from slist=
		appnamelen = iQues;
		applen = iEnd+1; // pass the parameters as well
	}
	else if(strncmp(p, "ondemand/", 9)==0) {
		// app = ondemand/foobar, only pass app=ondemand
		applen = 8;
	}
	else { // app!=ondemand, so app is app[/appinstance]
		appnamelen = iSlash2 < iEnd ? iSlash2 : iEnd;
		if(iSlash3 < iEnd)
			appnamelen = iSlash3;
		
		applen = appnamelen;
	}
	
	*app = (char *)malloc((applen+1)*sizeof(char));
	strncpy(*app, p, applen);
	(*app)[applen]=0;
	Log(LOGDEBUG, "Parsed app     : %s", *app);
	
	p += appnamelen; 
	iEnd -= appnamelen;
	
	// parse playpath
	int iPlaypathPos = -1;
	int iPlaypathLen = -1;
	
	bool bAddMP4 = false; // used to add at the end mp4: in front of the playpath
	
	// here filter out semicolon added parameters, e.g. slist=bla...;abc=def
	//if((temp=strstr(p, ";"))!=0)
	//	iEnd = temp-p-1;
	
	if(*p=='?' && (temp=strstr(p, "slist="))!=0) {
		iPlaypathPos = temp-p+6;
		
		int iAnd = iEnd+1;
		if((temp=strstr(p+iPlaypathPos, "&"))!=0)
			iAnd = temp-p;
		if(iAnd < iEnd)
			iPlaypathLen = iAnd-iPlaypathPos;
		else
			iPlaypathLen = iEnd-iPlaypathPos; // +1
	} else { // no slist parameter, so take string after applen 
		if(iEnd > 0) {
			iPlaypathPos = 1;
			iPlaypathLen = iEnd-iPlaypathPos;//+1;
			
			// filter .flv from playpath specified with slashes: rtmp://host/app/path.flv
			if(iPlaypathLen >=4) {
				if(strncmp(&p[iPlaypathPos+iPlaypathLen-4], ".f4v", 4)==0 || strncmp(&p[iPlaypathPos+iPlaypathLen-4], ".mp4", 4)==0) {
					bAddMP4 = true;
				} else if(strncmp(&p[iPlaypathPos+iPlaypathLen-4], ".flv", 4)==0) {
					iPlaypathLen-=4;
				}
			}
		} else {
			Log(LOGERROR, "No playpath found!");
		}
	}
	
	if(iPlaypathLen > -1) {
		*playpath = (char *)malloc((iPlaypathLen+(bAddMP4?4:0)+1)*sizeof(char));
		char *playpathptr = *playpath;
		
		if(bAddMP4 && strncmp(&p[iPlaypathPos], "mp4:", 4)!=0) {
			strcpy(playpathptr, "mp4:");
			playpathptr+=4;
		}
		
		strncpy(playpathptr, &p[iPlaypathPos], iPlaypathLen);
		playpathptr[iPlaypathLen]=0;
		
		Log(LOGDEBUG, "Parsed playpath: %s", *playpath);
	} else {
		Log(LOGWARNING, "No playpath in URL!");
	}
	
	return true;
}


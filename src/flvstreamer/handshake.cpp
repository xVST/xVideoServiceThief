/*
 *  Copyright (C) 2008-2009 Andrej Stepanchuk, The Flvstreamer Team
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

#ifdef WIN32
#include <winsock.h>
#define close(x)        closesocket(x)
#else
#include <sys/times.h>
#endif

#include "rtmp.h"
#include "AMFObject.h"
#include "log.h"
#include "bytes.h"

#define RTMP_SIG_SIZE 1536
#define RTMP_LARGE_HEADER_SIZE 12

#define RTMP_BUFFER_CACHE_SIZE (16*1024) // needs to fit largest number of bytes recv() may return

using namespace RTMP_LIB;
using namespace std;

/* handshake
 *
 * Type		= [1 bytes] 0x06, 0x08 encrypted, 0x03 plain
 * -------------------------------------------------------------------- [1536 bytes]
 * Uptime	= [4 bytes] big endian unsigned number, uptime
 * Version 	= [4 bytes] each byte represents a version number, e.g. 9.0.124.0
 * ... 
 *
 */

bool CRTMP::HandShake()
{
	
	char clientsig[RTMP_SIG_SIZE+1];
	char serversig[RTMP_SIG_SIZE];
	
	clientsig[0] = 0x03;
	
	uint32_t uptime = htonl(GetTime());
	memcpy(clientsig + 1, &uptime, 4);
	memset(&clientsig[5], 0, 4);
	
#ifdef _DEBUG
    for (int i=9; i<=RTMP_SIG_SIZE; i++) 
		clientsig[i] = 0xff;
#else
    for (int i=9; i<=RTMP_SIG_SIZE; i++)
		clientsig[i] = (char)(rand() % 256);
#endif
	
	if (!WriteN(clientsig, RTMP_SIG_SIZE + 1))
		return false;
	
	char type;
	if (ReadN(&type, 1) != 1) // 0x03
		return false;
	
	Log(LOGDEBUG, "%s: Type Answer   : %02X", __FUNCTION__, type);
	
	if(type != clientsig[0])
		Log(LOGWARNING, "%s: Type mismatch: client sent %d, server answered %d", __FUNCTION__, clientsig[0], type);
	
	if (ReadN(serversig, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return false;
	
	// decode server response
	uint32_t suptime;
	
	memcpy(&suptime, serversig, 4);
	suptime = ntohl(suptime);
	
	Log(LOGDEBUG, "%s: Server Uptime : %d", __FUNCTION__, suptime);
	Log(LOGDEBUG, "%s: FMS Version   : %d.%d.%d.%d", __FUNCTION__, serversig[4], serversig[5], serversig[6], serversig[7]);
	
	/*printf("Server signature:\n");
	 for(int i=0; i<RTMP_SIG_SIZE; i++) {
	 printf("%02X ", serversig[i]);
	 }
	 printf("\n");*/
	
	// 2nd part of handshake
	char resp[RTMP_SIG_SIZE];
	if (ReadN(resp, RTMP_SIG_SIZE) != RTMP_SIG_SIZE)
		return false;
	
	bool bMatch = (memcmp(resp, clientsig + 1, RTMP_SIG_SIZE) == 0);
	if (!bMatch)
	{
		Log(LOGWARNING, "%s, client signiture does not match!",__FUNCTION__);
	}
	
	// the second time field is when 1st part was read by the client:
	uint32_t timeRead = htonl(GetTime());
	memcpy(serversig+4, &timeRead, 4);
	
	if (!WriteN(serversig, RTMP_SIG_SIZE))
		return false;
	
	return true;
}

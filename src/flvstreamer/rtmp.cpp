/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *      Copyright (C) 2008-2009 Andrej Stepanchuk, The Flvstreamer Team
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#ifdef WIN32
#include <winsock.h>
#define close(x)	closesocket(x)
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

static const int packetSize[] = { 12, 8, 4, 1 };
#define RTMP_PACKET_SIZE_LARGE    0
#define RTMP_PACKET_SIZE_MEDIUM   1
#define RTMP_PACKET_SIZE_SMALL    2
#define RTMP_PACKET_SIZE_MINIMUM  3

int32_t GetTime()
{
#ifdef _DEBUG
	return 0;
#elif defined(WIN32)
	return timeGetTime();
#else
	struct tms t;
	return times(&t);
#endif
}

char RTMPProtocolStrings[][7] =
{
"RTMP",
"RTMPT",
"RTMPS",
"RTMPE",
"RTMPTE",
"RTMFP"
};

char RTMPProtocolStringsLower[][7] =
{
"rtmp",
"rtmpt",
"rtmps",
"rtmpe",
"rtmpte",
"rtmpfp"
};

CRTMP::CRTMP() : m_socket(0)
{
	Close();
	m_pBuffer = new char[RTMP_BUFFER_CACHE_SIZE];
	m_nBufferMS = 300;
	m_fDuration = 0;
}

CRTMP::~CRTMP()
{
	Close();
	delete [] m_pBuffer;
}

double CRTMP::GetDuration() { return m_fDuration; }
bool CRTMP::IsConnected() { return m_socket != 0; }

void CRTMP::SetBufferMS(int size)
{
	m_nBufferMS = size;
}

void CRTMP::UpdateBufferMS()
{
	SendPing(3, 1, m_nBufferMS);
}

bool CRTMP::Connect(
					int protocol, 
					char *hostname, 
					unsigned int port, 
					char *playpath, 
					char *tcUrl, 
					char *swfUrl, 
					char *pageUrl, 
					char *app, 
					char *auth,
					char *flashVer,
					char *subscribepath,
					double dTime,
					bool bLiveStream,
					long int timeout
)
{
	assert(protocol < 6);
	
	Log(LOGDEBUG, "Protocol      : %s", RTMPProtocolStrings[protocol]);
	Log(LOGDEBUG, "Hostname      : %s", hostname);
	Log(LOGDEBUG, "Port          : %d", port);
	Log(LOGDEBUG, "Playpath      : %s", playpath);
	
	if(tcUrl)
		Log(LOGDEBUG, "tcUrl         : %s", tcUrl);
	if(swfUrl)
		Log(LOGDEBUG, "swfUrl        : %s", swfUrl);
	if(pageUrl)
		Log(LOGDEBUG, "pageUrl       : %s", pageUrl);
	if(app)
		Log(LOGDEBUG, "app           : %s", app);
	if(auth)
		Log(LOGDEBUG, "auth          : %s", auth);
	if(subscribepath)
		Log(LOGDEBUG, "subscribepath : %s", subscribepath);
	if(flashVer)
		Log(LOGDEBUG, "flashVer      : %s", flashVer);
	if(dTime > 0)
		Log(LOGDEBUG, "SeekTime      : %lf", dTime);
	
	Log(LOGDEBUG,       "live          : %s", bLiveStream ? "yes":"no");
	Log(LOGDEBUG,       "timeout       : %d sec", timeout);
	
	Link.tcUrl = tcUrl;
	Link.swfUrl = swfUrl;
	Link.pageUrl = pageUrl;
	Link.app = app;
	Link.auth = auth;
	Link.flashVer = flashVer;
	Link.subscribepath = subscribepath;
	Link.seekTime = dTime;
	Link.bLiveStream = bLiveStream;
	Link.timeout = timeout;
	
	Link.protocol = protocol;
	Link.hostname = hostname;
	Link.port = port;
	Link.playpath = playpath;
	
	if (Link.port == 0)
		Link.port = 1935;
	
	// close any previous connection
	Close();
	
	sockaddr_in service;
	memset(&service, 0, sizeof(sockaddr_in));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(Link.hostname);
	if (service.sin_addr.s_addr == INADDR_NONE)
	{
		struct hostent *host = gethostbyname(Link.hostname);
		if (host == NULL || host->h_addr == NULL)
		{
			Log(LOGERROR, "Problem accessing the DNS. (addr: %s)", Link.hostname);
			return false;
		}
		service.sin_addr = *(struct in_addr*)host->h_addr;
	}
	
	service.sin_port = htons(Link.port);
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket != -1)
	{
		if (connect(m_socket, (sockaddr*) &service, sizeof(struct sockaddr)) < 0)
		{
			Log(LOGERROR, "%s, failed to connect socket. Error: %d", __FUNCTION__, GetSockError());
			Close();
			return false;
		}
		
		Log(LOGDEBUG, "%s, ... connected, handshaking", __FUNCTION__);
		if (!HandShake())
		{
			Log(LOGERROR, "%s, handshake failed.", __FUNCTION__);
			Close();
			return false;
		}
		
		Log(LOGDEBUG, "%s, handshaked", __FUNCTION__);
		
		if (!Connect())
		{
			Log(LOGERROR, "%s, RTMP connect failed.", __FUNCTION__);
			Close();
			return false;
		}
		// set timeout
		struct timeval tv;
		memset(&tv, 0, sizeof(tv));
		tv.tv_sec = Link.timeout;
		if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof(tv))) {
			Log(LOGERROR,"%s, Setting socket timeout to %ds failed!", __FUNCTION__, tv.tv_sec);
		}
	}
	else
	{
		Log(LOGERROR, "%s, failed to create socket. Error: %d", __FUNCTION__, GetSockError());
		return false;
	}
	
	return true;
}

int CRTMP::GetNextMediaPacket(RTMPPacket &packet)
{
	int bHasMediaPacket = 0;
	while (!bHasMediaPacket && IsConnected() && ReadPacket(packet))
	{
		if (!packet.IsReady())
		{
			packet.FreePacket();
			//usleep(5000); // 5ms
			continue;
		}
		
		switch (packet.m_packetType)
		{
			case 0x01:
				// chunk size
				HandleChangeChunkSize(packet);
				break;
				
			case 0x03:
				// bytes read report
				Log(LOGDEBUG, "%s, received: bytes read report", __FUNCTION__);
				break;
				
			case 0x04:
				// ping
				HandlePing(packet);
				break;
				
			case 0x05:
				// server bw
				Log(LOGDEBUG, "%s, received: server BW", __FUNCTION__);
				//LogHex(packet.m_body, packet.m_nBodySize);
				break;
				
			case 0x06:
				// client bw
				Log(LOGDEBUG, "%s, received: client BW", __FUNCTION__);
				//LogHex(packet.m_body, packet.m_nBodySize);
				break;
				
			case 0x08:
				// audio data
				//Log(LOGDEBUG, "%s, received: audio %lu bytes", __FUNCTION__, packet.m_nBodySize);
				HandleAudio(packet);
				bHasMediaPacket = 1;
				break;
				
			case 0x09:
				// video data
				//Log(LOGDEBUG, "%s, received: video %lu bytes", __FUNCTION__, packet.m_nBodySize);
				HandleVideo(packet);
				bHasMediaPacket = 1;
				break;
				
			case 0x0F: // flex stream send
				Log(LOGDEBUG, "%s, flex stream send, size %lu bytes, not supported, ignoring", __FUNCTION__, packet.m_nBodySize);
				break;
				
			case 0x10: // flex shared object
				Log(LOGDEBUG, "%s, flex shared object, size %lu bytes, not supported, ignoring", __FUNCTION__, packet.m_nBodySize);
				break;
				
			case 0x11: // flex message
			{
				Log(LOGDEBUG, "%s, flex message, size %lu bytes, not fully supported", __FUNCTION__, packet.m_nBodySize);
				//LogHex(packet.m_body, packet.m_nBodySize);
				
				// some DEBUG code
				/*RTMP_LIB::AMFObject obj;
				 int nRes = obj.Decode(packet.m_body+1, packet.m_nBodySize-1);
				 if(nRes < 0) {
				 Log(LOGERROR, "%s, error decoding AMF3 packet", __FUNCTION__);
				 //return;
				 }
				 
				 obj.Dump();*/
				
				// If we recv Play.Stop then return 2
				if ( HandleInvoke(packet.m_body+1, packet.m_nBodySize-1) == 1 )
					bHasMediaPacket = 2; 
				break;
			}
			case 0x12:
				// metadata (notify)
				Log(LOGDEBUG, "%s, received: notify %lu bytes", __FUNCTION__, packet.m_nBodySize);
				HandleMetadata(packet.m_body, packet.m_nBodySize);
				bHasMediaPacket = 1;
				break;
				
			case 0x13:
				Log(LOGDEBUG, "%s, shared object, not supported, ignoring", __FUNCTION__);
				break;
				
			case 0x14:
				// invoke
				Log(LOGDEBUG, "%s, received: invoke %lu bytes", __FUNCTION__, packet.m_nBodySize);
				//LogHex(packet.m_body, packet.m_nBodySize);
				
				if ( HandleInvoke(packet.m_body, packet.m_nBodySize) == 1 )
					bHasMediaPacket = 2; 
				break;
				
			case 0x16:
			{
				// go through FLV packets and handle metadata packets
				unsigned int pos=0;
				
				while(pos+11 < packet.m_nBodySize) {
					uint32_t dataSize = CRTMP::ReadInt24(packet.m_body+pos+1); // size without header (11) and prevTagSize (4)
					
					if(pos+11+dataSize+4 > packet.m_nBodySize) {
                        Log(LOGWARNING, "Stream corrupt?!");
						break;
					}
					if(packet.m_body[pos] == 0x12) {
						HandleMetadata(packet.m_body+pos+11, dataSize);
					}
					pos += (11+dataSize+4);
				}
				
				// FLV tag(s)
				//Log(LOGDEBUG, "%s, received: FLV tag(s) %lu bytes", __FUNCTION__, packet.m_nBodySize);
				bHasMediaPacket = 1;
				break;
			}
			default:
				Log(LOGDEBUG, "%s, unknown packet type received: 0x%02x", __FUNCTION__, packet.m_packetType);
#ifdef _DEBUG
				LogHex(packet.m_body, packet.m_nBodySize);
#endif
		}
		
		if (!bHasMediaPacket) { 
			packet.FreePacket();
		}
	}
	
	if (bHasMediaPacket)
		m_bPlaying = true;
	
	return bHasMediaPacket;
}

#ifdef _DEBUG
extern FILE *netstackdump;
extern FILE *netstackdump_read;
#endif

int CRTMP::ReadN(char *buffer, int n)
{
	int nOriginalSize = n;
	
#ifdef _DEBUG
	memset(buffer, 0, n);
#endif
	
	char *ptr = buffer;
	while (n > 0)
	{
		int nBytes = 0;
		
		if(m_nBufferSize == 0)
			if (!FillBuffer()) {
    	        Close();
                return 0;
			}
		int nRead = ((n<m_nBufferSize)?n:m_nBufferSize);
		if(nRead > 0) {
			memcpy(ptr, m_pBufferStart, nRead);
			m_pBufferStart += nRead;
			m_nBufferSize -= nRead;
			nBytes = nRead;
			m_nBytesIn += nRead;
			if(m_nBytesIn > m_nBytesInSent + (600*1024)) // report every 600K
				SendBytesReceived();
		}
		
		//again:
		//    nBytes = recv(m_socket, ptr, n, 0);
		
		//Log(LOGDEBUG, "%s: %d bytes\n", __FUNCTION__, nBytes);
#ifdef _DEBUG
        fwrite(ptr, 1, nBytes, netstackdump_read);
#endif
		
		if (nBytes == 0)
		{
			Log(LOGDEBUG, "%s, RTMP socket closed by server", __FUNCTION__);
			//goto again;
			Close();
			break;
		}
		
		n -= nBytes;
		ptr += nBytes;
	}
	
	return nOriginalSize - n;
}

bool CRTMP::WriteN(const char *buffer, int n)
{
	const char *ptr = buffer;
	
	while (n > 0)
	{
#ifdef _DEBUG
		fwrite(ptr, 1, n, netstackdump);
#endif
		
		int nBytes = send(m_socket, ptr, n, 0);
		//Log(LOGDEBUG, "%s: %d\n", __FUNCTION__, nBytes);
		
		if (nBytes < 0)
		{
			Log(LOGERROR, "%s, RTMP send error %d (%d bytes)", __FUNCTION__, GetSockError(), n);
			Close();
			
			return false;
		}
		
		if (nBytes == 0)
			break;
		
		n -= nBytes;
		ptr += nBytes;
	}
	
	return n == 0;
}

bool CRTMP::Connect()
{
	if (!SendConnectPacket())
	{
		Log(LOGERROR, "%s, failed to send connect RTMP packet", __FUNCTION__);
		return false;
	}
	
	return true;
}

bool CRTMP::SendConnectPacket()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = 0x14; // INVOKE
	packet.AllocPacket(4096);
	
	char *enc = packet.m_body;
	enc += EncodeString(enc, "connect");
	enc += EncodeNumber(enc, 1.0);
	*enc = 0x03; //Object Datatype
	enc++;
	
	if(Link.app)
		enc += EncodeString(enc, "app", Link.app);
	if(Link.flashVer)
		enc += EncodeString(enc, "flashVer", Link.flashVer);
	if(Link.swfUrl)
		enc += EncodeString(enc, "swfUrl", Link.swfUrl);
	if(Link.tcUrl)
		enc += EncodeString(enc, "tcUrl", Link.tcUrl);
	
	enc += EncodeBoolean(enc, "fpad", false);
	enc += EncodeNumber(enc, "capabilities", 15.0);
	enc += EncodeNumber(enc, "audioCodecs", 3191.0);
	enc += EncodeNumber(enc, "videoCodecs", 252.0);
	enc += EncodeNumber(enc, "videoFunction", 1.0);
	if(Link.pageUrl)
		enc += EncodeString(enc, "pageUrl", Link.pageUrl);
	
	//enc += EncodeNumber(enc, "objectEncoding", 3.0); // use AMF3 objects, not supported yet
	enc += 2; // end of object - 0x00 0x00 0x09
	*enc = 0x09;
	enc++;
	
	//enc += EncodeString(enc, "user"); // DEBUG, REMOVE!!!
	//*enc = 0x05; enc++;
	//enc += EncodeString(enc, "tvmanele1"); // DEBUG, REMOVE!!
	
	// add auth string
	if(Link.auth)
	{
		*enc = 0x01; enc++;
		*enc = 0x01; enc++;
		
		enc += EncodeString(enc, Link.auth);
	}
	packet.m_nBodySize = enc-packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendBGHasStream(double dId, char *playpath)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // INVOKE
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "bgHasStream");
	enc += EncodeNumber(enc, dId);
	*enc = 0x05; // NULL
	enc++;
	
	enc += EncodeString(enc, playpath);
	
	packet.m_nBodySize = enc-packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendCreateStream(double dStreamId)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // INVOKE
	
	packet.AllocPacket(1024); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "createStream");
	enc += EncodeNumber(enc, dStreamId);
	*enc = 0x05; // NULL
	enc++;
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendFCSubscribe(char *subscribepath)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // INVOKE
	
	packet.AllocPacket(256); // should be enough
	Log(LOGDEBUG, "FCSubscribe: %s", subscribepath);
	char *enc = packet.m_body;
	enc += EncodeString(enc, "FCSubscribe");
	enc += EncodeNumber(enc, 0);
	*enc = 0x05; // NULL
	enc++;
	enc += EncodeString(enc, subscribepath);
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendDeleteStream(double dStreamId)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // INVOKE
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "deleteStream");
	enc += EncodeNumber(enc, 0.0);
	*enc = 0x05; // NULL
	enc++;
	enc += EncodeNumber(enc, dStreamId);
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendPause(bool DoPause, double dTime)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x08;   // video channel 
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // invoke
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "pause");
	enc += EncodeNumber(enc, 0);
	*enc = 0x05; // NULL
	enc++;
	enc += EncodeBoolean(enc, DoPause);
	enc += EncodeNumber(enc, (double)dTime/1000);
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendSeek(double dTime)
{
	RTMPPacket packet;
	packet.m_nChannel = 0x08;   // video channel 
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // invoke
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "seek");
	enc += EncodeNumber(enc, 0);
	*enc = 0x05; // NULL
	enc++;
	enc += EncodeNumber(enc, dTime);
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendServerBW()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x02;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = 0x05; // Server BW
	
	packet.AllocPacket(4);
	packet.m_nBodySize = 4;
	
	EncodeInt32(packet.m_body, 0x001312d0); // hard coded for now
	return SendRTMP(packet);
}

bool CRTMP::SendBytesReceived()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x02;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x03; // bytes in
	
	packet.AllocPacket(4);
	packet.m_nBodySize = 4;
	
	EncodeInt32(packet.m_body, m_nBytesIn); // hard coded for now
	m_nBytesInSent = m_nBytesIn;
	
	//Log(LOGDEBUG, "Send bytes report. 0x%x (%d bytes)", (unsigned int)m_nBytesIn, m_nBytesIn);
	return SendRTMP(packet);
}

bool CRTMP::SendCheckBW()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = 0x14; // INVOKE
	packet.m_nInfoField1 = GetTime();
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "_checkbw");
	enc += EncodeNumber(enc, 0x00);
	*enc = 0x05; // NULL
	enc++;
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendCheckBWResult()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x03;   // control channel (invoke)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x14; // INVOKE
	packet.m_nInfoField1 = 0x16 * m_nBWCheckCounter; // temp inc value. till we figure it out.
	
	packet.AllocPacket(256); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "_result");
	enc += EncodeNumber(enc, (double)GetTime()); // temp
	*enc = 0x05; // NULL
	enc++;
	enc += EncodeNumber(enc, (double)m_nBWCheckCounter++); 
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}

bool CRTMP::SendPlay()
{
	RTMPPacket packet;
	packet.m_nChannel = 0x08;   // we make 8 our stream channel
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
	packet.m_packetType = 0x14; // INVOKE
	packet.m_nInfoField2 = m_stream_id; //0x01000000;
	
	packet.AllocPacket(1024); // should be enough
	char *enc = packet.m_body;
	enc += EncodeString(enc, "play");
	enc += EncodeNumber(enc, 0.0); // stream id??
	*enc = 0x05; // NULL
	enc++;
	
	Log(LOGDEBUG, "%s, sending play: %s", __FUNCTION__, Link.playpath);
	enc += EncodeString(enc, Link.playpath);
	
	// start: -2, -1, 0, positive number
	//  -2: looks for a live stream, then a recorded stream, if not found any open a live stream
	//  -1: plays a live stream
	// >=0: plays a recorded streams from 'start' milliseconds
	if(Link.bLiveStream)
		enc += EncodeNumber(enc, -1000.0);
	else {
		if(Link.seekTime > 0.0)
			enc += EncodeNumber(enc, Link.seekTime); // resume from here
		else
			enc += EncodeNumber(enc, 0.0);//-2000.0); // recorded as default, -2000.0 is not reliable since that freezes the player if the stream is not found
	}
	
	// len: -1, 0, positive number
	//  -1: plays live or recorded stream to the end (default)
	//   0: plays a frame 'start' ms away from the beginning
	//  >0: plays a live or recoded stream for 'len' milliseconds
	//enc += EncodeNumber(enc, -1.0); // len
	
	packet.m_nBodySize = enc - packet.m_body;
	
	return SendRTMP(packet);
}
/*
 from http://jira.red5.org/confluence/display/docs/Ping:
 
 Ping is the most mysterious message in RTMP and till now we haven't fully interpreted it yet. In summary, Ping message is used as a special command that are exchanged between client and server. This page aims to document all known Ping messages. Expect the list to grow.
 
 The type of Ping packet is 0x4 and contains two mandatory parameters and two optional parameters. The first parameter is the type of Ping and in short integer. The second parameter is the target of the ping. As Ping is always sent in Channel 2 (control channel) and the target object in RTMP header is always 0 which means the Connection object, it's necessary to put an extra parameter to indicate the exact target object the Ping is sent to. The second parameter takes this responsibility. The value has the same meaning as the target object field in RTMP header. (The second value could also be used as other purposes, like RTT Ping/Pong. It is used as the timestamp.) The third and fourth parameters are optional and could be looked upon as the parameter of the Ping packet. Below is an unexhausted list of Ping messages.
 
 * type 0: Clear the stream. No third and fourth parameters. The second parameter could be 0. After the connection is established, a Ping 0,0 will be sent from server to client. The message will also be sent to client on the start of Play and in response of a Seek or Pause/Resume request. This Ping tells client to re-calibrate the clock with the timestamp of the next packet server sends.
 * type 1: Tell the stream to clear the playing buffer.
 * type 3: Buffer time of the client. The third parameter is the buffer time in millisecond.
 * type 4: Reset a stream. Used together with type 0 in the case of VOD. Often sent before type 0.
 * type 6: Ping the client from server. The second parameter is the current time.
 * type 7: Pong reply from client. The second parameter is the time the server sent with his ping request.
 * type 26: SWFVerification request
 * type 27: SWFVerification response
 */
bool CRTMP::SendPing(short nType, unsigned int nObject, unsigned int nTime)
{
	Log(LOGDEBUG, "sending ping. type: 0x%04x", (unsigned short)nType);
	
	RTMPPacket packet; 
	packet.m_nChannel = 0x02;   // control channel (ping)
	packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet.m_packetType = 0x04; // ping
	packet.m_nInfoField1 = GetTime();
	
	int nSize = (nType==0x03?10:6); // type 3 is the buffer time and requires all 3 parameters. all in all 10 bytes.
	if(nType == 0x1B) 
		nSize = 44;
	
	packet.AllocPacket(nSize);
	packet.m_nBodySize = nSize;
	
	char *buf = packet.m_body;
	buf += EncodeInt16(buf, nType);
	
	if(nType == 0x1B) {
	} else {
		if (nSize > 2)
			buf += EncodeInt32(buf, nObject);
		
		if (nSize > 6)
			buf += EncodeInt32(buf, nTime);
	}
	
	return SendRTMP(packet);
}

int CRTMP::HandleInvoke(const char *body, unsigned int nBodySize)
{
	if (body[0] != 0x02) // make sure it is a string method name we start with
	{
		Log(LOGWARNING, "%s, Sanity failed. no string method in invoke packet", __FUNCTION__);
		return 0;
	}
	
	RTMP_LIB::AMFObject obj;
	int nRes = obj.Decode(body, nBodySize);
	if (nRes < 0)
	{ 
		Log(LOGERROR, "%s, error decoding invoke packet", __FUNCTION__);
		return 0;
	}
	
	obj.Dump();
	std::string method = obj.GetProperty(0).GetString();
	Log(LOGDEBUG, "%s, server invoking <%s>", __FUNCTION__, method.c_str());
	
	if (method == "_result")
	{
		std::string methodInvoked = m_methodCalls[0];
		m_methodCalls.erase(m_methodCalls.begin());
		
		Log(LOGDEBUG, "%s, received result for method call <%s>", __FUNCTION__, methodInvoked.c_str());
		
		if (methodInvoked == "connect")
		{
			SendServerBW();
			SendPing(3, 0, 300);
			
			SendCreateStream(2.0);
			
			// Send the FCSubscribe if live stream or if subscribepath is set
			if (Link.subscribepath)
				SendFCSubscribe(Link.subscribepath);
			else if (Link.bLiveStream)
				SendFCSubscribe(Link.playpath);
		}
		else if (methodInvoked == "createStream")
		{
			m_stream_id = (int)obj.GetProperty(3).GetNumber();
			
			SendPlay();
			/* not required since we send the seek parameter in the play packet now
			 if(Link.seekTime > 0) {
			 Log(LOGDEBUG, "%s, sending seek: %f ms", __FUNCTION__, Link.seekTime);
			 SendSeek(Link.seekTime);
			 }*/
			
			SendPing(3, 1, m_nBufferMS);
		}
		else if (methodInvoked == "play")
		{
			SendPlay();
		}
	}
	else if (method == "onBWDone")
	{
		SendCheckBW();
	}
	else if (method == "onFCSubscribe")
	{
		// SendOnFCSubscribe();
	}
	else if (method == "_onbwcheck")
	{
		SendCheckBWResult();
	}
	else if (method == "_error")
	{
		Log(LOGERROR, "rtmp server sent error");
	}
	else if (method == "close")
	{
		Log(LOGERROR, "rtmp server requested close");
		Close();
	}
	else if (method == "onStatus")
	{
		std::string code  = obj.GetProperty(3).GetObject().GetProperty("code").GetString();
		std::string level = obj.GetProperty(3).GetObject().GetProperty("level").GetString();
		
		Log(LOGDEBUG, "%s, onStatus: %s", __FUNCTION__, code.c_str() );
		if (code == "NetStream.Failed"
			||  code == "NetStream.Play.Failed"
			||  code == "NetStream.Play.StreamNotFound"
			||  code == "NetConnection.Connect.InvalidApp")
			Close();
		
		// Return 1 if this is a Play.Complete or Play.Stop
		if (code == "NetStream.Play.Complete"
			||  code == "NetStream.Play.Stop") {
			Close();
			return 1;
		}
		
		/*if(Link.seekTime > 0) {
		 if(code == "NetStream.Seek.Notify") { // seeked successfully, can play now!
		 bSeekedSuccessfully = true;
		 } else if(code == "NetStream.Play.Start" && !bSeekedSuccessfully) { // well, try to seek again
		 Log(LOGWARNING, "%s, server ignored seek!", __FUNCTION__);
		 }
		 }*/
	}
	else
	{
		
	}
	return 0;
}

//int pnum=0;

bool CRTMP::FindFirstMatchingProperty(AMFObject &obj, std::string name, AMFObjectProperty &p)
{
	// this is a small object search to locate the "duration" property
	for (int n=0; n<obj.GetPropertyCount(); n++) {
		AMFObjectProperty prop = obj.GetProperty(n);
		
		if(prop.GetPropName() == name) {
			
			p = obj.GetProperty(n);
			return true;
		}
		
		if(prop.GetType() == AMF_OBJECT) {
			AMFObject next = prop.GetObject();
			return FindFirstMatchingProperty(next, name, p);
		}
	}
	return false;
}

bool CRTMP::DumpMetaData(AMFObject &obj)
{
	std::string name;
	AMFObjectProperty prop;
	for (int n=0; n<obj.GetPropertyCount(); n++) {
		AMFObjectProperty prop = obj.GetProperty(n);
		if ( prop.GetType() != AMF_OBJECT ) {
			char str[256]="";
			switch( prop.GetType() )
			{
				case AMF_NUMBER:
					if ( (double)int(prop.GetNumber()) == prop.GetNumber() )
						snprintf(str, 255, "%.0f", prop.GetNumber() );
					else
						snprintf(str, 255, "%.2f", prop.GetNumber() );
					break;
				case AMF_BOOLEAN:
					snprintf(str, 255, "%s", prop.GetNumber() == 1.?"TRUE":"FALSE");
					break;
				case AMF_STRING:
					snprintf(str, 255, "%s", prop.GetString().c_str());
					break;
				case AMF_DATE:
					snprintf(str, 255, "timestamp:%.2f", prop.GetNumber() );
					break;
				default:
					snprintf(str, 255, "INVALID TYPE 0x%02x", (unsigned char)prop.GetType() );
			}
			if ( prop.GetPropName() != "" ) {
				// chomp
				if ( strlen(str) >= 1 && str[strlen(str)-1 ] == '\n')
					str[strlen(str)-1] = '\0';
				LogPrintf("  %-22s%s\n", prop.GetPropName().c_str(), str );
			}
		} else {
			if ( prop.GetPropName() != "" )
				LogPrintf("%s:\n", prop.GetPropName().c_str() );
			AMFObject next = prop.GetObject();
			DumpMetaData(next);
		}
	}
	return false;
}

void CRTMP::HandleMetadata(char *body, unsigned int len)
{
	/*Log(LOGDEBUG,"Parsing meta data: %d @0x%08X", packet.m_nBodySize, packet.m_body);
	 LogHex(packet.m_body, packet.m_nBodySize);
	 
	 char str[256]={0};
	 sprintf(str, "packet%d", pnum);
	 pnum++;
	 FILE *f = fopen(str, "wb");
	 fwrite(packet.m_body, 1, packet.m_nBodySize, f);
	 fclose(f);//*/
	
	// allright we get some info here, so parse it and print it
	// also keep duration or filesize to make a nice progress bar
	
	//int len = packet.m_nBodySize;
	//char *p = packet.m_body;
	
	RTMP_LIB::AMFObject obj;
	int nRes = obj.Decode(body, len);
	if(nRes < 0) {
		Log(LOGERROR, "%s, error decoding meta data packet", __FUNCTION__);
		return;
	}
	
	obj.Dump();
  	std::string metastring = obj.GetProperty(0).GetString();
	
	if(metastring == "onMetaData") {
		// Show metadata
		LogPrintf("\r%s\n", "Metadata:                  " );
		DumpMetaData(obj);
		AMFObjectProperty prop;
		// Extract duration of stream
		if(FindFirstMatchingProperty(obj, "duration", prop)) {
			m_fDuration = prop.GetNumber();
			//Log(LOGDEBUG, "Set duration: %.2f seconds", m_fDuration);
		}
	}
}

void CRTMP::HandleChangeChunkSize(const RTMPPacket &packet)
{
	if (packet.m_nBodySize >= 4)
	{
		m_chunkSize = ReadInt32(packet.m_body);
		Log(LOGDEBUG, "%s, received: chunk size change to %d", __FUNCTION__, m_chunkSize);
	}
}

void CRTMP::HandleAudio(const RTMPPacket &packet)
{
}

void CRTMP::HandleVideo(const RTMPPacket &packet)
{
}

void CRTMP::HandlePing(const RTMPPacket &packet)
{
	short nType = -1;
	if (packet.m_body && packet.m_nBodySize >= 2)
		nType = ReadInt16(packet.m_body);
	Log(LOGDEBUG, "%s, received ping. type: %d, len: %d", __FUNCTION__, nType, packet.m_nBodySize);
	//LogHex(packet.m_body, packet.m_nBodySize);
	
	if (nType == 0x06 && packet.m_nBodySize >= 6) // server ping. reply with pong.
	{
		unsigned int nTime = ReadInt32(packet.m_body + 2);
		SendPing(0x07, nTime);
	}
	
	if (nType == 0x1A) {
		Log(LOGDEBUG, "%s, SWFVerification ping received: ", __FUNCTION__);
		//LogHex(packet.m_body, packet.m_nBodySize);
        Log(LOGWARNING, "%s: Ignoring unsupported SWFVerification request", __FUNCTION__);
	}
}

bool CRTMP::ReadPacket(RTMPPacket &packet)
{
	char type;
	if (ReadN(&type,1) == 0)
	{
		Log(LOGERROR, "%s, failed to read RTMP packet header", __FUNCTION__);
		return false;
	} 
	
	packet.m_headerType = (type & 0xc0) >> 6;
	packet.m_nChannel = (type & 0x3f);
	
	int nSize = packetSize[packet.m_headerType];
	
	if (nSize == RTMP_LARGE_HEADER_SIZE) // if we get a full header the timestamp is absolute
		packet.m_hasAbsTimestamp = true; 
	
	if (nSize < RTMP_LARGE_HEADER_SIZE) { // using values from the last message of this channel
		packet.FreePacketHeader(); // test whether this avoids memory leak
		packet = m_vecChannelsIn[packet.m_nChannel];
	}
	
	nSize--;
	
	char header[RTMP_LARGE_HEADER_SIZE] = {0};
	if (nSize > 0 && ReadN(header,nSize) != nSize)
	{
		Log(LOGERROR, "%s, failed to read RTMP packet header. type: %x", __FUNCTION__, (unsigned int)type);
		return false;
	}
	
	if (nSize >= 3)
		packet.m_nInfoField1 = ReadInt24(header);
	
	//Log(LOGDEBUG, "%s, reading RTMP packet chunk on channel %x, headersz %i, timestamp %i, abs timestamp %i", __FUNCTION__, packet.m_nChannel, nSize, packet.m_nInfoField1, packet.m_hasAbsTimestamp); 
	
	if (nSize >= 6)
	{
		packet.m_nBodySize = ReadInt24(header + 3);
		packet.m_nBytesRead = 0;
		packet.FreePacketHeader(); // new packet body
	}
	
	if (nSize > 6)
		packet.m_packetType = header[6];
	
	if (nSize == 11)
		packet.m_nInfoField2 = ReadInt32LE(header+7);
	
	if (packet.m_nBodySize > 0 && packet.m_body == NULL && !packet.AllocPacket(packet.m_nBodySize))
	{
		Log(LOGDEBUG, "%s, failed to allocate packet", __FUNCTION__);
		return false;
	}
	
	int nToRead = packet.m_nBodySize - packet.m_nBytesRead;
	int nChunk = m_chunkSize;
	if (nToRead < nChunk)
		nChunk = nToRead;
	
	if (ReadN(packet.m_body + packet.m_nBytesRead, nChunk) != nChunk)
	{
		Log(LOGERROR, "%s, failed to read RTMP packet body. len: %lu", __FUNCTION__, packet.m_nBodySize);
		packet.m_body = NULL; // we dont want it deleted since its pointed to from the stored packets (m_vecChannelsIn)
		return false;  
	}
	
	packet.m_nBytesRead += nChunk;
	
	// keep the packet as ref for other packets on this channel
	m_vecChannelsIn[packet.m_nChannel] = packet;
	
	if (packet.IsReady())
	{
		packet.m_nTimeStamp = packet.m_nInfoField1;
		
		// make packet's timestamp absolute 
		if (!packet.m_hasAbsTimestamp) 
			packet.m_nTimeStamp += m_channelTimestamp[packet.m_nChannel]; // timestamps seem to be always relative!! 
		
		m_channelTimestamp[packet.m_nChannel] = packet.m_nTimeStamp; 
		
		// reset the data from the stored packet. we keep the header since we may use it later if a new packet for this channel
		// arrives and requests to re-use some info (small packet header)
		m_vecChannelsIn[packet.m_nChannel].m_body = NULL;
		m_vecChannelsIn[packet.m_nChannel].m_nBytesRead = 0;
		m_vecChannelsIn[packet.m_nChannel].m_hasAbsTimestamp = false; // can only be false if we reuse header
	}
	else
		packet.m_body = NULL; // so it wont be erased on "free"
	
	return true;
}

short  CRTMP::ReadInt16(const char *data)
{
	short val;
	memcpy(&val,data,sizeof(short));
	return ntohs(val);
}

int  CRTMP::ReadInt24(const char *data)
{
	char tmp[4] = {0};
	memcpy(tmp+1, data, 3);
	int val;
	memcpy(&val, tmp, sizeof(int));
	return ntohl(val);
}

// big-endian 32bit integer
int  CRTMP::ReadInt32(const char *data)
{
	int val;
	memcpy(&val, data, sizeof(int));
	return ntohl(val);
}

std::string CRTMP::ReadString(const char *data)
{
	std::string strRes;
	short len = ReadInt16(data);
	if (len > 0)
	{
		char *pStr = new char[len+1]; 
		memset(pStr, 0, len+1);
		memcpy(pStr, data + sizeof(short), len);
		strRes = pStr;
		delete [] pStr;
	}
	return strRes;
}

bool CRTMP::ReadBool(const char *data)
{
	return *data == 0x01;
}

int CRTMP::EncodeString(char *output, const std::string &strName, const std::string &strValue)
{
	char *buf = output;
	short length = htons(strName.size());
	memcpy(buf, &length, 2);
	buf += 2;
	
	memcpy(buf, strName.c_str(), strName.size());
	buf += strName.size();
	
	buf += EncodeString(buf, strValue);
	return buf - output;
}

int CRTMP::EncodeInt16(char *output, short nVal)
{
	nVal = htons(nVal);
	memcpy(output, &nVal, sizeof(short));
	return sizeof(short);
}

int CRTMP::EncodeInt24(char *output, int nVal)
{
	nVal = htonl(nVal);
	char *ptr = (char *)&nVal;
	ptr++;
	memcpy(output, ptr, 3);
	return 3;
}

// big-endian 32bit integer
int CRTMP::EncodeInt32(char *output, int nVal)
{
	nVal = htonl(nVal);
	memcpy(output, &nVal, sizeof(int));
	return sizeof(int);
}

int CRTMP::EncodeNumber(char *output, const std::string &strName, double dVal)
{
	char *buf = output;
	
	unsigned short length = htons(strName.size());
	memcpy(buf, &length, 2);
	buf += 2;
	
	memcpy(buf, strName.c_str(), strName.size());
	buf += strName.size();
	
	buf += EncodeNumber(buf, dVal);
	return buf - output;
}

int CRTMP::EncodeBoolean(char *output, const std::string &strName, bool bVal)
{
	char *buf = output;
	unsigned short length = htons(strName.size());
	memcpy(buf, &length, 2);
	buf += 2;
	
	memcpy(buf, strName.c_str(), strName.size());
	buf += strName.size();
	
	buf += EncodeBoolean(buf, bVal);
	
	return buf - output;
}

int CRTMP::EncodeString(char *output, const std::string &strValue)
{
	char *buf = output;
	*buf = 0x02; // Datatype: String
	buf++;
	
	short length = htons(strValue.size());
	memcpy(buf, &length, 2);
	buf += 2;
	
	memcpy(buf, strValue.c_str(), strValue.size());
	buf += strValue.size();
	
	return buf - output;
}

int CRTMP::EncodeNumber(char *output, double dVal)
{
	char *buf = output;  
	*buf = 0x00; // type: Number
	buf++;
	
	WriteNumber(buf, dVal);
	buf += 8;
	
	return buf - output;
}

int CRTMP::EncodeBoolean(char *output, bool bVal)
{
	char *buf = output;  
	
	*buf = 0x01; // type: Boolean
	buf++;
	
	*buf = bVal?0x01:0x00; 
	buf++;
	
	return buf - output;
}


bool CRTMP::SendRTMP(RTMPPacket &packet)
{
	const RTMPPacket &prevPacket = m_vecChannelsOut[packet.m_nChannel];
	if (packet.m_headerType != RTMP_PACKET_SIZE_LARGE)
	{
		// compress a bit by using the prev packet's attributes
		if (prevPacket.m_nBodySize == packet.m_nBodySize && packet.m_headerType == RTMP_PACKET_SIZE_MEDIUM) 
			packet.m_headerType = RTMP_PACKET_SIZE_SMALL;
		
		if (prevPacket.m_nInfoField2 == packet.m_nInfoField2 && packet.m_headerType == RTMP_PACKET_SIZE_SMALL)
			packet.m_headerType = RTMP_PACKET_SIZE_MINIMUM;
		
	}
	
	if (packet.m_headerType > 3) // sanity
	{ 
		Log(LOGERROR, "sanity failed!! tring to send header of type: 0x%02x.", (unsigned char)packet.m_headerType);
		return false;
	}
	
	int nSize = packetSize[packet.m_headerType];
	char header[RTMP_LARGE_HEADER_SIZE] = { 0 };
	header[0] = (char)((packet.m_headerType << 6) | packet.m_nChannel);
	if (nSize > 1)
		EncodeInt24(header+1, packet.m_nInfoField1);
	
	if (nSize > 4)
	{
		EncodeInt24(header+4, packet.m_nBodySize);
		header[7] = packet.m_packetType;
	}
	
	if (nSize > 8)
		EncodeInt32LE(header+8, packet.m_nInfoField2);
	
	if (!WriteN(header, nSize))
	{
		Log(LOGWARNING, "%s: couldn't send rtmp header", __FUNCTION__);
		return false;
	}
	
	nSize = packet.m_nBodySize;
	char *buffer = packet.m_body;
	
	while (nSize)
	{
		int nChunkSize = packet.m_packetType == 0x14?m_chunkSize:packet.m_nBodySize;
		if (nSize < m_chunkSize)
			nChunkSize = nSize;
		
		if (!WriteN(buffer, nChunkSize))
			return false;
		
		nSize -= nChunkSize;
		buffer += nChunkSize;
		
		if (nSize > 0)
		{
			char sep = (0xc0 | packet.m_nChannel);
			if (!WriteN(&sep, 1))
				return false;  
		}
	}
	
	if (packet.m_packetType == 0x14) // we invoked a remote method, keep it in call queue till result arrives
		m_methodCalls.push_back(ReadString(packet.m_body + 1));
	
	m_vecChannelsOut[packet.m_nChannel] = packet;
	m_vecChannelsOut[packet.m_nChannel].m_body = NULL;
	return true;
}

void CRTMP::Close()
{
	if (IsConnected())
		close(m_socket);
	
	m_socket = 0;
	m_chunkSize = 128;
	m_nBWCheckCounter = 0;
	m_nBytesIn = 0;
	m_nBytesInSent = 0;
	
	for (int i=0; i<64; i++)
	{
		m_vecChannelsIn[i].Reset();
		m_vecChannelsIn[i].m_nChannel = i;
		m_vecChannelsOut[i].Reset();
		m_vecChannelsOut[i].m_nChannel = i;
	}
	
	m_bPlaying = false;
	m_nBufferSize = 0;
}

bool CRTMP::FillBuffer()
{
    assert(m_nBufferSize == 0); // only fill buffer when it's empty
    int nBytes = recv(m_socket, m_pBuffer, RTMP_BUFFER_CACHE_SIZE, 0);
    if(nBytes != -1) {
    	m_nBufferSize += nBytes;
		m_pBufferStart = m_pBuffer;
    }
    else
    {
		Log(LOGDEBUG, "%s, recv returned %d. GetSockError(): %d", __FUNCTION__, nBytes, GetSockError());
		Close();
		//      exit(10);
		return false;
    }
	
	return true;
}

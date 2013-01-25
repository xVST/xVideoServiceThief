/*  FLVStreamer
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

#include <string.h>
#include <sys/types.h>
#include "bytes.h"

// write dVal as 64bit little endian double
void WriteNumber(char *data, double dVal)
{
	uint64_t res;
	
#if __FLOAT_WORD_ORDER == __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
	res = *((uint64_t*)&dVal);
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint64_t in  = *((uint64_t*)&dVal);
	res = __bswap_64(in);
#endif
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN // __FLOAT_WORD_ORER == __BIG_ENDIAN
	uint32_t in1 = *((uint32_t*)&dVal);
	uint32_t in2 = *((uint32_t*)((char *)&dVal+4));
	
	in1 = __bswap_32(in1);
	in2 = __bswap_32(in2);
	
	res = ((uint64_t)in2<<32) | (uint64_t)in1;
#else // __BYTE_ORDER == __BIG_ENDIAN && __FLOAT_WORD_ORER == __LITTLE_ENDIAN
	uint32_t in1 = *((uint32_t*)&dVal);
	uint32_t in2 = *((uint32_t*)((char*)&dVal+4));
	
	res = ((uint64_t)in1<<32) | (uint64_t)in2;
#endif
#endif
	memcpy(data, &res, 8);
}

// reads a little endian 64bit double from data
double ReadNumber(const char *data)
{
#if __FLOAT_WORD_ORDER == __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
	double dVal;
	memcpy(&dVal, data, 8);
	return dVal;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint64_t in  = *((uint64_t*)data);
	uint64_t res = __bswap_64(in);
	return *((double *)&res);
#endif
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN // __FLOAT_WORD_ORER == __BIG_ENDIAN
	uint32_t in1 = *((uint32_t*)data);
	uint32_t in2 = *((uint32_t*)(data+4));
	
	in1 = __bswap_32(in1);
	in2 = __bswap_32(in2);
	
	uint64_t res = ((uint64_t)in2<<32) | (uint64_t)in1;
	return *((double *)&res);
#else // __BYTE_ORDER == __BIG_ENDIAN && __FLOAT_WORD_ORER == __LITTLE_ENDIAN
	uint32_t in1 = *((uint32_t*)data);
	uint32_t in2 = *((uint32_t*)(data+4));
	
	uint64_t res = ((uint64_t)in1<<32) | (uint64_t)in2;
	return *((double *)&res);
#endif
#endif
}

// read little-endian 32bit integer
int ReadInt32LE(const char *data)
{
	int val;
	memcpy(&val, data, sizeof(int));
	
#if __BYTE_ORDER == __BIG_ENDIAN
	val = __bswap_32(val);
#endif
	
	return val;
}

// write little-endian 32bit integer
int EncodeInt32LE(char *output, int nVal)
{
	
#if __BYTE_ORDER == __BIG_ENDIAN
	nVal = __bswap_32(nVal);
#endif
	
	memcpy(output, &nVal, sizeof(int));
	return sizeof(int);
}


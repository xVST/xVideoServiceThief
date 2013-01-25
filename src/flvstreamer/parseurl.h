#ifndef _PARSEURL_H_
#define _PARSEURL_H_
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

int hex2bin(char *str, char **hex);
bool ParseUrl(char *url, int *protocol, char **host, unsigned int *port, char **playpath, char **app);

#endif


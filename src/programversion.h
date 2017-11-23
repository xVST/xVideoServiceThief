/*
*
* This file is part of xVideoServiceThief,
* an open-source cross-platform Video service download
*
* Copyright (C) 2007 - 2017 Xesc & Technology
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
* Program URL   : http://www.xvideothief.com/
*
*/

#ifndef __PROGRAMVERSION_H__
#define __PROGRAMVERSION_H__

#include <QString>

static const QString PROGRAM_VERSION		= "3.0"; 	//!< Current program version
static const QString PROGRAM_VERSION_SHORT	= "3.0";	//!< Current program version (short version)
static const int	 PROGRAM_VERSION_NUMBER	= 300;		//!< Current program version (numeric version)

#ifdef Q_OS_WIN32
static const QString CURRENT_OS = "MSWINDOWS";	//!< Microsoft windows version
#endif
#ifdef Q_OS_LINUX
static const QString CURRENT_OS = "LINUX";		//!< Gnu/Linux version
#endif
#ifdef Q_OS_MACX
static const QString CURRENT_OS = "MACOSX";		//!< MacOS X version
#endif

#ifdef STATIC_BUILD
static const QString COMPILATION_MODE = "STATIC";		//!< Static compilation
#else
	#ifdef FRAMEWORKS_BUILD
static const QString COMPILATION_MODE = "FRAMEWORKS";	//!< Frameworks compilation
	#else
static const QString COMPILATION_MODE = "DYNAMIC";		//!< Dynamic compilation
	#endif
#endif

#endif // __PROGRAMVERSION_H__

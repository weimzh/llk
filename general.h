/***************************************************************************
 *   Copyright (C) 2005 by jhwu                                            *
 *   jhwu@redflag-linux.com                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA           *
 ***************************************************************************/

#ifndef _LLK_GENERAL_H_
#define _LLK_GENERAL_H_

//#define DEBUGGING

#define LLK_VERSION "0.1"

#if defined (__APPLE__)
#define LLK_INSTALL_DIR "./llk.app/Contents/Resources"
#elif defined (_WIN32)
#define LLK_INSTALL_DIR "."
#else
#define LLK_INSTALL_DIR "/usr/share/apps/llk"
#endif

#define EMPTY 0
#define DEFAULTDELAY 250
#define DEFAULTSHUFFLE 4
#define TOTALTIME 200
#define TIMEBONUS 2

#endif


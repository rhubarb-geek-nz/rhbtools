/**************************************************************************
 *
 *  Copyright 2008, Roger Brown
 *
 *  This file is part of Roger Brown's Toolkit.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

/* 
 * $Id: link386.h 7 2020-06-06 02:59:05Z rhubarb-geek-nz $
 */

/* generalised pragmas for linking */

/* _MSC_VER

		1200 = MSVC6
		1400 = MSVC8

  */

#ifdef LINK386_ASSASSIN
#	pragma comment(lib,"assassin.lib")
#endif

#ifdef LINK386_WSOCK32
#	if (_MSC_VER < 1200)
#		pragma comment(lib,"wsock32.lib")
#	else
#		ifdef _WIN32_WCE
#			pragma comment(lib,"ws2.lib")
#		else
#			pragma comment(lib,"ws2_32.lib")
#		endif
#	endif
#endif

#ifdef LINK386_WININET
#	if (_MSC_VER < 1200)
#	else
#		pragma comment(lib,"wininet.lib")
#	endif
#endif

#ifdef LINK386_KERNEL32
#	pragma comment(lib,"kernel32.lib")
#endif

#ifdef LINK386_USER32
#	ifndef _WIN32_WCE
#		pragma comment(lib,"user32.lib")
#	endif
#endif

#ifdef LINK386_ADVAPI32
#	ifndef _WIN32_WCE
#		pragma comment(lib,"advapi32.lib")
#	endif
#endif

#ifdef LINK386_GDI32
#	ifndef _WIN32_WCE
#		pragma comment(lib,"gdi32.lib")
#	endif
#endif

#ifdef LINK386_RPCRT4
#	pragma comment(lib,"rpcrt4.lib")
#endif

#if defined(LINK386_WINSCARD) && (_MSC_VER >= 1200)
#	pragma comment(lib,"winscard.lib")
#endif

#ifdef LINK386_MSXML2
#	ifndef _WIN32_WCE
#		pragma comment(lib,"msxml2.lib")
#	endif
#endif

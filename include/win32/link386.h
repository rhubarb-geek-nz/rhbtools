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
 * $Id: link386.h 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

/* generalised pragmas for linking */

/* _MSC_VER

		1200 = MSVC6
		1400 = MSVC8

  */

#if defined(_MSC_VER) && defined(_DEBUG)
#	if (_MSC_VER < 1200)
#		define USE_RHBOLE
#	else
#	endif
#endif

#ifdef LINK386_SOMTK
#	pragma comment(lib,"somtk.lib")
#endif

#ifdef LINK386_OPENDOC
#	pragma comment(lib,"opendoc.lib")
#endif

#ifdef LINK386_SOMOBJVA
#	pragma comment(lib,"somobjva.lib")
#endif

#ifdef LINK386_SOMOS
#	pragma comment(lib,"somos.lib")
#endif

#ifdef LINK386_SOM
#	pragma comment(lib,"som.lib")
#endif

#ifdef LINK386_SOMREF
#	pragma comment(lib,"somref.lib")
#endif

#ifdef LINK386_SOMABS1
#	pragma comment(lib,"somabs1.lib")
#endif

#ifdef LINK386_SOMCORBA
#	pragma comment(lib,"somcorba.lib")
#endif

#ifdef LINK386_SOMANY
#	pragma comment(lib,"somany.lib")
#endif

#ifdef LINK386_SOMTC
#	pragma comment(lib,"somtc.lib")
#endif

#ifdef LINK386_SOMDCOMM
#	pragma comment(lib,"somdcomm.lib")
#endif

#ifdef LINK386_SOMD
#	pragma comment(lib,"somd.lib")
#endif

#ifdef LINK386_SOMIR
#	pragma comment(lib,"somir.lib")
#endif

#ifdef LINK386_SOMP
#	pragma comment(lib,"somp.lib")
#endif

#ifdef LINK386_SOMS
#	pragma comment(lib,"soms.lib")
#endif

#ifdef LINK386_SOMU
#	pragma comment(lib,"somu.lib")
#endif

#if defined(LINK386_SOMULINK) && !defined(_WIN64)
#	pragma comment(lib,"somulink.lib")
#endif

#ifdef LINK386_SOMU2
#	pragma comment(lib,"somu2.lib")
#endif

#ifdef LINK386_SOMNMF
#	pragma comment(lib,"somnmf.lib")
#endif

#ifdef LINK386_SOMESTRM
#	pragma comment(lib,"somestrm.lib")
#endif

#ifdef LINK386_SOMEM
#	pragma comment(lib,"somem.lib")
#endif

#ifdef LINK386_SOMDDMSG
#	pragma comment(lib,"somddmsg.lib")
#endif

#ifdef LINK386_SOMCDR
#	pragma comment(lib,"somcdr.lib")
#endif

#ifdef LINK386_SOMCORBA
#	pragma comment(lib,"somcorba.lib")
#endif

#ifdef LINK386_SOMANY
#	pragma comment(lib,"somany.lib")
#endif

#ifdef LINK386_SOMDOM
#	pragma comment(lib,"somdom.lib")
#endif

#ifdef LINK386_RHBXTW32
#	pragma comment(lib,"rhbxtw32.lib")
#endif

#ifdef LINK386_RHBXTUTL
#	pragma comment(lib,"rhbxtutl.lib")
#endif

#ifdef LINK386_RHBCOMIF
#	pragma comment(lib,"rhbcomif.lib")
#endif

#ifdef LINK386_RHBRPC
#	pragma comment(lib,"rhbrpc.lib")
#endif

#ifdef LINK386_RHBORPC
#	pragma comment(lib,"rhborpc.lib")
#endif

#ifdef LINK386_RHBNDR
#	pragma comment(lib,"rhbndr.lib")
#endif

#ifdef LINK386_ASSASSIN
#	pragma comment(lib,"assassin.lib")
#endif

#ifdef LINK386_ODCM
#	pragma comment(lib,"odcm.lib")
#endif

#ifdef LINK386_ODPUBUTL
#	pragma comment(lib,"odpubutl.lib")
#endif

#ifdef LINK386_ODIMAGNG
#	pragma comment(lib,"odimagng.lib")
#endif

#ifdef LINK386_ODLAYOUT
#	pragma comment(lib,"odlayout.lib")
#endif

#ifdef LINK386_ODREGSTY
#	pragma comment(lib,"odregsty.lib")
#endif

#ifdef LINK386_ODSHELL
#	pragma comment(lib,"odshell.lib")
#endif

#ifdef LINK386_ODUI
#	pragma comment(lib,"odui.lib")
#endif

#ifdef LINK386_ODUTILS
#	pragma comment(lib,"odutils.lib")
#endif

#ifdef LINK386_ODDATAX
#	pragma comment(lib,"oddatax.lib")
#endif

#ifdef LINK386_ODSTORAG
#	pragma comment(lib,"odstorag.lib")
#endif

#ifdef LINK386_ODBINDNG
#	pragma comment(lib,"odbindng.lib")
#endif

#ifdef LINK386_ODCORE
#	pragma comment(lib,"odcore.lib")
#endif

#ifdef LINK386_ODSOMUC
#	pragma comment(lib,"odsomuc.lib")
#endif

#ifdef LINK386_IODUTILS
#	pragma comment(lib,"iodutils.lib")
#endif

#ifdef LINK386_IODSIMPL
#	pragma comment(lib,"iodsimpl.lib")
#endif

#ifdef LINK386_IODSHAPE
#	pragma comment(lib,"iodshape.lib")
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

#if defined(USE_RHBOLE)
#	ifdef LINK386_OLE32
#		pragma comment(lib,"rhbole32.lib")
#	endif

#	ifdef LINK386_OLEAUT32
#		pragma comment(lib,"rhbaut32.lib")
#	endif

#	ifdef LINK386_UUID
#		pragma comment(lib,"guid.lib")
#	endif
#else
#	ifdef LINK386_OLE32
#		pragma comment(lib,"ole32.lib")
#	endif

#	ifdef LINK386_OLEAUT32
#		pragma comment(lib,"oleaut32.lib")
#	endif

#	ifdef LINK386_UUID
#		pragma comment(lib,"uuid.lib")
#	endif
#endif


#ifdef LINK386_DCOM
#	pragma comment(lib,"dcom.lib")
#endif

#ifdef LINK386_WINSPOOL
#	pragma comment(lib,"winspool.lib")
#endif

#ifdef LINK386_COMDLG32
#	pragma comment(lib,"comdlg32.lib")
#endif

#ifdef LINK386_RHBATALK
#	pragma comment(lib,"rhbatalk.lib")
#endif

#ifdef LINK386_RHBMTUT
#	pragma comment(lib,"rhbmtut.lib")
#endif

#ifdef LINK386_RPCRT4
#	pragma comment(lib,"rpcrt4.lib")
#endif

#if defined(LINK386_WINSCARD) && (_MSC_VER >= 1200)
#	pragma comment(lib,"winscard.lib")
#endif

#ifdef LINK386_LIBUSB
#	ifdef _WIN64
#		pragma comment(lib,"libusb0_x64.lib")
#	else
#		pragma comment(lib,"libusb0.lib")
#	endif
#endif

#ifdef LINK386_LIBNFC
#	pragma comment(lib,"nfc.lib")
#endif

#ifdef LINK386_SHELL32
#	ifndef _WIN32_WCE
#		pragma comment(lib,"shell32.lib")
#	endif
#endif

#ifdef LINK386_LIBEXPAT
#	pragma comment(lib,"libexpat.lib")
#endif

#ifdef LINK386_MCPAPI
#	pragma comment(lib,"mcpapi.lib")
#endif

#ifdef LINK386_SSLEAY32
#	pragma comment(lib,"ssleay32.lib")
#endif

#ifdef LINK386_LIBEAY32
#	pragma comment(lib,"libeay32.lib")
#endif

#ifdef LINK386_LIBCURL
#	pragma comment(lib,"curllib.lib")
#endif

#ifdef LINK386_RHBHTTPS
#	pragma comment(lib,"rhbhttps.lib")
#endif

#ifdef LINK386_RHBHTTPW
#	pragma comment(lib,"rhbhttpw.lib")
#endif

#ifdef LINK386_RHBHTTPC
#	pragma comment(lib,"rhbhttpc.lib")
#endif

#ifdef LINK386_RHBCDRIO
#	pragma comment(lib,"rhbcdrio.lib")
#endif

#ifdef LINK386_MSXML2
#	ifndef _WIN32_WCE
#		pragma comment(lib,"msxml2.lib")
#	endif
#endif

#ifdef LINK386_HTMLHELP
#	ifndef _WIN32_WCE
#		pragma comment(lib,"htmlhelp.lib")
#	endif
#endif

#ifdef LINK386_HTMLVIEW
#	ifdef _WIN32_WCE
#		pragma comment(lib,"htmlview.lib")
#	endif
#endif

#ifdef LINK386_AYGSHELL
#	ifdef _WIN32_WCE
#		pragma comment(lib,"aygshell")
#	endif
#endif

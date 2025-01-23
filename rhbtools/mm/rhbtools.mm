; Copyright 2012, Roger Brown
; All rights reserved
; $Id: rhbtools.mm 1 2014-02-06 21:56:41Z rhubarb-geek-nz $

#define? UISAMPLE_DIALOG_FILE_dlgbmp	     
; #define? UISAMPLE_BLINE_TEXT          ©2012, Roger Brown
#define? UISAMPLE_BLINE_TEXT          
#define? UISAMPLE_BLINE_TEXT_WIDTH	0
#define? UISAMPLE_BLINE_TEXT_INDENT	0
#define? UISAMPLE_ADD_MSG_TO_BOTTOM_BAR  N
          
#define? COMPANY_WANT_TO_INSTALL_DOCUMENTATION   N

;--- Include MAKEMSI support (with my customisations and MSI branding) ------
#define VER_FILENAME.VER  rhbtools.ver      
#include "ME.MMH"

;--- Want to debug (not common) ---------------------------------------------
;#debug on
;#Option DebugLevel=^NONE, +OpSys^

<$DirectoryTree Key="INSTALLDIR" Dir="[ProgramFilesFolder]rhubarb.geek.nz\RHBtools" CHANGE="\" PrimaryFolder="Y">
<$DirectoryTree Key="INSTALLDIR1" Dir="[INSTALLDIR]bin" >
<$DirectoryTree Key="INSTALLDIR2" Dir="[INSTALLDIR]lib" >
<$DirectoryTree Key="INSTALLDIR3" Dir="[INSTALLDIR]include" >
<$DirectoryTree Key="INSTALLDIR4" Dir="[INSTALLDIR]etc" >
<$DirectoryTree Key="INSTALLDIR5" Dir="[INSTALLDIR]src" >

	<$Files "..\bin\when.exe" DestDir="INSTALLDIR1" >
	<$Files "..\bin\what.exe" DestDir="INSTALLDIR1" >
	<$Files "..\tools\chown.exe" DestDir="INSTALLDIR1" >
	<$Files "..\tools\regsvr32.exe" DestDir="INSTALLDIR1" >
	<$Files "..\tools\xargs.exe" DestDir="INSTALLDIR1" >
	<$Files "..\tools\find.exe" DestDir="INSTALLDIR1" >
	<$Files "..\tools\textconv.exe" DestDir="INSTALLDIR1" >
	<$Files "src\textconv.c" DestDir="INSTALLDIR5" >
	<$Files "src\chown.c" DestDir="INSTALLDIR5" >
	<$Files "src\when.c" DestDir="INSTALLDIR5" >
	<$Files "src\what.c" DestDir="INSTALLDIR5" >
	<$Files "src\regsvr32.c" DestDir="INSTALLDIR5" >
	<$Files "src\xargs.c" DestDir="INSTALLDIR5" >
	<$Files "src\find.c" DestDir="INSTALLDIR5" >

<$Feature "F_rhbtools_cpp" Title="cpp">
	<$Files "..\bin\cpp.exe" DestDir="INSTALLDIR1" >
	<$Files "src\cpp.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_curios" Title="curios">
	<$Files "..\tools\findapi.exe" DestDir="INSTALLDIR1" >
	<$Files "src\findapi.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_build" Title="build">
	<$Files "..\tools\version.exe" DestDir="INSTALLDIR1" >
	<$Files "src\version.c" DestDir="INSTALLDIR5" >
	<$Files "..\tools\depends.exe" DestDir="INSTALLDIR1" >
	<$Files "src\depends.c" DestDir="INSTALLDIR5" >
	<$Files "..\tools\depvers.exe" DestDir="INSTALLDIR1" >
	<$Files "src\depvers.c" DestDir="INSTALLDIR5" >
	<$Files "..\bin\svninfo.exe" DestDir="INSTALLDIR1" >
	<$Files "src\svninfo.c" DestDir="INSTALLDIR5" >
	<$Files "..\tools\config.exe" DestDir="INSTALLDIR1" >
	<$Files "src\config.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_hex" Title="hex">
	<$Files "..\bin\hexdump.exe" DestDir="INSTALLDIR1" >
	<$Files "src\hexdump.c" DestDir="INSTALLDIR5" >
	<$Files "..\bin\binhex.exe" DestDir="INSTALLDIR1" >
	<$Files "src\binhex.c" DestDir="INSTALLDIR5" >
	<$Files "..\bin\unbinhex.exe" DestDir="INSTALLDIR1" >
	<$Files "src\unbinhex.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_socket" Title="socket">
	<$Files "..\bin\socket.exe" DestDir="INSTALLDIR1" >
	<$Files "src\socket.c" DestDir="INSTALLDIR5" >
	<$Files "..\bin\tcpiptry.exe" DestDir="INSTALLDIR1" >
	<$Files "src\tcpiptry.c" DestDir="INSTALLDIR5" >
	<$Files "..\bin\wol.exe" DestDir="INSTALLDIR1" >
	<$Files "src\wol.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_idl" Title="idl">
	<$Files "..\tools\idltool.exe" DestDir="INSTALLDIR1" >
	<$Files "src\idltool.c" DestDir="INSTALLDIR5" >
<$/Feature>

<$Feature "F_rhbtools_inetserv" Title="inetserv">
	<$Files "..\bin\inetserv.exe" DestDir="INSTALLDIR1" >
	<$Files "src\inetserv.c" DestDir="INSTALLDIR5" >
<$/Feature>

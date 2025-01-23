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
 * $Id: regsvr32.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <windows.h>

int main(int argc,char **argv)
{
	int i=1;
	char *fn="DllRegisterServer";
	int rc=0;

	while (i < argc)
	{
		char *p=argv[i++];

		if ((*p=='-')||(*p=='/'))
		{
			p++;
			if (!strcmp(p,"u"))
			{
				fn="DllUnregisterServer";
			}
			p=NULL;
		}

		if (p)
		{
			HMODULE h=LoadLibrary(p);

			if (h)
			{
				char buf[256];
				typedef int (CALLBACK *ex_t)(void);

				ex_t ex=(ex_t)GetProcAddress(h,fn);

				GetModuleFileName(h,buf,sizeof(buf));

				if (ex)
				{
					int r=ex();
					if (r)
					{
						char buf[4096];
						int i=FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS|
							FORMAT_MESSAGE_FROM_SYSTEM,
							NULL,
							r,0,buf,sizeof(buf),0);
						if (i > 0)
						{
							DWORD dw=0;
							WriteFile(GetStdHandle(STD_ERROR_HANDLE),buf,i,&dw,NULL);
						}
						rc=1;
					}
				}

/*				FreeLibrary(h);*/
			}
		}
	}

	return rc;
}

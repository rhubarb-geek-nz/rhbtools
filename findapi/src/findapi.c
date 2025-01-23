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
 * $Id: findapi.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <windows.h>

#include <stdio.h>

int main(int argc,char **argv)
{
	WIN32_FIND_DATA wfd;
	HANDLE h;
	char *mask="*.dll";


	if (argc > 2)
	{
		mask=argv[2];
	}

	h=FindFirstFile(mask,&wfd);

	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			HANDLE m=LoadLibrary(wfd.cFileName);

			if (m)
			{
				if (GetProcAddress(m,argv[1]))
				{
					printf("%s\n",wfd.cFileName);
				}

				FreeLibrary(m);
			}
		} while (FindNextFile(h,&wfd));

		FindClose(h);
	}
	return 0;
}

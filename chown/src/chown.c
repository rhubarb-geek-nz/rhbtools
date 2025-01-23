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
 * $Id: chown.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <windows.h>
#include <stdio.h>
#include <rhbutils.h>
/*#include <aclapi.h>
#pragma comment(lib,"advapi32")
*/
int main(int argc,char **argv)
{
	int err=1;

	if (argc > 1)
	{
		char *user=argv[1];
		char sid[1024];
		SID_NAME_USE use=SidTypeUnknown;
		DWORD sidSize=sizeof(sid);
		char domName[256];
		SID *psid=(void *)sid;
		DWORD domSize=sizeof(domName);
		BOOL b=LookupAccountName(NULL,user,psid,&sidSize,domName,&domSize,&use);

		if (b)
		{
			SECURITY_DESCRIPTOR sd;
			int i=2;

			b=InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
			b=SetSecurityDescriptorOwner(&sd,psid,FALSE);

			err=0;

			while (i < argc)
			{
				const char *f=argv[i++];

/*				DWORD dw=SetNamedSecurityInfo(argv[i],
						SE_FILE_OBJECT,
						OWNER_SECURITY_INFORMATION,psid,NULL,NULL,NULL);
				if (dw)
				{
					err=1;
					printf("%s: failed with %d\n",argv[i],GetLastError());
				}
*/
				b=SetFileSecurity(f,OWNER_SECURITY_INFORMATION,&sd);

				if (!b)
				{
					err=1;

					rhbutils_print_Win32Error(GetLastError(),
						"SetFileSecurity(%s,OWNER_SECURITY_INFORMATION,...\n",f);

				}
			}
		}
		else
		{
			printf("failed to find username \"%s\"\n",user);
		}
	}
	else
	{
		printf("usage: %s user files....\n");
	}

	return err;
}

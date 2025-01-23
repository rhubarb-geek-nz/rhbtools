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
 * $Id: version.c 6 2020-06-06 01:48:03Z rhubarb-geek-nz $
 */

#include <windows.h>
#include <stdio.h>

static void rhbutils_print_Win32Error(unsigned long err)
{
	char buf[1024];
	HANDLE h=GetStdHandle(STD_ERROR_HANDLE);
	DWORD dw=FormatMessage(
		FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		buf,
		sizeof(buf)-2,
		NULL);

	if (h==INVALID_HANDLE_VALUE)
	{
#ifdef _CONSOLE
		buf[dw++]='\n';
		fwrite(buf,1,dw,stderr);
		fflush(stderr);
#else
		char app[256];
		char *p=app+GetModuleFileName(NULL,app,sizeof(app));
		while (p > app)
		{
			p--;
			if (*p=='\\')
			{
				p++;
				break;
			}
		}
		buf[dw]=0;
		MessageBox(NULL,buf,p,MB_OK|MB_ICONERROR);
#endif
	}
	else
	{
		buf[dw++]='\r';
		buf[dw++]='\n';
		WriteFile(h,buf,dw,&dw,NULL);
	}
}

#pragma comment(lib,"version.lib")

static const char *varnames[]={
	"Comments",
	"CompanyName",
	"FileDescription",
	"FileVersion",
	"InternalName",
	"LegalCopyright",
	"OriginalFilename",
	"ProductName",
	"ProductVersion",
	"SubProductName",
	"SubProductVersion",
	"OLESelfRegister"
};

static int dumpbin_header(char *p,long *majorVersion,long *minorVersion,char *type,size_t typeLen)
{
	int rc=1;
	char buf[512];
	FILE *fp;

	_snprintf(buf,sizeof(buf),"dumpbin /HEADERS \"%s\"",p);

	fp=_popen(buf,"r");

	if (fp)
	{
		while (fgets(buf,sizeof(buf),fp))
		{
			p=buf+strlen(buf);
			while (p > buf)
			{
				p--;
				if (*p > ' ')
				{
					break;
				}
				*p=0;
			}
			if (buf[0])
			{
				size_t bl=strlen(buf);
				const char *q=" image version";
				size_t ql=strlen(q);
				if (bl > ql)
				{
					if (!memicmp(q,buf+bl-ql,ql))
					{
						char other[sizeof(buf)];
						int i=sscanf(buf,"%ld.%ld %s",majorVersion,minorVersion,other);
						if (i==3)
						{
							rc=0;
							break;
						}
						break;
					}
				}
				if (!memicmp("File Type:",buf,10))
				{
					p=buf+strlen(buf);

					while (p > buf)
					{
						p--;
						if (*p <= ' ')
						{
							strncpy(type,p+1,typeLen);
							break;
						}
					}
				}
			}
		}

		_pclose(fp);
	}

	return rc;
}

static int show_vers(char *p)
{
	DWORD dwZero=0;
	DWORD dwLen=GetFileVersionInfoSize(p,&dwZero);
	LPVOID data=0;
	int rc=0;
	long dumpbin_major=0,dumpbin_minor=0;
	char fileType[256]={0};

	if (!dwLen) 
	{
		DWORD dw=GetLastError();

		fprintf(stderr,"file %s\n",p); fflush(stderr);

		rhbutils_print_Win32Error(dw);

		return 1;
	}

	rc=dumpbin_header(p,&dumpbin_major,&dumpbin_minor,fileType,sizeof(fileType));

	if (rc)
	{
		return 1;
	}

	data=LocalAlloc(LMEM_FIXED,dwLen);

	if (!data) 
	{
		DWORD dw=GetLastError();

		fprintf(stderr,"file %s\n",p); fflush(stderr);

		rhbutils_print_Win32Error(dw);

		return 1;
	}

	if (GetFileVersionInfo(p,dwZero,dwLen,data))
	{
		LPVOID pv=0;
		UINT ui=0;
		char filevers[128]={0};
		char prodvers[128]={0};
		int original_check=1;

		if (VerQueryValue(data,"\\",&pv,&ui))
		{
			VS_FIXEDFILEINFO *v=pv;

			_snprintf(filevers,sizeof(filevers),"%d.%d.%d.%d",
					HIWORD(v->dwFileVersionMS),
					LOWORD(v->dwFileVersionMS),
					HIWORD(v->dwFileVersionLS),
					LOWORD(v->dwFileVersionLS));

			_snprintf(prodvers,sizeof(prodvers),"%d.%d.%d.%d",
					HIWORD(v->dwProductVersionMS),
					LOWORD(v->dwProductVersionMS),
					HIWORD(v->dwProductVersionLS),
					LOWORD(v->dwProductVersionLS));

			if (
				(HIWORD(v->dwFileVersionMS)!=dumpbin_major)||
				(LOWORD(v->dwFileVersionMS)!=dumpbin_minor)
			  )
			{
				/* we will include both EXE and DLL in this test */

				if (!stricmp(fileType,"DLL"))
				{
					fprintf(stderr,"%s, %s, Image version %ld.%ld does not match file version %s\n",
						p,
						fileType,
						dumpbin_major,
						dumpbin_minor,
						filevers);

					rc=1;
				}
			}
		}
		else
		{
			DWORD dw=GetLastError();

			fprintf(stderr,"file %s\n",p); fflush(stderr);

			rhbutils_print_Win32Error(dw);

			rc=1;
		}

		if (VerQueryValue(data,"\\VarFileInfo\\Translation",&pv,&ui))
		{
			BYTE *pb=pv;

			while (ui)
			{
				char trans[32];
				int k=sizeof(varnames)/sizeof(varnames[0]);

				_snprintf(trans,sizeof(trans),"%02x%02x%02x%02x",
					pb[1],pb[0],pb[3],pb[2]);

				ui-=4;
	
				printf("trans=%s\n",trans);

				while (k--)
				{
					char key[256];
					const char *vn=varnames[k];
					LPVOID pv2=0;
					UINT ui2=0;

					_snprintf(key,sizeof(key),"\\StringFileInfo\\%s\\%s",trans,vn);

					if (VerQueryValue(data,key,&pv2,&ui2))
					{
						printf("%s: %s\n",vn,(const char *)pv2);

						if (!stricmp(vn,"FileVersion"))
						{
							if (stricmp(pv2,filevers))
							{
								fprintf(stderr,"%s: %s != %s\n",vn,(const char *)pv2,filevers);

								rc=1;
							}
						}

						if (!stricmp(vn,"ProductVersion"))
						{
							if (stricmp(pv2,prodvers))
							{
								fprintf(stderr,"%s: %s != %s\n",vn,(const char *)pv2,prodvers);

								rc=1;
							}
						}

						if (!stricmp(vn,"OriginalFilename"))
						{
							const char *q=p+strlen(p);

							while (q > p)
							{
								q--;

								if ((*q==':')||(*q=='/')||(*q=='\\'))
								{
									q++;
									break;
								}
							}

							if (stricmp(q,pv2))
							{
								fprintf(stderr,"%s: %s != %s\n",vn,(const char *)pv2,q);

								rc=1;
							}

							original_check=0;
						}
					}
				}
			}

			if (original_check)
			{
				fprintf(stderr,"No OriginalFilename check\n");
				rc=1;
			}
		}
		else
		{
			DWORD dw=GetLastError();

			fprintf(stderr,"%s, \\VarFileInfo\\Translation: ",p);
			fflush(stderr);

			rhbutils_print_Win32Error(dw);

			rc=1;
		}
	}
	else
	{
		DWORD dw=GetLastError();

		fprintf(stderr,"file %s\n",p); fflush(stderr);

		rhbutils_print_Win32Error(dw);

		rc=1;
	}

	LocalFree(data);

	return rc;
}

int main(int argc,char **argv)
{
	int i=1;
	int rc=0;

	while (i < argc)
	{
		char *p=argv[i++];

		if ((*p=='-')||(*p=='/'))
		{
		}
		else
		{
			rc|=show_vers(p);
		}
	}

	return rc;
}

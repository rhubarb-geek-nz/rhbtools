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
 * $Id: dirent.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */


#include <windows.h>
#include <stdlib.h>
#include <dirent.h>

struct tagDIR
{
	HANDLE h;
	WIN32_FIND_DATAA wfd;
	struct dirent entry;
};


DIR *opendir(const char *path)
{
	char buf[256];
	DIR *dir=malloc(sizeof(*dir));

	strncpy(buf,path,sizeof(buf));
	if (buf[0])
	{
		switch (buf[strlen(buf)-1])
		{
		case '/':
		case '\\':
			buf[strlen(buf)-1]=0;
			break;
		}
	}
	strncat(buf,"\\*",sizeof(buf));
	dir->h=FindFirstFileA(buf,&dir->wfd);
	if (dir->h==INVALID_HANDLE_VALUE)
	{
		free(dir);
		dir=NULL;
	}
	return dir;
}

void closedir(DIR *dir)
{
	if (dir->h != INVALID_HANDLE_VALUE)
	{
		FindClose(dir->h);
	}
	free(dir);
}

int readdir_r(DIR *dir,struct dirent *ent,struct dirent **pe)
{
	int rc=-1;

	if (dir->wfd.cFileName[0])
	{
		strncpy(ent->d_name,dir->wfd.cFileName,sizeof(ent->d_name));
		rc=0;
	}

	if (dir->h!=INVALID_HANDLE_VALUE)
	{
		if (FindNextFileA(dir->h,&dir->wfd))
		{
		}
		else
		{
			dir->wfd.cFileName[0]=0;
			FindClose(dir->h);
			dir->h=INVALID_HANDLE_VALUE;
		}
	}

	return rc;
}

struct dirent *readdir(DIR *d)
{
static struct
{
	struct dirent entry;
	char spare_room[256];
} entry;
struct dirent *r=&entry.entry;

	if (!readdir_r(d,&entry.entry,&r))
	{
		return &entry.entry;
	}

	return NULL;
}

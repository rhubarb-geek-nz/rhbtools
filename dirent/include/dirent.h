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
 * $Id: dirent.h 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */


#ifndef __READDIR_H__
#define __READDIR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct dirent
{
	char d_name[256];
};

typedef struct tagDIR DIR;

DIR *opendir(const char *path);
void closedir(DIR *dir);
int readdir_r(DIR *dir,struct dirent *ent,struct dirent **pe);
struct dirent *readdir(DIR *d);

#ifdef __cplusplus
}
#endif

#endif

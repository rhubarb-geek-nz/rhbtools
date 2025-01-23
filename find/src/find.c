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
 * $Id: find.c 16 2021-05-06 23:39:34Z rhubarb-geek-nz $
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


static int regexpr_match(const char *str,size_t slen,const char *pat,size_t plen)
{
	while (slen)
	{
		size_t x=0;

		if (!plen) return -1;

		while ((x < plen) && (pat[x]!='?') && (pat[x]!='*'))
		{
			x++;
		}

		if (x)
		{
			if (x > slen)
			{
				return -1;
			}

#if defined(_WIN32) || defined(__OS2__)
			if (memicmp(str,pat,x))
#else
			if (memcmp(str,pat,x))
#endif
			{
				return -1;
			}
		}
		else
		{
			if (pat[0]=='?')
			{
				x=1;
			}
			else
			{
				if (pat[0]=='*')
				{
					pat++;
					plen--;

					if (plen)
					{
						while (x < slen)
						{
							if (!regexpr_match(str+x,slen-x,pat,plen))
							{
								return 0;
							}

							x++;
						}

						return -1;
					}

					return 0;
				}
				else
				{
#if defined(_WIN32) && defined(_M_IX86)
					__asm int 3
#endif
					return -1;
				}
			}
		}

		slen-=x;
		str+=x;
		plen-=x;
		pat+=x;
	}

	while (plen)
	{
		if (pat[0]=='*')
		{
			pat++;
			plen--;
		}
		else
		{
			break;
		}
	}

	if (plen) 
	{
		return -1;
	}

	return 0;
}

static void finddefs(const char *root,char wantType,const char *filename)
{
size_t namelen=filename ? strlen(filename) : 0;
DIR *dir=opendir(root);

	while (dir)
	{
		struct dirent *de=readdir(dir);

		if (de)
		{
			if (de->d_name[0]!='.')
			{
				struct stat s;
				char buf[512];

				strncpy(buf,root,sizeof(buf)-1);
				strncat(buf,"/",sizeof(buf)-1);
				strncat(buf,de->d_name,sizeof(buf)-1);

				if (!stat(buf,&s))
				{
					char actType='?';

					switch (s.st_mode & S_IFMT)
					{
					case S_IFDIR:
						actType='d';
						break;
					case S_IFREG:
						actType='f';
						break;
					default:
						break;
					}

					if ((!wantType) || (wantType==actType))
					{
						if (filename)
						{
							if (!regexpr_match(de->d_name,strlen(de->d_name),filename,namelen))
							{
								printf("%s\n",buf);
							}
						}
						else
						{
							printf("%s\n",buf);
						}
					}

					if (actType=='d')
					{
						finddefs(buf,wantType,filename);
					}
				}
			}
		}
		else
		{
			closedir(dir);
			dir=0;
		}
	}
}

int main(int argc,char **argv)
{
const char *root_dir=".";
const char *filename=NULL;
char wantType=0;
int i=2;

	if (argc > 1) root_dir=argv[1];

	while (i < argc)
	{
		char *p=argv[i++];

		if ((p[0]=='/')||(p[0]=='-'))
		{
			if (!strcmp(p+1,"type"))
			{
				p=argv[i++];

				if (p)
				{
					wantType=p[0];
				}
			}
			else
			{
				if (!strcmp(p+1,"name"))
				{
					filename=argv[i++];
				}
				else
				{
					fprintf(stderr,"%s: arg error \"%s\"\n",argv[0],p);
					return 1;
				}
			}
		}
		else
		{
			fprintf(stderr,"%s: arg error \"%s\"\n",argv[0],p);
			return 1;
		}
	}

	finddefs(root_dir,wantType,filename);

	return 0;
}

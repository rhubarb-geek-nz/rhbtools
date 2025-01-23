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
 * $Id: stat.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#else
#	include <pwd.h>
#	include <grp.h>
#endif

int main(int argc,char **argv)
{
	int flag_x=0,
		flag_a=0,
		flag_c=0,
		flag_m=0,
		flag_l=0,
		flag_i=0,
		flag_g=0,
		flag_u=0,
		flag_d=0;
	const char *app=argv[0];

	while (--argc)
	{
		const char *p=*++argv;

		if (p && p[0])
		{
			if (*p=='-')
			{
				switch (p[1])
				{
				case 'x':
					flag_x=!flag_x;
					break;
				case 'a':
					flag_a=!flag_a;
					break;
				case 'l':
					flag_l=!flag_l;
					break;
				case 'c':
					flag_c=!flag_c;
					break;
				case 'm':
					flag_m=!flag_m;
					break;
				case 'd':
					flag_d=!flag_d;
					break;
				case 'i':
					flag_i=!flag_i;
					break;
				case 'u':
					flag_u=!flag_u;
					break;
				case 'g':
					flag_g=!flag_g;
					break;
				default:
					fprintf(stderr,"%s: unknown arg %s\n",app,p);
					return 1;
				}
			}
			else
			{
				const char *pval=NULL;
				struct stat s;
				time_t value=0;

				if (stat(p,&s))
				{
					perror(p);
					return 1;
				}

				if (flag_c)
				{
					value=s.st_ctime;
				}
				else
				{
					if (flag_m)
					{
						value=s.st_mtime;
					}
					else
					{
						if (flag_a)
						{
							value=s.st_atime;
						}
						else
						{
							if (flag_l)
							{
								value=s.st_size;
							}
							else
							{
								if (flag_i)
								{
									value=s.st_ino;
								}
								else
								{
									if (flag_d)
									{
										value=s.st_dev;
									}
									else
									{
										if (flag_u)
										{
#ifdef _WIN32
											value=s.st_uid;
#else
											struct passwd *pw=getpwuid(s.st_uid);
											value=s.st_uid;
											if (pw)
											{
												pval=pw->pw_name;
											}
#endif
										}
										else
										{
											if (flag_g)
											{
#ifdef _WIN32
												value=s.st_gid;
#else
												struct group *g=getgrgid(s.st_gid);
												value=s.st_gid;
												if (g)
												{
													pval=g->gr_name;
												}
#endif
											}
										}
									}
								}
							}
						}
					}
				}

				if (flag_x)
				{
					printf("%08lX\n",(long)value);
				}
				else
				{
					if (pval)
					{
						printf("%s\n",pval);
					}
					else
					{
						printf("%ld\n",(long)value);
					}
				}
			}
		}
	}

	return 0;
}

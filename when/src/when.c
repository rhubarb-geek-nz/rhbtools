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
 * $Id: when.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <rhbopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

static time_t read_value(const char *value,const char *fmt)
{
	struct tm tm;

	memset(&tm,0,sizeof(tm));

	tm.tm_isdst=-1;

	while (*fmt)
	{
		char f=*fmt;

		switch (f)
		{
		case 'Y':
		case 'M':
		case 'D':
		case 'h':
		case 'm':
		case 's':
			{
				int val=0;

				while (f==*fmt)
				{
					char d=*value++;
					fmt++;

					if ((d<'0')||(d>'9'))
					{
						fprintf(stderr,"date value error at \"%s\"\n",value);
						exit(1);
					}

					val=(val * 10)+(d-'0');
				}

				switch (f)
				{
				case 'Y':
					tm.tm_year=(val-1900);
					break;
				case 'M':
					tm.tm_mon=(val-1);
					break;
				case 'D':
					tm.tm_mday=val;
					break;
				case 'h':
					tm.tm_hour=(val%24);
					break;
				case 'm':
					tm.tm_min=val;
					break;
				case 's':
					tm.tm_sec=val;
					break;
				default:
					fprintf(stderr,"date format error at \'%c\'\n",f);
					exit(1);
				}
			}
			break;
		default:
			if (f!=*value)
			{
				fprintf(stderr,"date format error at \'%c\' != \'%c\'\n",f,*value);
				exit(1);
			}
			value++;
			break;
		}
	}

	return mktime(&tm);
}

static void write_value(time_t t,const char *fmt)
{
	struct tm *tmp=localtime(&t);
	
	if (tmp)
	{
		struct tm tm;
		
		tm=*tmp;

		while (*fmt)
		{
			char f=*fmt;
			int i=1;
			int val=0;

			while (f==fmt[i])
			{
				i++;
			}

			switch (f)
			{
			case 'Y':
				val=1900+tm.tm_year;
				break;
			case 'M':
				val=1+tm.tm_mon;
				break;
			case 'D':
				val=tm.tm_mday;
				break;
			case 'h':
				val=tm.tm_hour;
				break;
			case 'm':
				val=tm.tm_min;
				break;
			case 's':
				val=tm.tm_sec;
				break;
			default:
				{
					int k=i;

					while (k--)
					{
						printf("%c",f);
					}
				}
				val=-1;
				break;
			}

			if (val>=0)
			{
				char buf[9];
				int k=(i-8);

				snprintf(buf,sizeof(buf),"%08d",val);

				while (k > 0)
				{
					printf("0");

					k--;
				}

				printf("%s",buf+(8-i));
			}

			fmt+=i;
		}
	}

	printf("\n");
}

int main(int argc,char **argv)
{
	int flag_x=0;
	const char *app=argv[0];
	const char *output_format=NULL;
	const char *input_format=NULL;

	while (--argc)
	{
		const char *p=*++argv;

		if (p && p[0])
		{
			time_t when=0;

			if (*p=='-')
			{
				char cmd=p[1];

				switch (cmd)
				{
				case 'x':
					flag_x=!flag_x;
					break;
				case 'n':
					time(&when);
					break;
				case 'i':
				case 'o':
				case 'f':
				case 'v':
				case 'h':
				case 'd':
					if (p[2])
					{
						p+=2;
					}
					else
					{
						argc--;
						p=*++argv;
					}

					if (!p)
					{
						fprintf(stderr,"unexpected NULL\n");
						return 1;
					}

					switch (cmd)
					{
					case 'h':
						{
							long n=0;
							if (sscanf(p,"%lx",&n)>0)
							{
								when=(time_t)n;
							}
							else
							{
								fprintf(stderr,"sscanf(x) on \"%s\" failed\n",p);
								return 1;
							}
						}
						break;
					case 'd':
						{
							long n=0;
							if (sscanf(p,"%ld",&n)>0)
							{
								when=(time_t)n;
							}
							else
							{
								fprintf(stderr,"sscanf(d) on \"%s\" failed\n",p);
								return 1;
							}
						}
						break;
					case 'i':
						input_format=p;
						break;
					case 'o':
						output_format=p;
						break;
					case 'f':
						{
							struct stat s;

							if (stat(p,&s))
							{
								perror(p);
								return 1;
							}

							when=s.st_mtime;
						}
						break;
					case 'v':
						when=read_value(p,input_format);
						break;
					}
					break;
				default:
					fprintf(stderr,"%s: unknown arg %s\n",app,p);
					return 1;
				}
			}
			else
			{
				fprintf(stderr,"unexpected argument %s\n",p);
				return 1;
			}

			if (when && (when!=(time_t)-1L))
			{
				if (output_format)
				{
					write_value(when,output_format);
				}
				else
				{
					if (flag_x)
					{
						printf("%08lX\n",(long)when);
					}
					else
					{
						printf("%ld\n",(long)when);
					}
				}
			}
		}
	}

	return 0;
}

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
 * $Id: what.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char signature[]="@(#)";
char i_am[]="@(#)what 1.0";
static char terminators[]={'\"','>','\n','\\',0};

static void what(FILE *fp)
{
	int so_far=0;

	while (1)
	{
		int c=fgetc(fp);

		if (c==EOF) break;

		if (c==signature[so_far])
		{
			so_far++;

			if (!signature[so_far])
			{
				int length=0;

				while (1)
				{
					int i=sizeof(terminators);

					c=fgetc(fp);

					if (c==EOF) break;

					while (i--)
					{
						if (c==terminators[i]) 
						{
							c=0;

							break;
						}
					}

					if (c)
					{
						if (!length++)
						{
							printf("\t");
						}

						printf("%c",c);
					}
					else
					{
						break;
					}
				}

				if (length)
				{
					printf("\n");

					fflush(stdout);
				}

				so_far=0;

				if (c==EOF) break;
			}
		}
		else
		{
			so_far=0;
		}
	}
}

int main(int argc,char **argv)
{
	if (argc > 1)
	{
		int i=1;

		while (i <argc)
		{
			FILE *fp=fopen(argv[i],"rb");

			if (fp)
			{
				printf("%s:\n",argv[i]);
				what(fp);
				fclose(fp);	
			}

			i++;
		}
	}
	else
	{
		what(stdin);
	}

	return 0;
}

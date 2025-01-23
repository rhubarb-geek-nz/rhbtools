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
 * $Id: depends.c 1 2014-02-06 21:56:41Z rhubarb-geek-nz $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct part
{
	struct part *next;
	struct depends *depends;
	struct provides *provides;
	char name[1];
};

struct depends
{
	struct depends *next;
	char label[1];
};

struct provides
{
	struct provides *next;
	char label[1];
};

static struct part *parts;
static struct part *makelist;

int my_fgetc(FILE *fp)
{
	int i=fgetc(fp);

	return i;
}

#define DEP_NAME		1
#define DEP_PROVIDES	2
#define DEP_DEPENDS		3


static struct {
	const char *name;
	int value;
} flags[]={
	{"NAME",DEP_NAME},
	{"PROVIDES",DEP_PROVIDES},
	{"DEPENDS",DEP_DEPENDS}};

static int get_flag(const char *p)
{
	int i=sizeof(flags)/sizeof(flags[0]);
	while (i--)
	{
		if (!strcmp(p,flags[i].name))
		{
			return flags[i].value;
		}
	}
	return 0;
}

static void remove_from_list(struct part *part,struct part **list)
{
	if (part && *list)
	{
		if (part==list[0])
		{
			list[0]=part->next;
		}
		else
		{
			struct part *p=list[0];

			while (p)
			{
				if (p->next==part)
				{
					p->next=part->next;
					break;
				}

				p=p->next;
			}
		}
	}
}

static struct part *find_provider(const char *name,struct part **list)
{
	struct part *p=list[0];

	while (p)
	{
		struct provides *n=p->provides;

		while (n)
		{
			if (!strcmp(name,n->label))
			{
				return p;
			}

			n=n->next;
		}

		p=p->next;
	}

	return 0;
}

static void do_read_config(FILE *fp)
{
char name[256];
struct depends *depends=NULL;
struct provides *provides=NULL;
int n=0;
char text[256];
int textLen=0;
int flag=0;

	name[0]=0;

	while (fp)
	{
		int ch=my_fgetc(fp);

		if (ch==EOF) break;

		switch (ch)
		{
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			text[textLen]=0;

			if (textLen)
			{
				if (n)
				{
					switch (flag)
					{
					case DEP_NAME:
						memcpy(name,text,textLen+1);
						break;
					case DEP_PROVIDES:
						{
							struct provides *p=calloc(sizeof(*p)+textLen,1);
							memcpy(p->label,text,textLen+1);
							p->next=provides;
							provides=p;
						}
						break;
					case DEP_DEPENDS:
						{
							struct depends *p=calloc(sizeof(*p)+textLen,1);
							memcpy(p->label,text,textLen+1);
							p->next=depends;
							depends=p;
						}
						break;
					}
				}
				else
				{
					flag=get_flag(text);
				}

				n++;

				textLen=0;
			}

			if (ch=='\n') 
			{
				n=0;
			}
			break;
		default:
			text[textLen++]=ch;
			break;
		}
	}

	if (name[0])
	{
		size_t i=strlen(name);
		struct part *p=calloc(sizeof(*p)+i,1);
		memcpy(p->name,name,i+1);
		p->depends=depends;
		p->provides=provides;
		p->next=parts;
		parts=p;
	}
}

int main(int argc,char **argv)
{
	char buf[16384];

	while (fgets(buf,sizeof(buf),stdin))
	{
		size_t i=strlen(buf);

		while (i)
		{
			if (buf[i-1]>' ') break;
			i--;
			buf[i]=0;
		}

		if (buf[0])
		{
			FILE *fp=fopen(buf,"r");

			if (!fp)
			{
				perror(buf);

				return 1;
			}

			do_read_config(fp);

			fclose(fp);
		}
	}

	if (argc==1)
	{
		makelist=parts;
		parts=0;
	}
	else
	{
		int i=1;

		while (i < argc)
		{
			const char *name=argv[i++];
			struct part *p=parts;

			while (p)
			{
				if (!strcmp(p->name,name))
				{
					remove_from_list(p,&parts);
					p->next=makelist;
					makelist=p;
					break;
				}

				p=p->next;
			}
		}
	}

	while (makelist)
	{
		struct part *p=makelist;

		while (p)
		{
			struct depends *d=p->depends;
			int changed=0;

			while (d)
			{
				struct part *n=find_provider(d->label,&parts);

				if (n)
				{
					remove_from_list(n,&parts);

					n->next=makelist;
					makelist=n;

					changed=1;
					break;
				}

				d=d->next;
			}

			if (changed) break;

			p=p->next;
		}

		if (!p) break;
	}

	while (makelist)
	{
		int reordered=0;
		struct part *p=makelist;

		while (p && !reordered)
		{
			struct depends *d=p->depends;

			while (d)
			{
				struct part *prov=find_provider(d->label,&p->next);

				if (prov)
				{
					remove_from_list(p,&makelist);

					p->next=prov->next;
					prov->next=p;

					reordered=1;
					break;
				}

				d=d->next;
			}

			if (p)
			{
				p=p->next;
			}
		}

		if (!reordered) break;
	}

	if (makelist)
	{
		struct part *p=makelist;

		while (p)
		{
			struct depends *d=p->depends;

			while (d)
			{
				if (!find_provider(d->label,&makelist))
				{
					fprintf(stderr,"Cannot find provider '%s' for %s'\n",
								d->label,
								p->name);

					return 1;
				}

				d=d->next;
			}

			p=p->next;
		}
	}

	if (makelist)
	{
		struct part *p=makelist;

		while (p)
		{
			printf("%s\n",p->name);
			p=p->next;
		}
	}

	return 0;
}

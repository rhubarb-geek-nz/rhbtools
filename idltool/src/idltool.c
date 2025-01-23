#include <rhbopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *append_slash(char *p,size_t s)
{
	size_t k=strlen(p);
	if (k)
	{
		char c=p[k-1];

		if ((c=='/')||(c=='\\'))
		{
		}
		else
		{
#ifdef _WIN32
			strncat(p,"\\",s);
#else
			strncat(p,"/",s);
#endif
		}
	}

	return p;
}

static void strncat_quote(char *buf,const char *src,size_t len)
{
	int k=(int)strlen(src);
	while (k--)
	{
		if ((src[k]==' ')||(src[k]=='\t')) break;
	}

	if (k>=0) strncat(buf,"\"",len);
	strncat(buf,src,len);
	if (k>=0) strncat(buf,"\"",len);
}

int main(int argc,char **argv)
{
	int iOarg=0;
	const char *tool=NULL;
	int i=2;
	char target[256];
	char *targetDir=0;
	const char *emitter=0;
	const char *basename=0;
	int rc=1;
	int found=0;

	target[0]=0;

	if (argc < 4)
	{
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"\t%s tool [source-directories....] -o target-file [args....]\n",argv[0]);

		return 1;
	}

	tool=argv[1];

	while (i < argc)
	{
		const char *p=argv[i];

		if (!strcmp(p,"-o"))
		{
			iOarg=i++;

			p=argv[i++];

			strncpy(target,p,sizeof(target));

			break;
		}

		i++;
	}

	if (target[0])
	{
		size_t k=strlen(target);

		while (k-- && !targetDir)
		{
			switch (target[k])
			{
			case '.':
				if (!emitter)
				{
					emitter=target+k+1;
					target[k]=0;
				}
				break;
			case '/':
			case '\\':
				target[k]=0;
				targetDir=target;
				basename=target+k+1;
				break;
			}
		}
	}
	else
	{
		fprintf(stderr,"%s: No target\n",argv[0]);
		return 1;
	}

#ifdef _DEBUGx
	printf("tool=%s\n",tool);
	printf("emitter=%s\n",emitter);
	printf("basename=%s\n",basename);
	printf("targetDir=%s\n",targetDir);
#endif

	i=2;

	while (i < iOarg)
	{
		const char *p=argv[i++];
		struct stat s;

		if (stat(p,&s))
		{
			perror(p);
		}
		else
		{
			char buf[256];

			strncpy(buf,p,sizeof(buf));
			
			append_slash(buf,sizeof(buf));

			strncat(buf,basename,sizeof(buf));
			strncat(buf,".idl",sizeof(buf));

			if (!stat(buf,&s))
			{
				char command[4096];

				found=1;

				command[0]=0;
				strncat_quote(command,tool,sizeof(command));

				if (strcmp(emitter,"idl"))
				{
					strncat(command," -s",sizeof(command));
					strncat(command,emitter,sizeof(command));
				}

				if (targetDir[0])
				{
					strncat(command," -d ",sizeof(command));
					strncat_quote(command,targetDir,sizeof(command));
				}

				strncat(command," ",sizeof(command));
				strncat_quote(command,buf,sizeof(command));

				i=iOarg+2;

				while (i < argc)
				{
					p=argv[i++];

					strncat(command," ",sizeof(command));
					strncat_quote(command,p,sizeof(command));
				}

				fprintf(stderr,"%s: %s\n",argv[0],command);

				rc=system(command);

				break;
			}
		}
	}

	if (!found)
	{
		fprintf(stderr,"%s: failed to find '%s.idl'\n",argv[0],basename);
	}

	return rc;
}

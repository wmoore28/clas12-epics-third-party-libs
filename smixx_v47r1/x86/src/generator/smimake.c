#include <stdio.h>

main(argc,argv)
int argc;
char **argv;										
{
char *ptr;
int i, index = 0;
char strin[80];
char path[128];
char in_file[80];
char ext[16];
char cmnd[128];
char smidir[80],xdir[20];
char cmnd_path[128];
int gen_flag = 0;
int ret;

	if(argc < 2)
	{
		printf("\nsmiMake [options] file\n\n");
		printf("DESCRIPTION:\n");
		printf("\tsmiMake Optionally generates Proxy skelettons\n"); 
		printf("\tand include files containing SMI object headers\n");
		printf("\tthen preprocesses and translates the \".SML\" code\n");
		printf("\tPossible Options:\n");
 		printf("\t\t-gp\tGenerate Proxies\n");
 		printf("\t\t-gi\tGenerate include files for SMI objects\n");
 		printf("\t\t-c\t(if -gp)Generate Proxies in \"C\" \n");
		printf("\t\t-C\t(if -gp)Generate Proxies in \"C++\" (default)\n");
		printf("\t\t-m\t(if -gp)Generate a makefile for the proxies\n");
 		exit(1);
	}
#ifdef unix 

	if( (ptr = getenv("SMIDIR")) == NULL )
	{
		printf("Environment variable SMIDIR not defined\n");
		exit(0);
	}
	strcpy(smidir,ptr);
	if( (ptr = getenv("XDIR")) == NULL )
	{
		printf("Environment variable XDIR not defined\n");
		exit(0);
	}
	strcpy(xdir,ptr);
#endif
	strcpy(cmnd,"smiGen");
	for(i = 1; i < argc; i++)
	{
		sprintf(strin,argv[i]);
		if(strin[0] == '-')
		{
			switch(strin[1])
			{
				case 'g':
					if(strin[2] == 'p')
					{
						strcat(cmnd," -p");
					}
					else if(strin[2] == 'i')
					{
						strcat(cmnd," -i");
					}
					gen_flag = 1;
					break;
				case 'c':
					if(!strcmp(&strin[1],"cpp"))
					{
						strcat(cmnd," -cpp");
					}
					break;
				case 'C':
					strcat(cmnd," -cpp");
					break;
				case 'm':
				case 'M':
					strcat(cmnd," -m");
					break;
			}
		}
		else
			strcpy(path,strin);
	}
	for(i = 0; i < strlen(path); i++)
	{
		path[i] = tolower(path[i]);
		if((path[i] == '/') || (path[i] == ']'))
			index = i+1;
	}	
	strcpy(ext,".sml");
	strcpy(in_file, &path[index]);
	path[index] = '\0';
	if(ptr = (char *)strchr(in_file,'.'))
	{
		strcpy(ext,ptr);
		*ptr = '\0';
	}
	strcat(cmnd," ");
	strcat(cmnd,path);
	strcat(cmnd,in_file);
	strcat(cmnd,ext);
/*
	printf("Executing : %s\n",cmnd);
*/
#ifdef unix
	sprintf(cmnd_path,"%s/generator/%s/%s",smidir,xdir,cmnd);
	ret = system(cmnd_path);
#else
	ret = system(cmnd);
#endif
	if(ret > 1)
		exit(0);
	printf("Creating : %s%s.sobj\n",path,in_file);
	strcpy(cmnd,"smiPreproc ");
	strcat(cmnd,path);
	strcat(cmnd,in_file);
	strcat(cmnd,ext);
	strcat(cmnd," ");
	strcat(cmnd,path);
	strcat(cmnd,in_file);
	strcat(cmnd,".smi");
/*
	printf("Executing : %s\n",cmnd);
*/
#ifdef unix
	sprintf(cmnd_path,"%s/preprocessor/%s/%s",smidir,xdir,cmnd);
	ret = system(cmnd_path);
#else
	ret = system(cmnd);
#endif
#ifdef unix
	strcpy(cmnd,"smiTransf ");
#else
	strcpy(cmnd,"smiTrans ");
#endif
	strcat(cmnd,path);
	strcat(cmnd,in_file);
	strcat(cmnd,".smi");
/*
	printf("Executing : %s\n",cmnd);
*/
#ifdef unix
	sprintf(cmnd_path,"rm %s%s.sobj",path,in_file);
	ret = system(cmnd_path);
#endif
#ifdef unix
	sprintf(cmnd_path,"%s/ftranslator/%s/%s",smidir,xdir,cmnd);
	ret = system(cmnd_path);
#else
	ret = system(cmnd);
#endif
	if(strcmp(ext,".smi"))
	{
#ifdef unix
		sprintf(cmnd_path,"rm %s%s.smi",path,in_file);
#else
		sprintf(cmnd_path,"del %s%s.smi",path,in_file);
#endif
		ret = system(cmnd_path);
	}
}

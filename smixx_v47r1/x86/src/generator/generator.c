#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <smirtl.h>
#include <dim.h>

  /*
#ifndef SMI_PAR_TYPES
typedef enum { STRING, INTEGER, FLOAT } PAR_TYPES;
#define SMI_PAR_TYPES
#endif
*/

_PROTO( static int cmp_token,	(char *token, char *pattern) );
_PROTO( static int next_token,	(char *token, int flag) );
/*_PROTO( static int push_token, (TOKEN_STR *ptr) ); */
/*_PROTO( static int pop_token,  (TOKEN_STR *ptr) ); */
_PROTO( static int cmp_token,  (char *token, char *str) );
/*_PROTO( static int first_token,(TOKEN_STR *ptr) ); */

FILE	*finptr;
FILE	*foutptr;

typedef struct param{
	struct PARAM *next;
	char *name;
	int type;
}PARAM;

PARAM *Param_head = (PARAM *)0;	

typedef struct action{
	struct action *next;
	char *name;
	PARAM *param_head;	
}ACTION;

ACTION *Action_head = (ACTION *)0;	

typedef struct state{
	struct state *next;
	char *name;
	ACTION *action_head;	
}STATE;

STATE *State_head = (STATE *)0;	
STATE *Curr_state = (STATE *)0;

typedef struct proxy{
	struct state *next;
	char *name;
}PROXY;

PROXY *Proxy_head = (PROXY *)0;	

typedef struct smiobj{
	struct state *next;
	char *name;
}SMIOBJ;

SMIOBJ *Smiobj_head = (SMIOBJ *)0;	

int Sub_object_index;

char buffer[80];
int Line = 0;

typedef struct {
	char name[33];
	int n_states;
}SUBO;

char Curr_object[64];
char Domain_name[64];
SUBO Sub_objects[50];
int Generating = 0;
int Cpp_flag = 1;
int Make_flag = 0;
int Inc_flag = 0;
int Proxy_flag = 0;


int handle_object()
{
char *token=buffer;
STATE *statep;
ACTION *actionp;
PARAM *paramp;
/*
SMIOBJ *smiobjp;
*/
	while ( (statep = (STATE *) sll_remove_head(State_head)) )
	{
		free(statep->name);
		if(statep->action_head)
		{
			while ( (actionp = (ACTION *) sll_remove_head(statep->action_head)) )
			{
				free(actionp->name);
				free(actionp);
			}
		}	
		free(statep);
	}
	while ( (actionp = (ACTION *) sll_remove_head(Action_head)) )
	{
		free(actionp->name);
		if(actionp->param_head)
		{
			while ( (paramp = (PARAM *) sll_remove_head(actionp->param_head)) )
			{
				free(paramp->name);
				free(paramp);
			}
		}	
		free(actionp);
	}
	while ( (paramp = (PARAM *) sll_remove_head(Param_head)) )
	{
		free(paramp->name);
		free(paramp);
	}

	next_token(token,1);
	if(!cmp_token(token,":"))
	{
		return(0);
	}
	next_token(token,0);
	if(strstr(token,"::"))
		return(0);
	strcpy(Curr_object,token);
	next_token(token,0);
	if(!cmp_token(token,"/"))
	{
/*
		smiobjp = (SMIOBJ *)malloc(sizeof(SMIOBJ));
		smiobjp->name = (char *)malloc(strlen(Curr_object)+1);
		strcpy(smiobjp->name,Curr_object);
		sll_insert_queue( Smiobj_head, smiobjp );
*/
		next_token(token,-1);
		return(2);
	}
	next_token(token,0);
	if(!cmp_token(token,"ASSOCIATED"))
	{
		return(0);
	}

	Sub_object_index = 0;
	return(1);
}

int handle_state()
{
char *token=buffer;
STATE *statep;

	next_token(token,1);
	if(!cmp_token(token,":"))
	{
		return(0);
	}
	next_token(token,0);

	statep = (STATE *)malloc(sizeof(STATE));
	statep->name = (char *)malloc(strlen(token)+1);
	strcpy(statep->name,token);

	next_token(token,0);
	if(cmp_token(token,"/"))
	{	
		next_token(token,0);
		if(cmp_token(token,"DEAD_STATE"))
		{
			free(statep->name);
			free(statep);
			return(1);
		}
	}
	next_token(token,-1);
	sll_insert_queue( State_head, statep );
	statep->action_head = (ACTION *)malloc(sizeof(ACTION));
	sll_init( statep->action_head );
	Curr_state = statep;
	if(Sub_object_index)
		Sub_objects[Sub_object_index - 1].n_states++;		
	return(1);
}

int handle_sub_object()
{
char *token=buffer;

	next_token(token,1);
	if(!cmp_token(token,":"))
	{
		return(0);
	}
	next_token(token,0);
	strcpy(Sub_objects[Sub_object_index++].name,token);
	Sub_objects[Sub_object_index-1].n_states = 0;
	return(1);
}

int handle_action()
{
char *token=buffer;
int insert = 1;
ACTION *actionp;
PARAM *paramp;

	next_token(token,1);
	if(!cmp_token(token,":"))
	{
		return(0);
	}
	next_token(token,0);
	actionp = (ACTION *)malloc(sizeof(ACTION));
	actionp->name = (char *)malloc(strlen(token)+1);
	strcpy(actionp->name,token);

	actionp->param_head = (PARAM *)0;
	sll_insert_queue(Curr_state->action_head, actionp);

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		if(!strcmp(actionp->name,token))
			insert = 0;
	}
	actionp = (ACTION *)malloc(sizeof(ACTION));
	actionp->name = (char *)malloc(strlen(token)+1);
	strcpy(actionp->name,token);

	actionp->param_head = (PARAM *)0;

	if(insert)
		sll_insert_queue( Action_head, actionp );

	next_token(token,0);
/* handle action parameters */
	if(cmp_token(token,"("))
	{	
		actionp->param_head = (PARAM *)malloc(sizeof(PARAM));
		sll_init( actionp->param_head );
		do
		{
			next_token(token,0);
			paramp = (PARAM *)malloc(sizeof(PARAM));
			if(cmp_token(token,"INT"))
			{
				next_token(token,0);
				paramp->name = (char *)malloc(strlen(token)+1);
				strcpy(paramp->name,token);
				paramp->type = SMI_INTEGER;
			}
			else if(cmp_token(token,"FLOAT"))
			{
				next_token(token,0);
				paramp->name = (char *)malloc(strlen(token)+1);
				strcpy(paramp->name,token);
				paramp->type = SMI_FLOAT;
			}
			else if(cmp_token(token,"STRING"))
			{
				next_token(token,0);
				paramp->name = (char *)malloc(strlen(token)+1);
				strcpy(paramp->name,token);
				paramp->type = SMI_STRING;
			}
			else
			{
				paramp->name = (char *)malloc(strlen(token)+1);
				strcpy(paramp->name,token);
				paramp->type = SMI_STRING;
			}
			sll_insert_queue( actionp->param_head, paramp );
			next_token(token,0);
		}while(cmp_token(token,","));
	}
	next_token(token,-1);
	return(1);
}

int handle_param()
{
char *token=buffer;
PARAM *paramp;

	next_token(token,1);
	if(!cmp_token(token,":"))
	{
		return(0);
	}
	do
	{
		next_token(token,0);
		paramp = (PARAM *)malloc(sizeof(PARAM));
		if(cmp_token(token,"INT"))
		{
			next_token(token,0);
			paramp->name = (char *)malloc(strlen(token)+1);
			strcpy(paramp->name,token);
			paramp->type = SMI_INTEGER;
		}
		else if(cmp_token(token,"FLOAT"))
		{
			next_token(token,0);
			paramp->name = (char *)malloc(strlen(token)+1);
			strcpy(paramp->name,token);
			paramp->type = SMI_FLOAT;
		}
		else if(cmp_token(token,"STRING"))
		{
			next_token(token,0);
			paramp->name = (char *)malloc(strlen(token)+1);
			strcpy(paramp->name,token);
			paramp->type = SMI_STRING;
		}
		else
		{
			paramp->name = (char *)malloc(strlen(token)+1);
			strcpy(paramp->name,token);
			paramp->type = SMI_STRING;
		}
		sll_insert_queue( Param_head, paramp );
		next_token(token,0);
	}while(cmp_token(token,","));
	next_token(token,-1);
	return(1);
}

int gen_smiobj()
{
	char aux[64],file_name[128];
	int i;
	STATE *statep;
	ACTION *actionp;

	if(!Inc_flag)
		return(0);
/*
	for(i = 0; i < strlen(Domain_name); i++)
	{
		aux[i] = tolower(Domain_name[i]);
	}	
	aux[i] = '\0';
	strcpy(file_name,aux);
*/
	for(i = 0; i < (int)strlen(Curr_object); i++)
	{
		aux[i] = tolower(Curr_object[i]);
	}	
	aux[i] = '\0';
/*
	if(strcmp(file_name,aux))
	{
		strcat(file_name,"_");
		strcat(file_name,aux);
	}
*/
	strcpy(file_name,aux);
	strcat(file_name,".inc");
	if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
	{
		printf("Cannot open: %s for writing\n",file_name);
		exit(0);
	}									 
	printf("Generating : %s\n",file_name);
	fprintf(foutptr,"object: %s::%s /associated\n",
		Domain_name, Curr_object);
	statep = State_head;
	while ( ( statep = (STATE *) sll_get_next( statep) ) ) 
	{
		fprintf(foutptr,"\tstate: %s\n", statep->name);
		actionp = statep->action_head;
		while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
		{
			fprintf(foutptr,"\t\taction: %s\n", actionp->name);
		}
	}
	fclose(foutptr);
	return(1);
}

int gen_ccfile(str)
char *str;
{

	char file_name[64];
	STATE *statep;
	ACTION *actionp;
	PARAM *paramp;
	int first_time = 1;

	strcpy(file_name,str);
	strcat(file_name,"_skel.cxx");
	if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
	{
		printf("Cannot open: %s for writing\n",file_name);
		exit(0);
	}									 
	printf("Generating : %s\n",file_name);
	fprintf(foutptr,"#include \"%s.hxx\"\n\n", str);

	fprintf(foutptr,"class %s: public %sProxy\n{\nprotected :\n",
		Curr_object, Curr_object);
	fprintf(foutptr,"// Actions\n");

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) )
	{
		fprintf(foutptr,"\tvirtual void %s();\n", actionp->name);
	}
	
	fprintf(foutptr,"public:\n");

	fprintf(foutptr,"// Constructors\n");
	fprintf(foutptr,"\t%s(char *domain): %sProxy(domain) { };\n",
		Curr_object, Curr_object);
	fprintf(foutptr,"};\n\n");	

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		fprintf(foutptr,"void %s::%s()\n{\n",Curr_object, actionp->name);
		fprintf(foutptr,"//\tUser code for action %s\n", actionp->name);
		first_time = 1;
		statep = State_head;
		while ( ( statep = (STATE *) sll_get_next( statep) ) ) 
		{
			if(first_time)
			{
				fprintf(foutptr,"//\tset%s()", statep->name);
				first_time = 0;
			}
			else
				fprintf(foutptr,"/ set%s()", statep->name);
		}
		fprintf(foutptr,";\n}\n\n");
	}

	fprintf(foutptr,"int main(int argc, char *argv[])\n{\n\n");
	fprintf(foutptr,"\t%s %s(argv[1]);\n",Curr_object, str);

	paramp = Param_head;
	while ( ( paramp = (PARAM *) sll_get_next( paramp) ) ) 
	{
		fprintf(foutptr,"//\t%s.setPar%s(value);\n", 
			str, paramp->name);
	}

	statep = (STATE *)sll_get_head(State_head);
	fprintf(foutptr,"//\t%s.set%s();\n",str, statep->name);
	fprintf(foutptr,"\twhile(1)\n\t{\n\t\tpause();\n\t}\n}\n\n");

	fclose(foutptr);
	return(1);
}

int gen_ccfile_inherit(str)
char *str;
{

	char file_name[64];
	STATE *statep;
	ACTION *actionp;
	PARAM *paramp;

	strcpy(file_name,str);
	strcat(file_name,"_inherit_skel.cxx");
	if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
	{
		printf("Cannot open: %s for writing\n",file_name);
		exit(0);
	}									 
	printf("Generating : %s\n",file_name);
	fprintf(foutptr,"#define SMI_INHERIT\n");
/*	fprintf(foutptr,"#define SMI_INHERIT\n", str); */

	fprintf(foutptr,"#include \"%s.hxx\"\n\n", str);

	fprintf(foutptr,"class Proxy: public %s\n{\n", Curr_object);
	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) )
	{
		fprintf(foutptr,"\tvoid %s();\n", actionp->name);
	}
	fprintf(foutptr,"public:\n");
	fprintf(foutptr,"/* Constructors */\n");
	fprintf(foutptr,"\tProxy(char *domain): %s(domain) {};\n",
		Curr_object);
	fprintf(foutptr,"};\n\n");	

	fprintf(foutptr,"int main(int argc, char *argv[])\n{\n\n");
	fprintf(foutptr,"\tProxy %s(argv[1]);\n",str);

	paramp = Param_head;
	while ( ( paramp = (PARAM *) sll_get_next( paramp) ) ) 
	{
		fprintf(foutptr,"/*\t%s.setPar%s(value); */\n", str, paramp->name);
	}

	statep = (STATE *) sll_get_head(State_head);
	fprintf(foutptr,"/*\t%s.set%s();*/\n",str, statep->name);
	fprintf(foutptr,"\twhile(1)\n\t{\n\t\tpause();\n\t}\n}\n\n");

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		fprintf(foutptr,"/* Action %s User code */\n", actionp->name);
		fprintf(foutptr,"void Proxy::%s()\n{\n/*\tsetXXX();*/\n}\n\n", 
			actionp->name);
	}
	fclose(foutptr);
	return (1);
}

int gen_makefile()
{
	char file_name[64];
	PROXY *proxyp;
	int first_time;
	
	first_time = 1;

#ifdef vms

	if(Proxy_head)
	{
		strcpy(file_name,"descrip.mms");
		if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
		{
			printf("Cannot open: %s for writing\n",file_name);
			exit(0);
		}									 
		printf("Generating : %s\n",file_name);

		fprintf(foutptr,"!\n! DESCRIP.MMS\n!\n");
		fprintf(foutptr,"!\tThis command file builds the Proxies for %s\n",
			Domain_name);                                                       
		fprintf(foutptr,"!\n!\tdefine main targets\n!\n");	
		fprintf(foutptr,"SDIR = []\nRDIR = [.$(node_arch)]\nTDIR = $(RDIR)\n");
		fprintf(foutptr,"ODIR = $(RDIR)\n\n");
		fprintf(foutptr,"dim_dir = delphi$online:[communications.dim]\n");
		fprintf(foutptr,"smi_dir = delphi$online:[smixx.rtl]\n\n");
								
		fprintf(foutptr,"cflags = /debug/noopt/include=($(dim_dir),$(smi_dir))\n\n");

		fprintf(foutptr,".include $(dim_dir)rules.mms\n\nmain_targets = ");

		proxyp = Proxy_head;
		while( proxyp = (PROXY *) sll_get_next( proxyp) ) 
		{
			if(first_time)
			{
				fprintf(foutptr,"%s", proxyp->name);
				first_time = 0;
			}
			else
				fprintf(foutptr,", %s", proxyp->name);
		}

		fprintf(foutptr,"\n!\n!\tdefine dependency rules\n!\n");
		fprintf(foutptr,"* :\t$(main_targets)\n\tcontinue\n!\n");
		fprintf(foutptr,"!\tbuild the executables and libraries\n!\n\n");

		if(Cpp_flag)
		{
			proxyp = Proxy_head;
			while( proxyp = (PROXY *) sll_get_next( proxyp) ) 
			{
				fprintf(foutptr,"$(ODIR)%s.obj : $(SDIR)%s.cxx\n\n",
					proxyp->name, proxyp->name);

				fprintf(foutptr,"%s : $(RDIR)%s.obj\n",
					proxyp->name, proxyp->name);
				fprintf(foutptr,
"\t$link/exe=$(ODIR)%s $(RDIR)%s,$(smi_dir)smirtl/opt,$(dim_dir)dim/opt\n\n",
					proxyp->name, proxyp->name);
			}
		}
		else
		{
			proxyp = Proxy_head;
			while( proxyp = (PROXY *) sll_get_next( proxyp) ) 
			{
				fprintf(foutptr,"$(ODIR)%s.obj : $(SDIR)%s.c\n\n",
					proxyp->name, proxyp->name);

				fprintf(foutptr,"%s : $(RDIR)%s.obj\n",
					proxyp->name, proxyp->name);
				fprintf(foutptr,
"\t$link/exe=$(ODIR)%s $(RDIR)%s,$(smi_dir)smirtl/opt,$(dim_dir)dim/opt\n\n",
					proxyp->name, proxyp->name);
			}
		}					
		fclose(foutptr);
	}
#else

	if(Proxy_head)
	{
		strcpy(file_name,"Makefile");
		if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
		{
			printf("Cannot open: %s for writing\n",file_name);
			exit(0);
		}									 
		printf("Generating : %s\n",file_name);

		fprintf(foutptr,"#\n# Makefile: builds the Proxies for %s\n#\n\n",
			Domain_name);                                                       
		fprintf(foutptr,"CC = cc\nCXX = CC\nMAKE = gmake\n");

		fprintf(foutptr,"CFLAGS = $(FLAGS) -g -I. -I$(DIMDIR)/dim -I$(SMIDIR)/smixx\n");
		fprintf(foutptr,"EXTRALIBS =\nODIR =\n\n");

		fprintf(foutptr,"ifeq ($(OS),SunOS)\n\tODIR=sunos\nendif\n");
		fprintf(foutptr,"ifeq ($(OS),Solaris)\n\tEXTRALIBS = -lpthread -lposix4 -lsocket -lnsl");
		fprintf(foutptr,"\n\tFLAGS += -mt");
		fprintf(foutptr,"\n\tODIR = solaris\nendif\n");
		fprintf(foutptr,"ifeq ($(OS),HP-UX)\n\tODIR = hp\nendif\n");
		fprintf(foutptr,"ifeq ($(OS),OSF1)\n\tODIR = osf\nendif\n");
		fprintf(foutptr,"ifeq ($(OS),AIX)\n\tODIR = aix\nendif\n");
		fprintf(foutptr,"ifeq ($(OS),Linux)\n\tEXTRALIBS = -lpthread");
		fprintf(foutptr,"\n\tCXX = g++\n\tODIR = linux\nendif\n");
		fprintf(foutptr,"all:");
		proxyp = Proxy_head;
		while ( ( proxyp = (PROXY *) sll_get_next( proxyp) ) )
		{
			fprintf(foutptr,"\t$(ODIR)/%s", proxyp->name);
		}
		fprintf(foutptr,"\n\n");

		if(Cpp_flag)
		{
			proxyp = Proxy_head;
			while ( ( proxyp = (PROXY *) sll_get_next( proxyp) ) ) 
			{
				fprintf(foutptr,"$(ODIR)/%s : %s.cxx\n",
					proxyp->name, proxyp->name);

				fprintf(foutptr,
"\t$(CXX) $(CFLAGS) %s.cxx -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/%s\n",
					proxyp->name, proxyp->name);
			}
		}
		else
		{
			proxyp = Proxy_head;
			while ( ( proxyp = (PROXY *) sll_get_next( proxyp) ) )
			{
				fprintf(foutptr,"$(ODIR)/%s : %s.c\n",
					proxyp->name, proxyp->name);

				fprintf(foutptr,
"\t$(CC) $(CFLAGS) %s.c -L$(SMIDIR)/$(ODIR) -lsmi -L$(DIMDIR)/$(ODIR) -ldim $(EXTRALIBS) -o $(ODIR)/%s\n",
					proxyp->name, proxyp->name);
			}
		}
		fclose(foutptr);
	}
#endif
	return(1);
}

int handle_comment()
{
char *token=buffer;

	next_token(token,2);
	return(1);
}

int cmp_token(token,pattern)
char *token, *pattern;
{
	return(!(strcmp(token,pattern)));
}

int next_token(token,flg)
char *token;
int flg;
{
char ch, str[132];
int filling = 0;
static char next_ch = 0;
#ifdef VMS
static fpos_t f_pos;
#else
static int f_pos;
#endif

	if(next_ch)
	{
		*token++ = next_ch;
		*token = '\0';
		next_ch = 0;
		return(1);
	}
	if(flg == -1)
	{
#ifdef VMS
		fsetpos(finptr,&f_pos);
#else
		fseek(finptr, f_pos, 0);
#endif
		return(1);
    }
#ifdef VMS
	fgetpos(finptr,&f_pos);
#else
	f_pos = ftell(finptr);
#endif
	while(1)
	{
		ch = fgetc(finptr);
		if(feof(finptr))
			return(0);
		if((ch == ' ')||(ch == '\t'))
		{
			if(filling)
				break;
		}
		else if(ch == '\n')
		{
				Line++;
			if(filling)
				break;
		}
		else if( (ch == ',')||(ch == '=')||(ch == '{')||(ch == '}')||
			(ch == ';')||(ch == '/')||(ch == '(')||(ch == ')')||
			(ch == '\"'))
		{
			if(filling)
			{
             	next_ch = ch;
				break;
			}
			else
			{
				*token++ = ch;
				break;
			}
		}
		else if(ch == ':')
		{
			if(flg == 1)
			{
				if(filling)
				{
             		next_ch = ch;
					break;
				}
				else
				{
					*token++ = ch;
					break;
				}
			}
			else
				*token++ = ch;
		}
		else if(ch == '!')
		{
			fscanf(finptr,"%[^\n]%c",str,&ch);
				Line++;
			if(filling)
				break;
		}
		else
		{
			filling = 1;
			*token++ = toupper(ch);
		}
	}
	*token = '\0';
	return(1);
}

int gen_cpp()
{
	char file_name[64];
	char str[256];
	STATE *statep;
	ACTION *actionp;
	PARAM *paramp;
	PROXY *proxyp;
	int i;
	int first_time = 0;

	for(i = 0; i < (int)strlen(Curr_object); i++)
	{
		file_name[i] = tolower(Curr_object[i]);
	}	
	file_name[i] = '\0';
	strcpy(str,file_name);

	if(!Proxy_head)
	{
		Proxy_head = (PROXY *)malloc(sizeof(PROXY));
		sll_init( Proxy_head );
	}
	proxyp = (PROXY *)malloc(sizeof(PROXY));
	proxyp->name = (char *)malloc(strlen(file_name)+1);
	strcpy(proxyp->name,file_name);
	sll_insert_queue( Proxy_head, proxyp );

	strcat(file_name,".hxx");
	if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
	{
		printf("Cannot open: %s for writing\n",file_name);
		exit(0);
	}									 
	printf("Generating : %s\n",file_name);
	fprintf(foutptr,"#include <smirtl.hxx>\n\n");

	fprintf(foutptr,"class %sProxy: public SmiProxy\n{\nprotected :\n",
		Curr_object);
	fprintf(foutptr,"// Actions\n");

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		fprintf(foutptr,"\tvirtual void %s() {};\n", actionp->name);
	}
	
	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		if(actionp->param_head)
		{
			fprintf(foutptr,"// Parameters of action %s\n", actionp->name);
			paramp = actionp->param_head;
			while ( ( paramp = (PARAM *) sll_get_next( paramp) ) ) 
			{
				switch(paramp->type)
				{
					case SMI_INTEGER:
						fprintf(foutptr,
					"\tint getPar%s() {return getParameterInt(\"%s\");};\n", 
							paramp->name, paramp->name);
						break;
					case SMI_FLOAT:
						fprintf(foutptr,
					"\tfloat getPar%s() {return getParameterFloat(\"%s\");};\n", 
							paramp->name, paramp->name);
						break;
					case SMI_STRING:
						fprintf(foutptr,
					"\tchar *getPar%s() {return getParameterString(\"%s\");};\n", 
							paramp->name, paramp->name);
						break;
				}
			}
		}
	}

	fprintf(foutptr,"// Command Handler\n");
	fprintf(foutptr,"\tvoid smiCommandHandler();\n");
	fprintf(foutptr,"public:\n");

	fprintf(foutptr,"// States\n");
	statep = State_head;
	while ( ( statep = (STATE *) sll_get_next( statep) ) ) 
	{
		fprintf(foutptr,"\tvoid set%s() {setState(\"%s\");};\n", 
			statep->name, statep->name);
	}
	
	fprintf(foutptr,"// Object Parameters\n");

	paramp = Param_head;
	while ( ( paramp = (PARAM *) sll_get_next( paramp) ) ) 
	{
		switch(paramp->type)
		{
			case SMI_INTEGER:
				fprintf(foutptr,
					"\tint setPar%s(int val) {return setParameter(\"%s\", val);};\n", 
					paramp->name, paramp->name);
				break;
			case SMI_FLOAT:
				fprintf(foutptr,
					"\tint setPar%s(double val) {return setParameter(\"%s\", val);};\n", 
					paramp->name, paramp->name);
				break;
			case SMI_STRING:
				fprintf(foutptr,
					"\tint setPar%s(char *val) {return setParameter(\"%s\", val);};\n", 
					paramp->name, paramp->name);
				break;
		}
	}
	fprintf(foutptr,"// Constructors\n");
	fprintf(foutptr,"\t%sProxy(char *domain): SmiProxy(\"%s\") { attach(domain);};\n",
		Curr_object, Curr_object);
	fprintf(foutptr,"};\n\n");	

	fprintf(foutptr,"void %sProxy::smiCommandHandler()\n{\n", Curr_object);	

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) ) 
	{
		if(!first_time)
			fprintf(foutptr,"\tif(testAction(\"%s\"))\n",actionp->name);
		else
			fprintf(foutptr,"\telse if(testAction(\"%s\"))\n",actionp->name);
		fprintf(foutptr,"\t\t%s();\n", actionp->name);
		first_time = 1;
	}
	fprintf(foutptr,"}\n");	

	fclose(foutptr);
/*
	gen_ccfile_inherit(str);
*/
	gen_ccfile(str);
	return(1);
}

int gen_proxy()
{
	char file_name[64];
	char str[256];
	int first_time = 0;
	int i;
	STATE *statep;
	ACTION *actionp;
	PARAM *paramp;
	PROXY *proxyp;

	if(!Proxy_flag)
		return(1);
	if(Cpp_flag)
	{
		gen_cpp();
		return(1);
	}
	for(i = 0; i < (int)strlen(Curr_object); i++)
	{
		file_name[i] = tolower(Curr_object[i]);
	}	
	file_name[i] = '\0';

	if(!Proxy_head)
	{
		Proxy_head = (PROXY *)malloc(sizeof(PROXY));
		sll_init( Proxy_head );
	}
	proxyp = (PROXY *)malloc(sizeof(PROXY));
	proxyp->name = (char *)malloc(strlen(file_name)+1);
	strcpy(proxyp->name,file_name);
	sll_insert_queue( Proxy_head, proxyp );

	strcat(file_name,"_skel.c");
	if( ( foutptr = fopen( file_name, "w" ) ) == (FILE *)0 )
	{
		printf("Cannot open: %s for writing\n",file_name);
		exit(0);
	}									 
	printf("Generating : %s\n",file_name);
	fprintf(foutptr,
		"#include <smirtl.h>\n\n");

	fprintf(foutptr,"/* States */\n");
	statep = State_head;
	while ( ( statep = (STATE *) sll_get_next( statep) ) ) 
	{
		fprintf(foutptr,"void set%s() {smi_set_state(\"%s\");};\n", 
			statep->name, statep->name);
	}
	
	fprintf(foutptr,"\n/* Object Parameters */\n");

	paramp = Param_head;
	while ( ( paramp = (PARAM *) sll_get_next( paramp) ) )
	{
		switch(paramp->type)
		{
			case SMI_INTEGER:
				fprintf(foutptr,
		"int setPar%s(int val) {return smi_set_par(\"%s\", &val, INTEGER);};\n", 
					paramp->name, paramp->name);
				break;
			case SMI_FLOAT:
				fprintf(foutptr,
		"int setPar%s(double val) {return smi_set_par(\"%s\", &val, FLOAT);};\n", 
					paramp->name, paramp->name);
				break;
			case SMI_STRING:
				fprintf(foutptr,
		"int setPar%s(char *val) {return smi_set_par(\"%s\", val, STRING);};\n", 
					paramp->name, paramp->name);
				break;
		}
	}

	fprintf(foutptr,"\n/* Command Handler */\n");

	fprintf(foutptr,"void handle_command()\n{\n");

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) )
	{
		fprintf(foutptr,"\tvoid %s();\n", actionp->name);
	}

	actionp = Action_head;
	while ( ( actionp = (ACTION *) sll_get_next( actionp) ) )
	{
		if(!first_time)
			fprintf(foutptr,"\n\tif(smi_test_action(\"%s\"))\n",actionp->name);
		else
			fprintf(foutptr,"\telse if(smi_test_action(\"%s\"))\n",
				actionp->name);
		fprintf(foutptr,"\t{\n\t\t%s();\n\t}\n",actionp->name);
		first_time = 1;
	}
	fprintf(foutptr,"}\n");	

	fprintf(foutptr,"\n/* Main Program */\n");

	fprintf(foutptr,"int main(argc, argv)\nint argc;\nchar **argv[];\n{\n\n");

	fprintf(foutptr,"\tchar proxy_name[64];\n");
	fprintf(foutptr,"\tvoid handle_command();\n");

	sprintf(str,"sprintf(proxy_name,\"%%s::%s\",argv[1]);",Curr_object);

	fprintf(foutptr,"\t%s\n\tsmi_attach(proxy_name, handle_command);\n",str);

	statep = State_head;
	paramp = Param_head;
	while ( ( paramp = (PARAM *) sll_get_next( paramp) ) )
	{
		fprintf(foutptr,"/*\tsetPar%s(value); */\n", paramp->name);
	}
	statep = (STATE *)sll_get_head(State_head);
	fprintf(foutptr,"/*\tset%s() */\n", statep->name);

	fprintf(foutptr,"\n\twhile(1)\n\t{\n\t\tpause();\n\t}\n}\n\n");

	actionp = Action_head;
	while (( actionp = (ACTION *) sll_get_next( actionp) ) )
	{

		if(actionp->param_head)
		{
			fprintf(foutptr,"/* Parameters of action %s */\n", actionp->name);
			paramp = actionp->param_head;
			while ( ( paramp = (PARAM *) sll_get_next( paramp) ) )
			{
				switch(paramp->type)
				{
					case SMI_INTEGER:
						fprintf(foutptr,
	"int getPar%s(int *val) { return smi_get_par_value(\"%s\",val);};\n", 
							paramp->name, paramp->name);
						break;
					case SMI_FLOAT:
						fprintf(foutptr,
	"int getPar%s(float *val) { return smi_get_par_value(\"%s\",val);};\n", 
							paramp->name, paramp->name);
						break;
					case SMI_STRING:
						fprintf(foutptr,
	"int getPar%s(char *val) { return smi_get_par_value(\"%s\",val);};\n", 
							paramp->name, paramp->name);
						break;
				}
			}
		}

		fprintf(foutptr,"\n/* Action %s User code */\n", actionp->name);
		fprintf(foutptr,"void %s()\n{\n/*\tsetXXX();*/\n}\n\n", 
			actionp->name);
	}
	fclose(foutptr);
	return(0);
}

int main(argc,argv)
int argc;
char **argv;										
{
char *token=buffer, *ptr;
int i, index = 0;
char strin[80];
char path[128];
char in_file[80];
char ext[16];

	if(argc < 2)
	{
		printf("\nsmiGen [options] file\n\n");
		printf("DESCRIPTION:\n");
		printf("\tsmiGen Generates Proxy skelettons from the SML code \".SMI\"\n");
		printf("\tand include files containing SMI object headers\n");
		printf("\tPossible Options:\n");
		printf("\t\t-p\tGenerate Proxies\n");
		printf("\t\t-c\tGenerate Proxies in \"C\"\n");
		printf("\t\t-C\tGenerate Proxies in \"C++\" (default)\n");
		printf("\t\t-m\tGenerate a makefile for the proxies\n");
		printf("\t\t-i\tGenerate include files for SMI objects\n");
		exit(0);
	}
	for(i = 1; i < argc; i++)
	{
		sprintf(strin,argv[i]);
		if(strin[0] == '-')
		{
			switch(strin[1])
			{
				case 'p':
					Proxy_flag = 1;
					break;
				case 'c':
					if(!strcmp(&strin[1],"cpp"))
						Cpp_flag = 1;
					else
						Cpp_flag = 0;
					break;
				case 'C':
					Cpp_flag = 1;
					break;
				case 'm':
				case 'M':
					Make_flag = 1;
					break;
				case 'i':
				case 'I':
					Inc_flag = 1;
					break;
			}
		}
		else
			strcpy(path,strin);
	}
	for(i = 0; i < (int)strlen(path); i++)
	{
		path[i] = tolower(path[i]);
		if((path[i] == '/') || (path[i] == ']'))
			index = i+1;
	}	
	strcpy(ext,".smi");
	strcpy(in_file, &path[index]);
	path[index] = '\0';
	if ( (ptr = (char *)strchr(in_file,'.')) )
	{
		strcpy(ext,ptr);
		*ptr = '\0';
	}
	strcpy(Domain_name,in_file);
	for(i = 0; i < (int)strlen(Domain_name); i++)
	{
		Domain_name[i] = toupper(Domain_name[i]);
	}	
	strcat(path,in_file);
	strcat(path,ext);
/*
	printf("Input : %s\n",path);
*/
	if( ( finptr = fopen( path, "r" ) ) == (FILE *)0 )
	{
		printf("%s : File not found\n",path);
		exit(2);
	}					

	State_head = (STATE *)malloc(sizeof(STATE));
	sll_init( State_head );
	Action_head = (ACTION *)malloc(sizeof(ACTION));
	sll_init( Action_head );
	Param_head = (PARAM *)malloc(sizeof(PARAM));
	sll_init( Param_head );
/*
	Smiobj_head = (SMIOBJ *)malloc(sizeof(SMIOBJ));
	sll_init( Smiobj_head );
*/
	Proxy_head = 0;
				 
	while(next_token(token,1))
	{
		switch(token[0])
		{
			case 'C' :
				if(cmp_token(token,"CLASS")) 
				{
					if(Generating == 1)
						gen_proxy();
					else if(Generating == 2)
						gen_smiobj();
					Generating = 0;
				}
				break;
			case 'O' :
				if(cmp_token(token,"OBJECT")) 
				{
					if(Generating == 1)
						gen_proxy();
					else if(Generating == 2)
						gen_smiobj();
					Generating = handle_object();
				}
				break;
			case 'S' :
				if(cmp_token(token,"STATE")) 
				{
					if(Generating)
						handle_state();
				}
				if(cmp_token(token,"SUBOBJECT")) 
				{
					if(Generating)
						handle_sub_object();
				}
				break;
			case 'A' :
				if(cmp_token(token,"ACTION")) 
				{
					if(Generating)
						handle_action();
				}
				break;
			case 'P' :
				if(cmp_token(token,"PARAMETERS")) 
				{
					if(Generating)
						handle_param();
				}
				break;
			default	: 
				break;
		}
	}
	if(Generating == 1)
		gen_proxy();
	else if(Generating == 2)
		gen_smiobj();
	fclose( finptr );
	if(Make_flag)
		gen_makefile();
	exit(0);
}

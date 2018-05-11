#include <smiuirtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "smixx_parstring_util.h"

/*=======================================================================================*/
void printString(str)
/*                             Boda Franek  May 2007 */
char* str;
{
	int i;
	char ch;
	printf("<");
	for ( i=0; ; i=i+1) {
		if ( *(str+i) == '\0' ) break;
		ch = *(str+i);
		if ( !isprint(ch) ) ch = '?';
		printf("%c",ch);
	}
	printf(">\n");
		
	for (i=0; ; i=i+1) {
		if (*(str+i) == '\0' ) break;
		printf(" %o",*(str+i));
	}
	printf("\n");

	return;
}
void convToUpper(str)
char* str;
{
	int i;
	
	for (i = 0; i < (int)strlen(str); i++) str[i] = toupper(str[i]);
	return;
}
/*=========================================================================================*/
void handle_parameter(void* idComm, char* parName, char* parVal, char type)
/*                             Boda Franek  May 2007 */
/*	idComm	identifier of the command to which parameter will be attached */
/*	parName	name of the parameter as picked up from the command line */
/*	parVal	parameter value as it is picked up from the command line. Deppending on the
		type of the parameter, this is translated accordigly */
/*	type	the type of the parameter. S I or F  */
{
	long lintvalue;
	double dfvalue;
	char* stringvalue;
	
	char *endptr;
	int nchar;
	char* tempString;


	/*printf("\n%s\n%c\n",parName,type);*/
	if ( type == 'I' )
	{
		lintvalue = strtol(parVal,&endptr,0);
		/*printf("errno %d endptr <%s> value %d\n",errno,endptr,lintvalue);*/
		if ( strcmp(endptr,"") || errno )
		{
			 printf(" Parameter %s has illegal value\n",parName);
			 exit (1);
		}
		if (!smiui_add_param_to_command(idComm,parName,(void*)&lintvalue,SMI_INTEGER)) exit (1);
	}
	else if ( type == 'F' )
	{
		dfvalue = strtod(parVal,&endptr);
		/*printf("errno %d endptr <%s> value %e\n",errno,endptr,dfvalue);*/
		if ( strcmp(endptr,"") || errno ) 
		{
			printf(" Parameter %s has llegal value\n",parName);
			exit (1);
		}
		if(!smiui_add_param_to_command(idComm,parName,(void*)&dfvalue,SMI_FLOAT)) exit (1);
	}
	else if ( type == 'S' )
	{
		/*printString(parVal); printf("\n");*/
		/* the string on the command line should be C-escaped ,
		 need only to escape smixx specials such as slash */
		nchar = smixx_escape_slash(NULL,parVal,0);
		if(nchar < 0) exit(1);
		tempString = malloc(nchar+1);
		nchar = smixx_escape_slash(tempString,parVal,nchar);
		
		/*printString(tempString); printf("\n");*/
		nchar = smixx_unescape(NULL,tempString,0);
	 	stringvalue = malloc(nchar+1);
		nchar = smixx_unescape(stringvalue,tempString,nchar);
		free(tempString);
		/*printString(stringvalue); printf("\n");*/
		if(!smiui_add_param_to_command(idComm,parName,stringvalue,SMI_STRING)) exit (1);
	}
	else
	{
		printf(" unknown type\n");
		exit(1);
	}

}
/*=========================================================================================*/
void print_help()
{
	char text[] = 
"Usage :\n\
(prompt) smi_send_command <objectName> <commandString> [options] [<proxy>]\n\n\
where \n\
	<commandString> is, in the simplest case, the name of the action\n\
			to be sent to the object.\n\
	<proxy>		can be any printable character (except '-') and \n\
			indicates that the command should be sent directly\n\
			to the proxy (bypassing any state manager)\n\
the following options are available:\n\
	-dns <dnsNode[:dnsPort]> \n\
			allows to change the DNS Node and Port number\n\n\
	-pi <parameterName> <integerValue>		or\n\
	-pf <parameterName> <floatingPointValue>	or\n\
	-ps <parameterName> <stringConstant>\n\
	will append integer/floatingpoint/string parameter\n\
	to the command string\n\n\
	-dbg		the command string,instead of being sent to the object,\n\
			is simply printed to standart output. Usefull for\n\
			debugging.";
	
	printf("%s\n",text);
	 
	return;
}
/*=========================================================================================*/
int main(argc,argv)
int argc;
char **argv;
{
int i;
char *obj = 0, *cmd = 0, *proxy = 0;
char *parName = 0,  *value = 0; char type;

char* ptrComm = 0;
void* idComm = 0;
char *dbg = 0;

char dns_node[128], *ptr;
int dns_port = 0;

	dns_node[0] = '\0';
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i],"-dns"))
		{
			strcpy(dns_node,argv[i+1]);
			if((ptr = strchr(dns_node,':')))
			{
				*ptr = '\0';
				ptr++;
				sscanf(ptr,"%d",&dns_port);
			}
			i++;
		}
		if(!strcmp(argv[i],"-dbg"))
		{
			dbg = argv[i];
		}
		else if ( !strcmp(argv[i],"-pi") || !strcmp(argv[i],"-pf") || !strcmp(argv[i],"-ps"))
		{
			if (!cmd) 
			{
				printf("smi_send_command: Action name has to come before the first parameter\n");
				print_help();
				exit(1);
			}
			if (!parName)
			{  /* this is the first parameter */
				idComm = smiui_create_command(cmd);
			}
			parName = argv[i+1];
			convToUpper(parName);
			/*printf("%s\n",argv[i]);*/
			type = toupper(argv[i][2]);
			value = argv[i+2];
			i = i + 2;
			/*printf("\n%s\n%c\n",parName,type);*/
			handle_parameter(idComm,parName,value,type);
		}
		else
		{
			if ( argv[i][0] == '-' ) 
			{
				printf("Unknown option %s \n",argv[i]); print_help(); exit(1);
			}
			if(!obj)
				obj = argv[i];
			else if(!cmd)
				cmd = argv[i];
			else
				proxy = argv[i];
		}
	}
	if(dns_node[0])
	{
		dic_set_dns_node(dns_node);
	}
	if(dns_port)
	{
		dic_set_dns_port(dns_port);
	}
	if((!obj) || (!cmd))
	{
		printf("smi_send_command: Insufficient number of arguments\n");
		print_help();
		exit(1);
	}
	
	if (idComm) 
	{
		ptrComm = smiui_get_command_string(idComm);
	}
	else 
	{
		ptrComm = cmd;
	}
	
	if(dbg)
	{
		printf(" Command : <%s>\n",ptrComm);
		exit(0);
	}
	if(proxy)
		smiui_ep_send_command_wait(obj, ptrComm);
	else
		smiui_send_command_wait(obj, ptrComm);
	usleep(500000);

	exit(0);
}


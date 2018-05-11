#define MAX_NAME 		132

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef VMS
#include <ssdef.h>   
#include <iodef.h>   
#include <lnmdef.h>  
#include <cfortran.h>
#endif
/*
#include "parameters.h"
*/
#define SMILIB
#include "version.h"
#include "smirtl.h"

#include "smixx_parstring_util.h"


typedef struct{
	char name[MAX_NAME];
	char state[MAX_NAME];
}SUB;

static SUB Subs[100];
static int N_sub_objects = 0;

typedef struct{
	char *name;
	char *value;
	int size;
	int type;
	char type_str[4];
	int esc_size;
	char *esc_value;
}PAR;

#define PAR_CHUNK 10

static PAR *Pars = 0;

static int N_parameters = 0;

typedef struct {
	char *token_buff;
	int size;
	char *token_ptr;
	char *curr_token_ptr;
}TOKEN_STR;

static TOKEN_STR SMI_command;

static char Domain_name[80];
static char EP_domain_name[80];
static char* SMI_state = 0;
static int SMI_state_size = 0;
static int SMI_state_id = 0;
static void (*SMI_ast)();
static int SMI_ef;
static char Object[MAX_NAME];
static char EP_object[MAX_NAME];
static int Cmnd_wake_up = -1;
static int Cmnd_arrived = 0;
static int Cmnd_valid = 0;

static int Initialized = 0;

static int Info_print = 1;
static int Version_number = SMIXX_VERSION;

typedef struct{
	int (*exit_handler)();
	void *context;
} EXIT_PARS;

static EXIT_PARS Exit_pars = {0,0};

_PROTO( static int init_token, (TOKEN_STR *ptr) );
_PROTO( static int get_token,  (TOKEN_STR *ptr, char **token) );
_PROTO( static int push_token, (TOKEN_STR *ptr) );
_PROTO( static int pop_token,  (TOKEN_STR *ptr) );
_PROTO( static int cmp_token,  (char *token, char *str) );
/*_PROTO( static int first_token,(TOKEN_STR *ptr) );*/

void smi_set_print_on()
{
	Info_print = 1;
}

void smi_set_print_off()
{
	Info_print = 0;
}

void smi_print_date_time()
{
	time_t t;
	char str[128];

	t = time((time_t *)0);
	printf("SMIRTL - ");
/*
#ifdef WIN32
	strcpy(str, ctime(&t));
#else
	ctime_r(&t, str);
#endif
*/
	my_ctime(&t, str, 128);
	str[strlen(str)-1] = '\0';
	printf("%s -",str);
}

/*int smi_init_for(user_ef, user_ast)
int user_ef;
void (*user_ast)();
{

	return(smi_init_(user_ef, (void (*)())user_ast));
}*/

int smi_init_(user_ef, user_ast)
int user_ef;
void (*user_ast)();
{

	if(user_ef)
	{
		SMI_ef = user_ef;
		SMI_ast = 0;
#ifdef VMS
		sys$clref(SMI_ef);
#endif
	}
	else
	{
		SMI_ef = 0;
		SMI_ast = user_ast;
	}
	return(1);
}

int smi_init_for(user_ef, user_ast)
int user_ef;
void (*user_ast)();
{

	return(smi_init_(user_ef, (void (*)())user_ast));
}

void get_obj_name(obj_name, serv_name, ep)
char *obj_name, *serv_name;
int ep;
{

	char obj[MAX_NAME];
	char *ptr_domain, *ptr_obj;

	strcpy(obj,obj_name);
	if ( (ptr_obj = (char *)strstr(obj,"::")) )
	{
		*ptr_obj = '\0';
		ptr_obj +=2;
		ptr_domain = obj;
	}
	else
	{
#ifdef VMS
		get_domain_name(Domain_name);
#endif
		ptr_domain = Domain_name;
		ptr_obj = obj;
	}
	strcpy(Object,ptr_obj);
	if(ep)
	{
		sprintf(serv_name,"EP/%s/%s", ptr_domain, ptr_obj);
		strcpy(EP_domain_name,ptr_domain);
		strcpy(EP_object,ptr_obj);
	}
	else
		sprintf(serv_name,"SMI/%s/%s", ptr_domain, ptr_obj);
	strcpy(Domain_name,ptr_domain);
}
	
static void capitalize(obj)
char *obj;
{
	while(*obj)
	{
		*obj = toupper(*obj);
		obj++;
	}
}
/*=======================================================================*/
void provide_buffer(ptr_tag,ptr_address,ptr_size)
/*                  Boda Franek   Aug 2007                */
long* ptr_tag;
int** ptr_address;
int* ptr_size;
{
	ptr_tag=ptr_tag;
	*ptr_address = (int*) SMI_state;
	*ptr_size = strlen(SMI_state) + 1;
	return;	
}
/*========================================================================*/
int smi_associate(obj_name)
char *obj_name;
{
	char serv_name[MAX_NAME], tmp_name[MAX_NAME];
	void execute_cmnd();
	int id;
	
	strcpy(tmp_name, obj_name);
	capitalize(tmp_name);
	SMI_command.size = 0;
	if(SMI_state_size < 6)
	{
		if(SMI_state_size)
			free(SMI_state);
		SMI_state = malloc(6);
		SMI_state_size = 6;
	}
/*
	if (SMI_state) free(SMI_state);
	SMI_state = malloc(6);
*/
	strcpy(SMI_state,"$DEAD");		
	get_obj_name(tmp_name,serv_name,1);
	id = dis_add_service(serv_name, "C",
		0, 0, provide_buffer, 0);
    if(id != 0)
		SMI_state_id = id;
	strcat(serv_name,"/CMD");
	dis_add_cmnd(serv_name, "C", execute_cmnd, 0);
	get_obj_name(tmp_name,serv_name,1);
	strcat(serv_name,"/SMI_VERSION_NUMBER");
	dis_add_service(serv_name, "L",
		&Version_number, 4, 0, 0);
	if(!strchr(tmp_name,':'))
		sprintf(serv_name,"%s_%s",EP_domain_name,tmp_name);
	else
		strcpy(serv_name,tmp_name);
	return(dis_start_serving(serv_name));
}	

void handle_exit(client_id)
int *client_id;
{
	int exit_code;

	client_id=client_id;
	smi_print_date_time();
	printf(" Smi Domain %s Exited - Proxy %s Exiting...\n",
	       EP_domain_name, EP_object);
	fflush(stdout);
	if(Exit_pars.exit_handler)
	{
		if(Exit_pars.exit_handler(Exit_pars.context, &exit_code))
			exit(exit_code);
	}
	else
		exit(0);
}

void smi_register_termination(exit_handler, context)
int (*exit_handler)();
void *context;
{
	Exit_pars.exit_handler = exit_handler;
	Exit_pars.context = context;
	dis_add_client_exit_handler(handle_exit);
}

int suicide_rout(context, exit_code)
int *exit_code;
void *context;
{
	context=context;
	*exit_code = 0;
	return(1);
}

void smi_volatile()
{
	smi_register_termination(suicide_rout, 0);
}	

/* smi_attach attaches a proxy to its associated object in SMI */
int smi_attach(obj_name, user_ast)
char *obj_name;
void (*user_ast)();
{

	char serv_name[MAX_NAME], tmp_name[MAX_NAME];
	void execute_cmnd();
	
	SMI_ef = 0;
	SMI_ast = user_ast;

	strcpy(tmp_name, obj_name);
	capitalize(tmp_name);
	SMI_command.size = 0;
	if(SMI_state_size < 6)
	{
		if(SMI_state_size)
			free(SMI_state);
		SMI_state = malloc(6);
		SMI_state_size = 6;
	}
/*
	if (SMI_state) free(SMI_state);
	SMI_state = malloc(6);
*/
	strcpy(SMI_state,"$DEAD");		
	get_obj_name(tmp_name,serv_name,1);
	SMI_state_id = dis_add_service(serv_name, "C",
		0, 0, provide_buffer, 0);

	strcat(serv_name,"/CMD");
	dis_add_cmnd(serv_name, "C", execute_cmnd, 0);
	get_obj_name(tmp_name,serv_name,1);
	strcat(serv_name,"/SMI_VERSION_NUMBER");
	dis_add_service(serv_name, "L",
		&Version_number, 4, 0, 0);
	if(!strchr(tmp_name,':'))
		sprintf(serv_name,"%s_%s",EP_domain_name,tmp_name);
	else
		strcpy(serv_name,tmp_name);
	return(dis_start_serving(serv_name));
}
/*==========================================================================*/
void form_state(state)
char *state;
{
	int i;
	int size;
	static char* SMI_state_tmp = 0;
	static int SMI_state_tmp_size = 0;
	int stateLen = strlen(state);
	int SMI_state_tmp_Len;

/* for some reason some users terminate the state by a '\' */
	if(SMI_state_tmp_size < stateLen+1)
	{
		if(SMI_state_tmp_size)
			free(SMI_state_tmp);
		SMI_state_tmp = malloc(strlen(state)+1);
		SMI_state_tmp_size = strlen(state)+1;
	}
	strcpy(SMI_state_tmp, state);
	if(SMI_state_tmp[strlen(SMI_state_tmp)-1] == '\\')
		SMI_state_tmp[strlen(SMI_state_tmp)-1] = '\0';
/* SMI is not Case sensitive so uppercase state */

	SMI_state_tmp_Len = strlen(SMI_state_tmp);
	for(i = 0; i < SMI_state_tmp_Len; i++)
		SMI_state_tmp[i] = toupper(SMI_state_tmp[i]);
/* First calculate the length of the string 
  *** A T T E N T I O N !!  The following calculation must map exactly the
    filling code. I know that there is a better way to do it, but this
    is so simple */
	size = 1;
	size = size + strlen(SMI_state_tmp);
	/*strcpy(SMI_state,state);*/
	for(i = 0; i<N_parameters; i++)
	{
		size = size + 1;
		/*strcat(SMI_state,"/");*/
		size = size + strlen(Pars[i].name);
		/*strcat(SMI_state,Pars[i].name);*/
		size = size + strlen(Pars[i].type_str);
		/*strcat(SMI_state,Pars[i].type_str);*/
		size = size + 1;
		/*strcat(SMI_state,"=");*/
		if ( Pars[i].esc_value )
		{
			size = size + strlen(Pars[i].esc_value);
			/*strcat(SMI_state,Pars[i].esc_value);*/
		}
		else
		{
			size = size + strlen(Pars[i].value);
			/*strcat(SMI_state,Pars[i].value);*/
		}
	}
/* now allocate the space */
/*printf("size=%d\n",size);*/
	if(SMI_state_size < size)
	{
		if(SMI_state_size)
			free(SMI_state);
		SMI_state = malloc(size);
		SMI_state_size = size;
	}
/*
	if (SMI_state) free(SMI_state);
	SMI_state = malloc(size);
*/
/* and now fill it  */	
	strcpy(SMI_state,SMI_state_tmp);
	for(i = 0; i<N_parameters; i++)
	{
		strcat(SMI_state,"/");
		strcat(SMI_state,Pars[i].name);
		strcat(SMI_state,Pars[i].type_str);
		strcat(SMI_state,"=");
		if ( Pars[i].esc_value )
		{
			strcat(SMI_state,Pars[i].esc_value);
		}
		else
		{
			strcat(SMI_state,Pars[i].value);
		}
	}
	SMI_state[size-1] = '\0';
	return;
}
/*==================================================================*/	
void store_par_value(index, value, type)
int index, type;
void *value;
{
	PAR *parp;
	int size;
	int nout;

	parp = &Pars[index];
	switch(type)
	{
		case SMI_STRING:
			size = strlen((char *)value)+1;
			if(size > parp->size)
			{
				if(parp->value)
					free(parp->value);
				parp->value = malloc(size);
				parp->size = size;
			}
			strcpy(parp->value,(char *)value);
			strcpy(parp->type_str,"(S)");
			nout = smixx_escape(NULL,value,0);
			parp->esc_size = nout;
			if (nout <= 0)
			{ 
				parp->esc_value = 0;	
			}
			else
			{
				if (parp->esc_value) free(parp->esc_value);
				parp->esc_value = malloc(nout+1);
				nout = smixx_escape(parp->esc_value,value,nout);	
			}
			break;
		case SMI_INTEGER:
			size = 32;
			if(!parp->size)
			{
				parp->value = malloc(size);
				parp->size = size;
			}
			sprintf(parp->value,"%d",*(int*)value);
			strcpy(parp->type_str,"(I)");
			parp->esc_value = 0;
			break;
		case SMI_FLOAT:
			size = 32;
			if(!parp->size)
			{
				parp->value = malloc(size);
				parp->size = size;
			}
			sprintf(parp->value,"%e",*(double*)value);
			strcpy(parp->type_str,"(F)");
			parp->esc_value = 0;
			break;
	}
}

int add_parameter(name, value, type)
char *name;
void *value;
int type;
{
	static int N_alloc_chunks = 0;
	PAR *oldp;

	if(N_parameters == (N_alloc_chunks * PAR_CHUNK))
	{
		oldp = Pars;
		Pars = malloc(sizeof(PAR) * (N_alloc_chunks+1) * PAR_CHUNK);
		if(oldp)
		{
			memcpy(Pars, oldp, sizeof(PAR) * N_alloc_chunks * PAR_CHUNK);
			free(oldp);
		}
		N_alloc_chunks++;
	}
	Pars[N_parameters].name = malloc(strlen(name)+1);
	strcpy(Pars[N_parameters].name,name);
	Pars[N_parameters].type = type;
	Pars[N_parameters].value = 0;
	Pars[N_parameters].size = 0;
	Pars[N_parameters].esc_value = 0;
	Pars[N_parameters].esc_size = 0;
	store_par_value(N_parameters, value, type);
	N_parameters++;
	return(1);
}

int smi_set_par(par, value, type)
char *par;
void *value;
int type;
{
	int i, found = 0;

	if(!Initialized)
	{
		add_parameter(par, value, type);
	}
	else
	{
		for(i = 0; i < N_parameters; i++)
		{
			if(!strcmp(Pars[i].name,par))
			{
				if(type != Pars[i].type)
					return(-1);
				store_par_value(i, value, type);
				found = 1;
				break;
			}
		}
		if(!found)
			return(0);
	}
	return(1);
}

int smi_set_state(state)
char *state;
{
/*	static char* buffer; */

	DISABLE_AST
	if(!Initialized)
	{
		Initialized = 1;
	}
/*
	buffer = malloc(strlen(state)+1);
	strcpy(buffer,state);
	Cmnd_valid = 0;
	form_state(buffer);
	free(buffer);
*/
	Cmnd_valid = 0;
	form_state(state);
	if(Info_print)
	{
		smi_print_date_time();
		printf(" %s::%s in state %s\n",EP_domain_name, EP_object, SMI_state);
		fflush(stdout);
	}
	dis_update_service(SMI_state_id);
	ENABLE_AST
	return(1);
}

int smi_terminate_command(state)
char *state;
{
/*	static char* buffer;  */

	DISABLE_AST
/*	
	buffer = malloc(strlen(state)+1);
	strcpy(buffer,state);
	form_state(buffer);
	free(buffer);
*/
	Cmnd_valid = 0;
	form_state(state);
	if(Info_print)
	{
		smi_print_date_time();
		printf(" %s::%s terminating in state %s\n",EP_domain_name, EP_object, SMI_state);
		fflush(stdout);
	}
	dis_update_service(SMI_state_id);
	ENABLE_AST
	return(1);
}

int smi_terminate_action(state)
char *state;
{
	return smi_terminate_command(state);
}

void execute_cmnd(tag, buffer, size)
int *tag, *size;
char *buffer;
{
	tag = tag;
	
	if(*size > SMI_command.size)
	{
		if(SMI_command.size)
			free(SMI_command.token_buff);
		SMI_command.token_buff = malloc(*size);
		SMI_command.size = *size;
	}
	strcpy(SMI_command.token_buff, buffer);
	init_token(&SMI_command);
	if(Info_print)
	{
		smi_print_date_time();
		printf(" %s::%s executing %s\n",EP_domain_name, EP_object, 
			SMI_command.token_buff);
		fflush(stdout);
	}
	Cmnd_arrived = 1;
	Cmnd_valid = 1;
	if(SMI_ef)
	{
#ifdef VMS
		sys$setef(SMI_ef);
#endif
	}
	if(SMI_ast)
	{
		SMI_ast();
	}
	if(!Cmnd_wake_up)
	{
		Cmnd_wake_up = 1;
#ifdef VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
}

void timeout_ast(tag)
int tag;
{
	tag = tag;
	
	if(!Cmnd_wake_up)
	{
		Cmnd_wake_up = 1;
#ifdef VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
}

int smi_get_command(cmnd, size)
char *cmnd;
int *size;
{

#ifdef vms
	if(SMI_ef)
		sys$clref(SMI_ef);
#endif
	Cmnd_arrived = 0;
	if(!Cmnd_valid)
		return(0);
	strcpy(cmnd, SMI_command.token_buff);
	*size = strlen(cmnd);
	return(1);
}

/*int smi_get_commandw_for(cmnd, size, timeout)
char *cmnd;
int *size;
int timeout;
{
	return(smi_get_commandw(cmnd, size, timeout));
}*/

int smi_get_commandw(cmnd, size, timeout)
char *cmnd;
int *size;
int timeout;
{
	if(Cmnd_arrived)
	{
		Cmnd_arrived = 0;
		strcpy(cmnd, SMI_command.token_buff);
		*size = strlen(SMI_command.token_buff);
		return(1);
	}
	Cmnd_wake_up = 0;
	if(timeout)
		dtq_start_timer(timeout, timeout_ast, 0);
	while(!Cmnd_wake_up)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	if(Cmnd_arrived)
	{
		if(timeout)
			dtq_stop_timer(0);
		Cmnd_arrived = 0;
		strcpy(cmnd, SMI_command.token_buff);
		*size = strlen(SMI_command.token_buff);
		return(1);
	}
	return(0);
}

int smi_get_commandw_for(cmnd, size, timeout)
char *cmnd;
int *size;
int timeout;
{
	return(smi_get_commandw(cmnd, size, timeout));
}
int smi_get_action(cmnd, n_pars)
char *cmnd;
int *n_pars;
{
char *ptr;


#ifdef vms
	if(SMI_ef)
		sys$clref(SMI_ef);
#endif

	Cmnd_arrived = 0;
	if(!Cmnd_valid)
		return(0);

	init_token(&SMI_command);
	*n_pars = 0;
	get_token(&SMI_command,&ptr);
	strcpy(cmnd,ptr);
	while(get_token(&SMI_command,&ptr))
	{
		if(cmp_token(ptr,"/"))
			(*n_pars)++;
	}	
	return(1);
}

int smi_test_action(cmnd)
char *cmnd;
{
char cmnd_only[MAX_NAME];
char *ptr;

	Cmnd_arrived = 0;
	if(!Cmnd_valid)
		return(0);
	init_token(&SMI_command);
	get_token(&SMI_command,&ptr);
	strcpy(cmnd_only, ptr);
	return(!(strcmp(cmnd,cmnd_only)));
}

static int check_type(str)
char *str;
{
	int type;

	switch(*str)
	{
		case 'S':
			type = SMI_STRING;
			break;
		case 'I':
			type = SMI_INTEGER;
			break;
		case 'F':
			type = SMI_FLOAT;
			break;
	}
	return(type);
}

static int get_next_param(ptr, par, type, size)
TOKEN_STR *ptr;
char *par;
int *type, *size;
{

	char *aux_ptr;

	*type = SMI_STRING;
	push_token(ptr);
	while(get_token(ptr,&aux_ptr))
	{
		if(cmp_token(aux_ptr,"/"))
		{
			get_token(ptr,&aux_ptr);
			strcpy(par, aux_ptr);
			push_token(ptr);
			get_token(ptr,&aux_ptr);
			if(cmp_token(aux_ptr,"("))
			{
				get_token(ptr,&aux_ptr);
				*type = check_type(aux_ptr);
				get_token(ptr,&aux_ptr);
			}
			else
				pop_token(ptr);

			get_token(ptr,&aux_ptr);
			if(cmp_token(aux_ptr,"="))
			{
				get_token(ptr,&aux_ptr);
				switch(*type)
				{
					case SMI_STRING:
						*size = smixx_unescape(NULL,aux_ptr,0);
						if(aux_ptr[0] == '"')
						{
							*size = smixx_unescape(NULL,aux_ptr,0);
						}
						else
						{
							while(get_token(ptr,&aux_ptr))
							{
								if(cmp_token(aux_ptr,"/"))
								{
									break;
								}
/*
								if(cmp_token(aux_ptr,"|"))
								{
									break;
								}
*/
								else
									*size += smixx_unescape(NULL,aux_ptr,0);
							}
						}
						break;
					case SMI_INTEGER:
						*size = sizeof(int);
						break;
					case SMI_FLOAT:
						*size = sizeof(double);
						break;
				}
			}
			else
				*size = 0;
			pop_token(ptr);
			return(1);
		}
/*
		if(cmp_token(aux_ptr,"|"))
		{
			break;
		}
*/
	}
	pop_token(ptr);
	return(0);
}

int smi_get_next_par(param, type, size)
char *param;
int *type, *size;
{

	if(!Cmnd_valid)
		return(0);
	if(Cmnd_valid == 1)
	{
		init_token(&SMI_command);
		Cmnd_valid = 2;
	}
	return(get_next_param(&SMI_command, param, type, size));
}

static int get_param_value(ptr, val)
TOKEN_STR *ptr;
void *val;
{
	char *strval;
	int nout;
	char *aux_ptr;
	int type, ret = 0;
	int* pintval;  double* pdoubleval;
	
	pintval = val;  pdoubleval = val;

	type = SMI_STRING;
	push_token(ptr);
	get_token(ptr,&aux_ptr);
	if(cmp_token(aux_ptr,"("))
	{
		get_token(ptr,&aux_ptr);
		type = check_type(aux_ptr);
		get_token(ptr,&aux_ptr);
	}
	else
		pop_token(ptr);

	get_token(ptr,&aux_ptr);
	if(cmp_token(aux_ptr,"="))
	{
		get_token(ptr,&aux_ptr);
		switch(type)
		{
			case SMI_STRING:
				if(aux_ptr[0] == '"')
				{
					nout = smixx_unescape(NULL,aux_ptr,0);
					nout = smixx_unescape(val,aux_ptr,nout);
				}
				else
				{
					nout = smixx_unescape(NULL,aux_ptr,0);
					nout = smixx_unescape(val,aux_ptr,nout);
					/*strcpy(val, aux_ptr);*/
					while(get_token(ptr,&aux_ptr))
					{
						if(cmp_token(aux_ptr,"/"))
						{
							break;
						}
/*
						if(cmp_token(aux_ptr,"|"))
						{
							break;
						}
*/
						else
							strval = (char*) val; /*to make compiler happy */
							nout = smixx_unescape(NULL,aux_ptr,0);
							nout = smixx_unescape(&strval[strlen(strval)],aux_ptr,nout);
							/*strcat(val,aux_ptr);*/
					}
/*
					push_token(ptr);
					strcpy(val, aux_ptr);
					while(get_token(ptr,&aux_ptr))
					{
						if(cmp_token(aux_ptr,"/"))
						{
							pop_token(ptr);
							break;
						}
						else
							strcat(val,aux_ptr);
					}
*/
				}
				break;
			case SMI_INTEGER:
				if(strchr(aux_ptr,'x'))
					sscanf(aux_ptr,"%x",pintval);
				else
					sscanf(aux_ptr,"%d",pintval);
				break;
			case SMI_FLOAT:
				sscanf(aux_ptr,"%lf",pdoubleval);
				break;
		}   	
		ret = 1;
	}
	pop_token(ptr);
	return(ret);
}

int smi_get_par_value(param, value)
char *param;
void *value;
{
	int type, size;
	char aux_param[32];

	if(!Cmnd_valid)
		return(0);
	init_token(&SMI_command);
	while(get_next_param(&SMI_command, aux_param, &type, &size))
	{
		if(!strcmp(param, aux_param))
		{
			get_param_value(&SMI_command, value);
			return(1);
		}
	}
	return(0);
}

int smi_get_state(state, max_len)
char *state;
int max_len;
{
	char *ptr;
	int SMI_state_Len;

	strncpy(state, SMI_state, max_len);
	if ( (ptr = strchr(state,'/')) )
		*ptr = '\0';
	else
	{
		SMI_state_Len = strlen(SMI_state);
		if(SMI_state_Len >= max_len)
			state[max_len - 1] = '\0';
		else
			state[strlen(SMI_state)] = '\0';
	}
	return(1);
}

int smi_get_substate(subobj, state)
char *subobj, *state;
{
	int i;

	for(i = 0; i < N_sub_objects; i++)
	{
		if(!strcmp(Subs[i].name,subobj))
		{
			strcpy(state,Subs[i].state);
			return(1);
		}
	}
	return(0);
}

static int init_token(ptr)
TOKEN_STR *ptr;
{
	ptr->token_ptr = ptr->token_buff;
	ptr->curr_token_ptr = 0;
	return(1);
}

static int get_token(ptr,token)
TOKEN_STR *ptr;
char **token;
{
static char *Actions;
static int Actions_size = 0;
char *Actions_ptr = 0;
char *aux_ptr;
int in_tok;
int token_buff_Len;

	if(!ptr->curr_token_ptr)
	{
		ptr->curr_token_ptr = ptr->token_buff;
	}
	token_buff_Len = strlen(ptr->token_buff);
	if(Actions_size < token_buff_Len+1)
	{
		if(Actions_size)
			free(Actions);
		Actions = malloc(strlen(ptr->token_buff)+1);
		Actions_size = strlen(ptr->token_buff)+1;
	}
	aux_ptr = ptr->curr_token_ptr;
	Actions_ptr = Actions;	

	in_tok = 0;
	while(*aux_ptr)
	{
		if(*aux_ptr == '"')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			aux_ptr = strchr(aux_ptr,'"');
			aux_ptr++;
			break;
		}
		else if(*aux_ptr == '/')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			break;
		}
		else if(*aux_ptr == '|')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			break;
		}
		else if(*aux_ptr == '=')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			break;
		}
		else if(*aux_ptr == '(')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			break;
		}
		else if(*aux_ptr == ')')
		{
			if(in_tok)
				break;
			in_tok = 1;
			aux_ptr++;
			break;
		}
		else
		{
			*Actions_ptr++ = *aux_ptr++;
			in_tok = 2;
		}
	}
	if(in_tok == 2)
	{
		*Actions_ptr = '\0';
		*token = Actions;
		ptr->curr_token_ptr = aux_ptr;
		return(1);
	}
	if(in_tok == 1)
	{
		strncpy(Actions, ptr->curr_token_ptr, aux_ptr - ptr->curr_token_ptr);
		Actions[aux_ptr - ptr->curr_token_ptr] = '\0';
		*token = Actions;
		ptr->curr_token_ptr = aux_ptr;
		return(1);
	}
	ptr->curr_token_ptr = 0;
	return(0);
}		

static int push_token(ptr)
TOKEN_STR *ptr;
{
	ptr->token_ptr = ptr->curr_token_ptr;
	return(1);
}

static int pop_token(ptr)
TOKEN_STR *ptr;
{
	ptr->curr_token_ptr = ptr->token_ptr;
	return(1);
}

static int cmp_token(token, str)
char *token, *str;
{
	if(!strcmp(token, str))
		return(1);
	return(0);
}

/*static int first_token(ptr)
TOKEN_STR *ptr;
{
	if(ptr->token_ptr == ptr->token_buff)
		return(1);
	return(0);
}*/

#ifdef VMS
int get_log_name(log_name, table, name)
char *log_name, *table, *name;
{
short length;
int ret;
struct dsc$descriptor_s tabnam_dsc;
struct dsc$descriptor_s lognam_dsc;

struct {
            short  length;
            short  code;
            char   *address;
            short  *len_add;
        } trnlnm_itmlst[] =
        {
            132,LNM$_STRING, 0, &length,
            0, 0, 0, 0
        };

	tabnam_dsc.dsc$a_pointer = table;
	tabnam_dsc.dsc$w_length = strlen(table);
	lognam_dsc.dsc$a_pointer = log_name;
	lognam_dsc.dsc$w_length = strlen(log_name);
	trnlnm_itmlst[0].address = name;
    ret = sys$trnlnm (0, &tabnam_dsc, &lognam_dsc, 0, trnlnm_itmlst);
    if (ret != SS$_NORMAL)
		return(0);
	name[length] = '\0';
	return(1);
}

get_domain_name(name)
char *name;
{
	get_log_name("SMI_DOMAIN","LNM$FILE_DEV",name);
}

int smi_send_command(obj, cmnd)
char *obj, *cmnd;
{
	char serv_name[MAX_NAME];
			
	get_obj_name(obj,serv_name,0);
	strcat(serv_name,"/CMD");
	dic_cmnd_service(serv_name, cmnd, strlen(cmnd)+1);
	return(1);
}

#endif

/* CFORTRAN WRAPPERS */

#ifdef VMS
FCALLSCFUN2(INT, smi_attach, SMI_ATTACH, smi_attach,
                 STRING, PVOID)
FCALLSCFUN2(INT, smi_init_for, SMI_INIT_FOR, smi_init_for,
                 INT, INT)
FCALLSCFUN1(INT, smi_associate, SMI_ASSOCIATE, smi_associate,
                 STRING)
FCALLSCFUN1(INT, smi_set_state, SMI_SET_STATE, smi_set_state,
                 STRING)
FCALLSCFUN2(INT, smi_get_state, SMI_GET_STATE, smi_get_state,
                 STRING, INT)
FCALLSCFUN2(INT, smi_get_substate, SMI_GET_SUBSTATE, smi_get_substate,
                 STRING, STRING)
FCALLSCFUN1(INT, smi_terminate_command, SMI_TERMINATE_COMMAND, 
				 smi_terminate_command,
                 STRING)
FCALLSCFUN1(INT, smi_terminate_action, SMI_TERMINATE_ACTION, 
				 smi_terminate_action,
                 STRING)
FCALLSCFUN2(INT, smi_get_command, SMI_GET_COMMAND,
				 smi_get_command,
                 PSTRING, PINT)
FCALLSCFUN3(INT, smi_get_commandw_for, SMI_GET_COMMANDW_FOR,
				 smi_get_commandw_for,
                 PSTRING, PINT, INT)
FCALLSCFUN2(INT, smi_get_action, SMI_GET_ACTION,
				 smi_get_action,
                 PSTRING, PINT)
FCALLSCFUN1(INT, smi_test_action, SMI_TEST_ACTION,
				 smi_test_action,
                 STRING)
FCALLSCFUN3(INT, smi_get_next_par, SMI_GET_NEXT_PAR,
				 smi_get_next_par,
                 PSTRING, PINT, PINT)
FCALLSCFUN2(INT, smi_get_par_value, SMI_GET_PAR_VALUE,
				 smi_get_par_value,
                 STRING, PVOID)
FCALLSCFUN3(INT, smi_set_par, SMI_SET_PAR,
				 smi_set_par,
                 STRING, PVOID, INT)
FCALLSCFUN2(INT, smi_send_command, SMI_SEND_COMMAND, smi_send_command,
                 STRING, STRING)
FCALLSCSUB0(smi_volatile, SMI_VOLATILE, smi_volatile)
#endif


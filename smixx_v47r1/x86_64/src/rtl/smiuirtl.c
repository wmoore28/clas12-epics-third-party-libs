#define SMIUILIB
#include "smiuirtl.h"
#include "ctype.h"

#include <dim.h>

#ifdef VMS
#include <cfortran.h>
#include <ssdef.h>   
#include <iodef.h>   
#include <lnmdef.h>    
#endif

#include <stdlib.h>
#include <string.h>

#include "smixx_parstring_util.h"

typedef struct {
	int state;
	char* action;
}BUSY;
/* the following structure is there only to 'cast' in 'get_state_busy_rout' */
typedef struct {
	int state;
	char action[1];
}CASTBUSY;

typedef struct {
	char *token_buff;
	int size;
	char *token_ptr;
	char *curr_token_ptr;
}TOKEN_STR;

typedef struct dom_info{
	struct dom_info *next;
	char domain[80];

	int objlist_id;
	char *info;
	TOKEN_STR *objs;
	int size;
	int obj_index;
	void (*user_rout)();
	dim_long par;
	int nobjs;
	int objsetlist_id;
	TOKEN_STR *objsets;
	int sizes;
	int objset_index;
	void (*user_set_rout)();
	int setpar;
	int nobjsets;
	int nanswers;
}DOMAIN_INFO;

typedef struct {
	char obj_name[MAX_NAME];
	int state_id;
	int busy_id;
	int action_id;
	void (*user_rout)();
	dim_long par;
	TOKEN_STR *state;
	BUSY busyData; 
	TOKEN_STR *actions;
	DOMAIN_INFO *domain_ptr;
	int counter;
	int id;
}STATECHANGE;

typedef struct {
	char obj_name[MAX_NAME];
	int objs_id;
	void (*user_rout)();
	dim_long par;
	TOKEN_STR *objs;
	DOMAIN_INFO *domain_ptr;
	int id;
}SETCHANGE;
/*
typedef char STR_ARR[MAXNAMESIZE];
*/
/*static char My_address[MAX_NAME];*/
static int Get_obj_busy = -1;
static int Get_obj_state = 0;
static char No_link_str[8] = "DEAD";
static int No_link_busy = 0;
static int No_link = -1;
/*static char No_link_char = -1;*/
static int Wake_up_domain = 0;
static int Wake_up_proxy = 0;
static int Wake_up_exit = 0;
static int Wake_up_state = 0;
static int Wake_up_curr_state = 0;
static int Wake_up_cmd = 0;
/*static int Domain_serv_id = 0;*/
static int Booking_statechange = 0;
static DOMAIN_INFO *Domain_head = (DOMAIN_INFO *) 0;

_PROTO( static int init_token, (TOKEN_STR *ptr) );
_PROTO( static int get_token,  (TOKEN_STR *ptr, char **token) );
_PROTO( static int push_token, (TOKEN_STR *ptr) );
_PROTO( static int pop_token,  (TOKEN_STR *ptr) );
_PROTO( static int cmp_token,  (char *token, char *str) );
_PROTO( static int first_token,(TOKEN_STR *ptr) );

static void capitalize(name)
char *name;
{
	while(*name)
	{
		*name = toupper(*name);
		name++;
	}
}

static void capitalize_sel(name)
char *name;
{
	int par_value = 0;

	while(*name)
	{
		if(*name == '=')
			par_value = 1;
		if(*name == '/')
			par_value = 0;
		if(!par_value)
			*name = toupper(*name);
		name++;
	}
}

int get_service_name(obj_name, serv_name, ep, set)
char *obj_name, *serv_name;
int ep, set;
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
		strcpy(serv_name,"NONE");
		return(0);
	}
	if(ep)
		sprintf(serv_name,"EP/%s/%s", ptr_domain, ptr_obj);
	else if(set)
		sprintf(serv_name,"SMI/%s/SET/%s", ptr_domain, ptr_obj);
	else
		sprintf(serv_name,"SMI/%s/%s", ptr_domain, ptr_obj);
	return(1);
}
/*
void get_domain_info_rout(domain_ptr_ptr, buff, size)
DOMAIN_INFO **domain_ptr_ptr;
char *buff;
int *size;
{
DOMAIN_INFO *domain_ptr;
int n_objs;

	domain_ptr = *domain_ptr_ptr;
	domain_ptr->nanswers++;
	if(!strcmp(buff,No_link_str))
		domain_ptr->nobjs = 0;
	else
	{
		domain_ptr->info = malloc((*size)+1);
		memcpy(domain_ptr->info,buff,*size);
		domain_ptr->info[*size] = '\0';
		domain_ptr->size = *size;
		n_objs = domain_ptr->size/MAXNAMESIZE;
		if(!strcmp(domain_ptr->info,"&ALLOC"))
			n_objs--;
		domain_ptr->nobjs = n_objs;
	}
	if(domain_ptr->nanswers == 2)
	{
		if(domain_ptr->user_rout)
		{
			domain_ptr->user_rout(&domain_ptr->par, &domain_ptr->nobjs);
		}
		domain_ptr->nanswers = 0;
	}
	Wake_up++;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}
*/
static int get_next_attr(ptr, attr)
TOKEN_STR *ptr;
char *attr;
{
	char *aux_ptr;

	push_token(ptr);
	while(get_token(ptr,&aux_ptr))
	{
		if(cmp_token(aux_ptr,"/"))
		{
			get_token(ptr,&aux_ptr);
			strcpy(attr, aux_ptr);
			return(1);
		}
		if(cmp_token(aux_ptr,"|"))
		{
			break;
		}
	}
	pop_token(ptr);
	return(0);
}
/*
static int get_next_obj(ptr, obj)
TOKEN_STR *ptr;
char *obj;
{
	char *aux_ptr;
	int ret = 0, found = 0;

	if(ptr == 0)
		return(0);

	if(get_token(ptr,&aux_ptr))
	{
		strcpy(obj,aux_ptr);
		if(cmp_token(aux_ptr,"&ALLOC"))
			ret = 2;
		else
			ret = 1;
		push_token(ptr);
		while(get_token(ptr,&aux_ptr))
		{
			if(cmp_token(aux_ptr,"|"))
			{
				found = 1;
				break;
			}
			push_token(ptr);
		}
	}
	if(!found)
		pop_token(ptr);
	return(ret);
}
*/

static int get_next_obj(ptr, obj)
TOKEN_STR *ptr;
char *obj;
{
	char *aux_ptr;
	int ret = 0, found = 0;

	if(ptr == 0)
		return(0);

	push_token(ptr);
	if(!get_token(ptr, &aux_ptr))
	{
		pop_token(ptr);
		return(0);
	}
	if(cmp_token(aux_ptr,"/"))
	{
		push_token(ptr);
		while(get_token(ptr,&aux_ptr))
		{
			if(cmp_token(aux_ptr,"|"))
			{
				found = 1;
				break;
			}
			push_token(ptr);
		}
		if(!found)
			pop_token(ptr);
	}
	else if(cmp_token(aux_ptr,"|"))
	{
	}
	else
		pop_token(ptr);
	if(get_token(ptr,&aux_ptr))
	{
		strcpy(obj,aux_ptr);
		if(cmp_token(aux_ptr,"&ALLOC"))
			ret = 2;
		else
			ret = 1;
	}
	return(ret);
}

void get_objlist_info_rout(domain_ptr_ptr, buff, size)
DOMAIN_INFO **domain_ptr_ptr;
char *buff;
int *size;
{
DOMAIN_INFO *domain_ptr;
int n_objs = 0, ret;
char obj[MAX_NAME];

	
	domain_ptr = *domain_ptr_ptr;
	domain_ptr->nanswers++;
	if(!strcmp(buff,No_link_str))
	{
		domain_ptr->nobjs = 0;
		if(domain_ptr->objs)
		{
			free(domain_ptr->objs);
			domain_ptr->objs = 0;
		}
	}
	else
	{
		domain_ptr->objs = malloc(sizeof(TOKEN_STR));
		domain_ptr->objs->token_buff = malloc(*size);
		domain_ptr->objs->size = *size;
		strcpy(domain_ptr->objs->token_buff,buff);
		domain_ptr->size = *size;
		init_token(domain_ptr->objs);
		while ( (ret = get_next_obj(domain_ptr->objs, obj)) )
		{
			if(ret == 1)
				n_objs++;
		}
		domain_ptr->nobjs = n_objs;
		init_token(domain_ptr->objs);
	}
	if(domain_ptr->nanswers == 2)
	{
		if(domain_ptr->user_rout)
		{
			domain_ptr->user_rout(&domain_ptr->par, &domain_ptr->nobjs);
		}
		domain_ptr->nanswers = 0;
	}
	Wake_up_domain++;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

void get_objsetlist_info_rout(domain_ptr_ptr, buff, size)
DOMAIN_INFO **domain_ptr_ptr;
char *buff;
int *size;
{
DOMAIN_INFO *domain_ptr;
int n_objsets = 0, ret;
char objset[MAX_NAME];

	
	domain_ptr = *domain_ptr_ptr;
	domain_ptr->nanswers++;
	if(!strcmp(buff,No_link_str))
	{
		domain_ptr->nobjsets = 0;
		if(domain_ptr->objsets)
		{
			free(domain_ptr->objsets);
			domain_ptr->objsets = 0;
		}
	}
	else
	{
		domain_ptr->objsets = malloc(sizeof(TOKEN_STR));
		domain_ptr->objsets->token_buff = malloc(*size);
		domain_ptr->objsets->size = *size;
		strcpy(domain_ptr->objsets->token_buff,buff);
		domain_ptr->size = *size;
		init_token(domain_ptr->objsets);
		while ( (ret = get_next_obj(domain_ptr->objsets, objset)) )
		{
			if(ret == 1)
				n_objsets++;
		}
		domain_ptr->nobjsets = n_objsets;
		init_token(domain_ptr->objsets);
	}
	if(domain_ptr->nanswers == 2)
	{
		if(domain_ptr->user_rout)
		{
			domain_ptr->user_rout(&domain_ptr->par, &domain_ptr->nobjs);
		}
		domain_ptr->nanswers = 0;
	}
	Wake_up_domain++;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}

static DOMAIN_INFO *find_domain(domain)
char *domain;
{
	DOMAIN_INFO *domain_ptr;

    if( !Domain_head ) {
		return((DOMAIN_INFO *)0);
	}
	domain_ptr = (DOMAIN_INFO *)sll_search( (SLL *)Domain_head, 
						domain, strlen(domain)+1);
	return( domain_ptr);
}

static DOMAIN_INFO *connect_domain(domain, user_rout, par)
char *domain;
void (*user_rout)();
dim_long par;
{
	char serv_name[MAX_NAME];
	DOMAIN_INFO *domain_ptr;

    if( !Domain_head ) {
		Domain_head = (DOMAIN_INFO *)malloc(sizeof(DOMAIN_INFO));
		sll_init( (SLL *) Domain_head );
	}
	if(!(domain_ptr = find_domain(domain)))
	{
		domain_ptr = malloc(sizeof(DOMAIN_INFO));
		strcpy(domain_ptr->domain,domain);
		domain_ptr->user_rout = user_rout;
		domain_ptr->par = par;
		domain_ptr->info = 0;
		domain_ptr->objs = 0;       
		domain_ptr->nobjs = 0;
		domain_ptr->objsets = 0;       
		domain_ptr->nobjsets = 0;
		domain_ptr->nanswers = 0;
		Wake_up_domain = 0;
		sll_insert_queue( (SLL *) Domain_head, (SLL *) domain_ptr );
		sprintf(serv_name,"SMI/%s/OBJECTSET_LIST",domain);
		domain_ptr->objsetlist_id = dic_info_service(serv_name, MONITORED, 0, 
			0, 0, 
			get_objsetlist_info_rout, (dim_long)domain_ptr, No_link_str, 5);
		sprintf(serv_name,"SMI/%s/OBJECT_LIST",domain);
		domain_ptr->objlist_id = dic_info_service(serv_name, MONITORED, 0, 
			0, 0, 
			get_objlist_info_rout, (dim_long)domain_ptr, No_link_str, 5);
/*
		sprintf(serv_name,"SMI/%s/DOMAIN",domain);
		domain_ptr->dom_id = dic_info_service(serv_name, MONITORED, 0, 0, 0, 
			get_domain_info_rout, (int)domain_ptr, No_link_str, 5);
*/
  }
	return( domain_ptr);
}

static int disconnect_domain(domain, remove)
char *domain;
int remove;
{
DOMAIN_INFO *domain_ptr;

    if( !Domain_head ) {
		Domain_head = (DOMAIN_INFO *)malloc(sizeof(DOMAIN_INFO));
		sll_init( (SLL *) Domain_head );
	}
	if ( (domain_ptr = find_domain(domain)) )
	{
/*
		dic_release_service(domain_ptr->dom_id);
*/
		dic_release_service(domain_ptr->objlist_id);
		dic_release_service(domain_ptr->objsetlist_id);
		if(remove)
		{
			if(domain_ptr->info)
				free(domain_ptr->info);
			if(domain_ptr->objs)
				free(domain_ptr->objs);
			if(domain_ptr->objsets)
				free(domain_ptr->objsets);
			sll_remove( (SLL *) Domain_head, (SLL *) domain_ptr );
			free(domain_ptr);
		}
		return(1);
	}
	return(0);
}

int smiui_connect_domain(domain)
char *domain;
{
	DOMAIN_INFO *domain_ptr;
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	domain_ptr = find_domain(tmp_domain);
	if(!domain_ptr)
	{
		domain_ptr = connect_domain(tmp_domain, 0, 0);
		while(Wake_up_domain < 2)
		{
#ifdef VMS
			sys$hiber();
#else
			dim_wait();
#endif
		}
		if(!domain_ptr->nobjs)
		{
			disconnect_domain(tmp_domain, 1);
			return(0);
		}
		disconnect_domain(tmp_domain, 0);
	}
	domain_ptr->obj_index = 0;
	domain_ptr->objset_index = 0;
	return(domain_ptr->nobjs);
}
                     
int smiui_book_connect_domain(domain, user_rout, par)
char *domain;
void (*user_rout)();
dim_long par;
{
	DOMAIN_INFO *domain_ptr;
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	if(!(domain_ptr = find_domain(tmp_domain)))
	{
		domain_ptr = connect_domain(tmp_domain, user_rout, par);
		domain_ptr->obj_index = 0;
		domain_ptr->objset_index = 0;
		return(0);
	}
	else
	{
		init_token(domain_ptr->objs);
		init_token(domain_ptr->objsets);
	}
	domain_ptr->obj_index = 0;
	domain_ptr->objset_index = 0;
	return(domain_ptr->nobjs);
}
    
int smiui_cancel_connect_domain(domain)
char *domain;
{
/*	char serv_name[MAX_NAME]; */
/*	DOMAIN_INFO *domain_ptr; */
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	return(disconnect_domain(tmp_domain, 1));
}

int smiui_shutdown_domain(domain)
char *domain;
{
	char serv_name[MAX_NAME];
	int exit_code = 1;
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	/*
	sprintf(serv_name,"SMI/%s/&ALLOC/CMD",domain);
	dic_cmnd_service(serv_name, "EXIT", 5);
	*/
	sprintf(serv_name,"%s_SMI/EXIT",tmp_domain);
	dic_cmnd_service(serv_name, &exit_code, 4);
	return(1);
}

int smiui_number_of_objects(domain)
char *domain;
{

	return(smiui_connect_domain(domain));
}

int smiui_number_of_objectsets(domain)
char *domain;
{

	DOMAIN_INFO *domain_ptr;
/*	int ret; */
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	domain_ptr = find_domain(tmp_domain);
	if(!domain_ptr)
	{
		smiui_connect_domain(tmp_domain);
		domain_ptr = find_domain(tmp_domain);
	}
	if((!domain_ptr) || (!domain_ptr->nobjs))
		return(0);
	return(domain_ptr->nobjsets);
}

void get_proxy(tag, buff, size)
int *tag;
int *buff;
int *size;
{
	tag = tag; buff = buff; size = size;
	Wake_up_proxy++;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

int smiui_check_proxy(obj)
char *obj;
{
	char serv_name[MAX_NAME];
	static char state[MAX_NAME];

	strcpy(serv_name,obj);
	capitalize(serv_name);
	strcat(serv_name,"/VERSION_NUMBER");
	Wake_up_proxy = 0;
	dic_info_service(serv_name, ONCE_ONLY, 20, state, MAX_NAME, 
		get_proxy, 0, No_link_str, 5);
	while(!Wake_up_proxy)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	if(!strcmp(state, No_link_str))
		return(0);
	return(1);
}                 

void get_exit_res(tag, res)
int *tag;
int *res;
{
	tag = tag;  res = res;
	Wake_up_exit++;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

int smiui_kill(obj)
char *obj;
{
	char serv_name[MAX_NAME];
	int exit_code = 1;

	strcpy(serv_name,obj);
	capitalize(serv_name);
	if(strstr(serv_name,"::"))
	  strcat(serv_name,"/EXIT");
	else
	  strcat(serv_name,"_SMI/EXIT");
	Wake_up_exit = 0;
	dic_cmnd_callback(serv_name, &exit_code, sizeof(int),
		get_exit_res, 0);
	while(!Wake_up_exit)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	return(1);
}                 

int smiui_get_next_object(domain,object)
char *domain;
char *object;
{
	DOMAIN_INFO *domain_ptr;
/*
	STR_ARR *info_ptr;
*/
	char obj[MAX_NAME];
	int ret;
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);

	domain_ptr = find_domain(tmp_domain);
	if(!domain_ptr)
	{
		smiui_connect_domain(tmp_domain);
		domain_ptr = find_domain(tmp_domain);
	}
	if((!domain_ptr) || (!domain_ptr->nobjs))
		return(0);
	
	
	if(domain_ptr->objs)
	{
		if ( (ret = get_next_obj(domain_ptr->objs, obj)) )
		{
			if(ret == 2)
				get_next_obj(domain_ptr->objs, obj);
			strcpy(object,tmp_domain);
			strcat(object,"::");
			strcat(object,obj);
			return(1);
		}
		else
			return(0);
	}
/*
	info_ptr = (STR_ARR *)domain_ptr->info;
	if(!info_ptr[domain_ptr->obj_index][0])
	{
		domain_ptr->obj_index = 0;
		return(0);
	}
	if(!strcmp(info_ptr[domain_ptr->obj_index],"&ALLOC"))
	{
		domain_ptr->obj_index++;
		if(!info_ptr[domain_ptr->obj_index][0])
		{
			domain_ptr->obj_index = 0;
			return(0);
		}
	}
	strcpy(object,domain);
	strcat(object,"::");
	strcat(object,info_ptr[domain_ptr->obj_index++]);
	return(1);
*/
	return(1);
}

int smiui_get_next_objectset(domain,objectset)
char *domain;
char *objectset;
{
	DOMAIN_INFO *domain_ptr;
/*
	STR_ARR *info_ptr;
*/
	char objset[MAX_NAME];
	int ret;
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);

	domain_ptr = find_domain(tmp_domain);
	if(!domain_ptr)
	{
		smiui_connect_domain(tmp_domain);
		domain_ptr = find_domain(tmp_domain);
	}
	if((!domain_ptr) || (!domain_ptr->nobjs))
		return(0);
	
	
	if(domain_ptr->objsets)
	{
		if ( (ret = get_next_obj(domain_ptr->objsets, objset)) )
		{
			if(ret == 2)
				get_next_obj(domain_ptr->objsets, objset);
			strcpy(objectset,tmp_domain);
			strcat(objectset,"::");
			strcat(objectset,objset);
			return(1);
		}
		else
			return(0);
	}
/*
	info_ptr = (STR_ARR *)domain_ptr->info;
	if(!info_ptr[domain_ptr->obj_index][0])
	{
		domain_ptr->obj_index = 0;
		return(0);
	}
	if(!strcmp(info_ptr[domain_ptr->obj_index],"&ALLOC"))
	{
		domain_ptr->obj_index++;
		if(!info_ptr[domain_ptr->obj_index][0])
		{
			domain_ptr->obj_index = 0;
			return(0);
		}
	}
	strcpy(object,domain);
	strcat(object,"::");
	strcat(object,info_ptr[domain_ptr->obj_index++]);
	return(1);
*/
	return(1);
}

int smiui_get_next_attribute(object, attr)
char *object;
char *attr;
{
	DOMAIN_INFO *domain_ptr;
	char domain[MAX_NAME], *ptr;
/*	int ret; */

	strcpy(domain, object);
	capitalize(domain);
	ptr = strstr(domain,"::");
	*ptr = 0;
	ptr += 2;
	domain_ptr = find_domain(domain);
	if(!domain_ptr)
	{
		smiui_connect_domain(domain);
		domain_ptr = find_domain(domain);
	}
	if((!domain_ptr) || (!domain_ptr->nobjs))
		return(0);
	
	
	if(domain_ptr->objs)
	{
		return(get_next_attr(domain_ptr->objs, attr));
	}
	else
		return(0);
}

void get_state_rout(tag)
int *tag;
{
	if(*tag)
	{
		Get_obj_state = 1;
	}
	if((!Get_obj_busy) && (Get_obj_state))
	{
		Wake_up_state = 1;
#ifdef VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
	}
}
                     
int smiui_wait_stable_state(obj, state)
char *obj;
char *state;
{
	char serv_name[MAX_NAME];
	int state_id, busy_id;
	void get_state_rout();

	Wake_up_state = 0;
	get_service_name(obj,serv_name,0,0);
	Get_obj_state = 0;
	Get_obj_busy = -1;
	state_id = dic_info_service(serv_name, MONITORED, 0, state, MAX_NAME, 
		get_state_rout, 1, No_link_str, 5);
	strcat(serv_name,"/BUSY");
	busy_id = dic_info_service(serv_name, MONITORED, 0, &Get_obj_busy, 4, 
		get_state_rout, 0, &No_link_busy, 4);
	while(!Wake_up_state)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	dic_release_service(state_id);
	dic_release_service(busy_id);
	if(!strcmp(state,No_link_str))
		return(0);
	return(1);
}
                     
static BUSY Get_obj_state_busy = {-1, 0};

static char* state_for_smiui_current_state = 0;
/* this is purely for use in handling smiui_current_state. The state of the object
 is picked up in the callback, necessary space allocated, pointer to this space
 copied into  'state_for_smiui_current_state' and state copied.  It is then subsequently
 retrieved in smiu_current_state */

/*===============================================================================*/
void get_state_busy_rout(tag,address,size)
int *tag;
void* address;
int* size;
{
CASTBUSY* addressBusy;
char* addressState;
	
	if((Get_obj_state_busy.state != -1) && (Get_obj_state))
		return;
	if (*tag == 0)
	{
	/* this is the busy service callback and Get_obj_state_busy.state will be set
	   either to 0 (object not busy) or to 1 (object busy) */
		addressBusy = (CASTBUSY*) address;
		Get_obj_state_busy.state = addressBusy->state;
		if(Get_obj_state_busy.state == 1)
		{
			Get_obj_state_busy.action = malloc(*size+1);
			strcpy(Get_obj_state_busy.action, addressBusy->action);
		}
	}    
	
	if(*tag)
	{
	/* this is the state service callcack */
		Get_obj_state = 1;
		addressState = (char*) address;
		state_for_smiui_current_state = malloc(*size+1);
		strcpy(state_for_smiui_current_state,addressState);
	}
	if((Get_obj_state_busy.state != -1) && (Get_obj_state))
	{
	/* this means that both (state and busy) services had the callback and
	   we have all the info we need so we can wake up  */
		Wake_up_curr_state = 1;
#ifdef VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
}
/*=========================================================================*/
int smiui_current_state(obj, state, action)
char *obj;
char *state;
char *action;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];
	int state_id, busy_id, ret = 1;

	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	get_service_name(tmp_obj,serv_name,0,0);

	Wake_up_curr_state = 0;
	Get_obj_state = 0;
	Get_obj_state_busy.state = -1;
	state_id = dic_info_service(serv_name, MONITORED, 0, 0, 0, 
		get_state_busy_rout, 1, No_link_str, 5);
	strcat(serv_name,"/BUSY");
	busy_id = dic_info_service(serv_name, MONITORED, 0, 0, 
		0, get_state_busy_rout, 0, &No_link_busy, 4);
	while(!Wake_up_curr_state)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	dic_release_service(state_id);
	dic_release_service(busy_id);

	if(state != 0)
		strcpy(state,state_for_smiui_current_state);
	if(!strcmp(state_for_smiui_current_state,No_link_str))
		ret = 0;
	free(state_for_smiui_current_state);
	
	if(Get_obj_state_busy.state == 1)
	{
		if(action != 0)
			strcpy(action,Get_obj_state_busy.action);
		free(Get_obj_state_busy.action);
	}
	else
	{
		if(action != 0)
			action[0] = '\0';
	}
	return(ret);
}

void smiui_print_date_time()
{
	time_t t;
	char str[128];

	t = time((time_t *)0);
	printf("SMIUIRTL - ");
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
/*========================================================================*/
/* callback booked in smiui_book_statechange. Is invoked by /BUSY service .
The service delivers string of bytes (pointed to by addr) containing
an integer(busy flag) followed by character array (action in progress).
This is the CASTBUSY structure
*/
void busy_change_rout(ptr_ptr, addr, size)
STATECHANGE **ptr_ptr;
void* addr;
int* size;
{
	STATECHANGE *ptr;
	CASTBUSY* addrCASTBUSY; /*parameter addr will be cast into this
				variable. The compiler will then think that it
				is pointing to CASTBUSY structure and the flag and
				the action can be easily retrieved */

	int dim_busyFlg;        /* busy flag delivered by dim */
	char* dim_pBusyAction;  /* pointer to action in progress delivered by dim */
/*	char* heapaddr; */
	
/*
	BUSY obj_busyData;
*/
	int actionSize;
	char nullAction[1] = {'\0'}; /* we use this, when action is not delivered */
	
	ptr = *ptr_ptr;

/* copy the busy data items from DIM into local area */
	addrCASTBUSY = (CASTBUSY*)addr;
	dim_busyFlg = addrCASTBUSY->state;
	actionSize = *size - sizeof(int);
	if(actionSize > 0)
		dim_pBusyAction = &(addrCASTBUSY->action[0]);
	else
	{
		dim_pBusyAction = &nullAction[0];
		actionSize = 1;  /* action size always includes the last null */
	}

	free(ptr->busyData.action);
	ptr->busyData.state = dim_busyFlg;
	ptr->busyData.action = malloc(actionSize);
	strcpy(ptr->busyData.action, dim_pBusyAction);
/*
  get the object's busy data from the 'dynamical memory'  
	obj_busyData = ptr->busyData;

 replace the busy flg with the new one from dim 
	obj_busyData.state = dim_busyFlg;
		
 now the new address for current action
		
 first delete the existing heap space 	
	heapaddr = obj_busyData.action;
	free(heapaddr);
 now allocate the new space and copy the current action from dim 
	heapaddr = malloc(actionSize);
	obj_busyData.action = heapaddr;
	strcpy(heapaddr,dim_pBusyAction);

 now copy the new busy data back to the 'dynamical memory' 
	ptr->busyData = obj_busyData;
*/

	if(dim_busyFlg == 1)
	{
		ptr->counter = 0;
/*
printf("SMIUIRTL - Received %s busyFlag %d %d %08x\n", ptr->obj_name, dim_busyFlg, *size, ptr->counter);
printf("SMIUIRTL - %s busyFlag Calling User busy\n", ptr->obj_name);
*/
		ptr->user_rout(&ptr->id, &ptr->par);
	}
	else if(dim_busyFlg == -1)
	{
		if((ptr->state->size) && (ptr->actions->size))
		{
			if((!strcmp(ptr->state->token_buff,"No link")) &&
				(!strcmp(ptr->actions->token_buff,"No link")))
			{
				ptr->counter = 0;
/*
printf("SMIUIRTL - %s busyFlag Calling User bad\n", ptr->obj_name);
*/
				ptr->user_rout(&ptr->id, &ptr->par);
			}
		}
	}
	else
	{
		ptr->counter |= 0x1;
/*
printf("SMIUIRTL - Received %s busyFlag %d %d %08x\n", ptr->obj_name, dim_busyFlg, *size, ptr->counter);
*/
		if((ptr->counter & 0x7) == 0x7)
		{
			ptr->counter = 0;
/*
printf("SMIUIRTL - %s busyFlag Calling User good\n", ptr->obj_name);
*/
			ptr->user_rout(&ptr->id, &ptr->par);
		}
	}
}
/*=========================================================================================*/
void state_change_rout(ptr_ptr, addr, size)
STATECHANGE **ptr_ptr;
int *size;
char *addr;
{
	STATECHANGE *ptr;
	TOKEN_STR *statep;
	
	BUSY busyData;
	int busyFlg;


	ptr = *ptr_ptr;
	
	busyData = ptr->busyData;
	busyFlg = busyData.state;
	
	statep = ptr->state;
	if(*size > statep->size)
	{
		if(statep->size)
			free(statep->token_buff);
		statep->token_buff = malloc(*size);
		statep->size = *size;
	}
	memcpy(statep->token_buff,addr,*size);
	init_token(statep);
	if(!strcmp(addr,"No link"))
	{
		if((ptr->actions->size) && ( busyFlg != -2))
		{
			if((!strcmp(ptr->actions->token_buff,"No link")) &&
				busyFlg == -1)
			{
				ptr->counter = 0;
/*
printf("SMIUIRTL - %s state Calling User bad\n", ptr->obj_name);
*/
				ptr->user_rout(&ptr->id, &ptr->par);
			}
		}
	}
	else
	{
		ptr->counter |= 0x2;
/*
printf("SMIUIRTL - Received %s in_state %s %d %08x\n", ptr->obj_name, addr, *size, ptr->counter);
*/
		if((ptr->counter & 0x7) == 0x7)
		{
			ptr->counter = 0;
/*
printf("SMIUIRTL - %s state Calling User good\n", ptr->obj_name);
*/
			ptr->user_rout(&ptr->id, &ptr->par);
		}
	}
}
/*=========================================================================================*/
void actions_change_rout(ptr_ptr, addr, size)
STATECHANGE **ptr_ptr;
int *size;
char *addr;
{
	STATECHANGE *ptr;
	TOKEN_STR *actionsp;
	
	BUSY busyData;
	int busyFlg;

	ptr = *ptr_ptr;
	
	busyData = ptr->busyData;
	busyFlg = busyData.state;
	
	actionsp = ptr->actions;
	if(*size > actionsp->size)
	{
		if(actionsp->size)
			free(actionsp->token_buff);
		actionsp->token_buff = malloc(*size);
		actionsp->size = *size;
	}
	memcpy(actionsp->token_buff,addr,*size);
	init_token(actionsp);
	if(!strcmp(addr,"No link"))
	{
		if((ptr->state->size) && ( busyFlg != -2))
		{
			if((!strcmp(ptr->state->token_buff,"No link")) &&
				busyFlg == -1)
			{
				ptr->counter = 0;
/*
printf("SMIUIRTL - %s actionList Calling User bad\n", ptr->obj_name);
*/
				ptr->user_rout(&ptr->id, &ptr->par);
			}
		}
	}
	else
	{
		ptr->counter |= 0x4;
/*
printf("SMIUIRTL - Received %s actionList %d %08x\n", ptr->obj_name, *size, ptr->counter);
*/
		if((ptr->counter & 0x7) == 0x7)
		{
			ptr->counter = 0;
/*
printf("SMIUIRTL - %s actionList Calling User good\n", ptr->obj_name);
*/
			ptr->user_rout(&ptr->id, &ptr->par);
		}
	}
}
/*===========================================================================================*/
int smiui_book_statechange(obj, user_rout, par)
char *obj;
void (*user_rout)();
dim_long par;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];
	/*int index, state_id, busy_id;*/ int ptr_id;
	STATECHANGE *ptr;
	
	BUSY busyData;
	

	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	ptr = malloc(sizeof(STATECHANGE));
	ptr_id = id_get(ptr, SRC_USR);
	Booking_statechange = 1;
	strcpy(ptr->obj_name,tmp_obj);
	ptr->user_rout = user_rout;
	ptr->par = par; 
	ptr->state = malloc(sizeof(TOKEN_STR));
	ptr->state->size = 0;
	init_token(ptr->state);
	ptr->actions = malloc(sizeof(TOKEN_STR));
	ptr->actions->size = 0;
	init_token(ptr->actions);
	ptr->state_id = 0;
	ptr->action_id = 0;
	ptr->busy_id = 0;
	ptr->counter = 0;
	busyData.state = -2;
	busyData.action = malloc(1);
	busyData.action[0] = '\0';
	ptr->busyData = busyData;
	ptr->id = ptr_id;
	get_service_name(tmp_obj,serv_name,0,0);
	ptr->state_id = dic_info_service(serv_name, MONITORED, 0, 
		0,0, 
		state_change_rout, (dim_long)ptr, "No link", 8);
	get_service_name(tmp_obj,serv_name,0,0);
	strcat(serv_name,"/ACTIONS&PARS");
	ptr->action_id = dic_info_service(serv_name, MONITORED, 0,
		0,0, 
		actions_change_rout, (dim_long)ptr, "No link", 8);
	get_service_name(tmp_obj,serv_name,0,0);
	strcat(serv_name,"/BUSY");
	ptr->busy_id = dic_info_service(serv_name, MONITORED, 0, 
		0, 0, 
		busy_change_rout, (dim_long)ptr, &No_link, 4);
	if(!ptr->busy_id)
	{
		free(ptr);
		id_free(ptr_id, SRC_USR);
		ptr_id = 0;
	}
	Booking_statechange = 0;
	return(ptr_id);
}

void set_change_rout(ptr_ptr, addr, size)
SETCHANGE **ptr_ptr;
int *size;
char *addr;
{
	SETCHANGE *ptr;
	TOKEN_STR *objsp;

	ptr = *ptr_ptr;
	objsp = ptr->objs;
	if(*size > objsp->size)
	{
		if(objsp->size)
			free(objsp->token_buff);
		objsp->token_buff = malloc(*size);
		objsp->size = *size;
	}
	memcpy(objsp->token_buff,addr,*size);
	init_token(objsp);
	ptr->user_rout(&ptr->id, &ptr->par);
}

int smiui_book_objectsetchange(obj, user_rout, par)
char *obj;
void (*user_rout)();
dim_long par;
{
	char serv_name[MAX_NAME], domain[MAX_NAME], tmp_obj[MAX_NAME], *aux;
	/*int index, state_id, busy_id; */ int ptr_id;
	DOMAIN_INFO *domain_ptr;
	SETCHANGE *ptr;

	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	strcpy(domain, tmp_obj);
	aux = strstr(domain,"::");
	*aux = 0;
	aux += 2;
	domain_ptr = find_domain(domain);
	if(!domain_ptr)
	{
		smiui_connect_domain(domain);
		domain_ptr = find_domain(domain);
	}
	
	ptr = malloc(sizeof(SETCHANGE));
	ptr_id = id_get(ptr, SRC_USR);
	strcpy(ptr->obj_name,tmp_obj);
	ptr->user_rout = user_rout;
	ptr->par = par;
	ptr->domain_ptr = domain_ptr;
	ptr->objs = malloc(sizeof(TOKEN_STR));
	ptr->objs->size = 0;
	ptr->id = ptr_id;
	init_token(ptr->objs);
	get_service_name(tmp_obj,serv_name,0,1);
	ptr->objs_id = dic_info_service(serv_name, MONITORED, 0, 
		    0,0, 
		set_change_rout, (dim_long)ptr, "No link", 8);
	return(ptr_id);
}

int smiui_book_alloc_statechange(domain, user_rout, par)
char *domain;
void (*user_rout)();
dim_long par;
{
	char serv_name[MAX_NAME];

	strcpy(serv_name,domain);
	capitalize(serv_name);
	strcat(serv_name,"::");
	strcat(serv_name,"&ALLOC");
	return(smiui_book_statechange(serv_name,user_rout,par));
		
}
                     
int smiui_cancel_statechange(id)
int id;
{
STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id, SRC_USR);
	if(ptr->state_id)
	{
		dic_release_service(ptr->state_id);
		dic_release_service(ptr->action_id);
		dic_release_service(ptr->busy_id);
		ptr->state_id = 0;
	}
	if(ptr->state->size)
		free(ptr->state->token_buff);
	free(ptr->state);
	if(ptr->actions->size)
		free(ptr->actions->token_buff);
	free(ptr->actions);
	if(!Booking_statechange)
	{
		free(ptr);
		id_free(id, SRC_USR);
	}
	return(1);
}

int smiui_cancel_objectsetchange(id)
int id;
{
SETCHANGE *ptr;

	ptr = (SETCHANGE *)id_get_ptr(id,SRC_USR); 
	dic_release_service(ptr->objs_id);
	free(ptr->objs);
	free(ptr);
	id_free(id, SRC_USR);
	return(1);
}

int smiui_cancel_alloc_statechange(id)
int id;
{

	smiui_cancel_statechange(id);
	return(1);
}
                     

int smiui_get_alloc_state(id, busyflg, state)
int id;
int *busyflg;
char *state;
{
	int n_actions;

	return( smiui_get_state(id, busyflg, state, &n_actions));
}

/*======================================================================================*/
int smiui_get_action_in_progress_size(int id, int *size)
{
	STATECHANGE *ptr;
	BUSY busyData;
	int busyFlg;

    ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	
	busyData = ptr->busyData;
	busyFlg = busyData.state;
	
	if(busyFlg == 1)
		*size = strlen(busyData.action);
	else
		*size = 0;
	return(1);
}

int smiui_get_action_in_progress(id, action)
int id;
char *action;
{
/*	int index = 0; */
	STATECHANGE *ptr;
	BUSY busyData;
	int busyFlg;

    ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	
	busyData = ptr->busyData;
	busyFlg = busyData.state;
	
	if(busyFlg == 1)
		strcpy(action,busyData.action);
	else
		action[0] = '\0';
	return(1);
}

int smiui_get_name(id, name)
int id;
char *name;
{
STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	strcpy(name,ptr->obj_name);
	return(1);
}

int smiui_get_setname(id, name)
int id;
char *name;
{
SETCHANGE *ptr;

	ptr = (SETCHANGE *)id_get_ptr(id,SRC_USR); 
	strcpy(name,ptr->obj_name);
	return(1);
}
/*=========================================================================================*/
int smiui_get_state(id, busyflg, state, n_actions)
int id;
int *busyflg;
char *state;
int *n_actions;
{
/*	int index = 0;  */
	char *aux_ptr;
	STATECHANGE *ptr;
	BUSY busyData;
	int busyFlg;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	busyData = ptr->busyData;
	busyFlg = busyData.state;
	init_token(ptr->state);
	init_token(ptr->actions);
	*busyflg = busyFlg;
	if(*busyflg == No_link)
	{
		state[0] = '\0';
		*n_actions = 0;
		*busyflg = 0;
		return(0);
	}
	get_token(ptr->state, &aux_ptr);
	strcpy(state,aux_ptr);
	*n_actions = 0;
	
	if(get_token(ptr->actions, &aux_ptr))
	{
		(*n_actions)++;
    	while(get_token(ptr->actions, &aux_ptr))
		{
			if(cmp_token(aux_ptr,"|"))
				(*n_actions)++;
		}
	}
	init_token(ptr->actions);
	return(1);
}

int smiui_get_first_action(id, action, n_pars)
int id;
char *action;
int *n_pars;
{
STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	init_token(ptr->actions);
	return(smiui_get_next_action(id, action, n_pars));
}

int count_pars(ptr)
STATECHANGE *ptr;
{
	char *aux_ptr;
	int n_pars = 0;

	push_token(ptr->actions);
	while(get_token(ptr->actions,&aux_ptr))
	{
		if(cmp_token(aux_ptr,"/"))
		{
			n_pars++;
		}
		if(cmp_token(aux_ptr,"|"))
			break;
	}
	pop_token(ptr->actions);
	return(n_pars);
}	

int smiui_get_next_action(id, action, n_pars)
int id;
char *action;
int *n_pars;
{
	/*int i;*/
	char *aux_ptr;
	STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	if(first_token(ptr->actions))
	{
		if(get_token(ptr->actions,&aux_ptr))
		{
			strcpy(action, aux_ptr);
			*n_pars = count_pars(ptr);
			return(1);
		}
	}
	else
	{
		while(get_token(ptr->actions,&aux_ptr))
		{
			if(cmp_token(aux_ptr,"|"))
			{
				get_token(ptr->actions,&aux_ptr);
				strcpy(action, aux_ptr);
				*n_pars = count_pars(ptr);
				return(1);
			}
		}
	}
	return(0);
}

/*int count_pars(ptr)
STATECHANGE *ptr;
{
	char *aux_ptr;
	int n_pars = 0;

	push_token(ptr->actions);
	while(get_token(ptr->actions,&aux_ptr))
	{
		if(cmp_token(aux_ptr,"/"))
		{
			n_pars++;
		}
		if(cmp_token(aux_ptr,"|"))
			break;
	}
	pop_token(ptr->actions);
	return(n_pars);
}*/

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
				if(*type == SMI_STRING)
				{
					*size = smixx_unescape(NULL,aux_ptr,0);
					if(aux_ptr[0] == '"')
						*size = smixx_unescape(NULL,aux_ptr,0);
					else
					{
						while(get_token(ptr,&aux_ptr))
						{
							if(cmp_token(aux_ptr,"/"))
							{
								break;
							}
							if(cmp_token(aux_ptr,"|"))
							{
								break;
							}
							else
								*size += smixx_unescape(NULL,aux_ptr,0);
						}
					}
				}
				else if(*type == SMI_INTEGER)
				{
					*size = sizeof(int);
				}
				else
				{
					*size = sizeof(double);
				}
			}
			else
				*size = 0;
			pop_token(ptr);
			return(1);
		}
		if(cmp_token(aux_ptr,"|"))
		{
			break;
		}
	}
	pop_token(ptr);
	return(0);
}

int smiui_get_next_param(id, par, type, def_value_size)
int id;
char *par;
int *type, *def_value_size;
{
	STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	return(get_next_param(ptr->actions,par, type, def_value_size));
}

int smiui_get_next_obj_param(id, par, type, value_size)
int id;
char *par;
int *type, *value_size;
{
	STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	return(get_next_param(ptr->state, par, type, value_size));
}
/*
typedef struct {
	char *token_buff;
	int size;
	char *token_ptr;
	char *curr_token_ptr;
}TOKEN_STR;
*/
static int get_param_value(ptr, val)
TOKEN_STR *ptr;
void *val;
{
	char *strval;
	int nout;
	char *aux_ptr;
	int type, ret = 0, ret1;
	int* pintval; double* pdoubleval;
	
	pintval = val; pdoubleval = val;

	type = SMI_STRING;
	push_token(ptr);
	ret1 = get_token(ptr,&aux_ptr);
	if(!ret1)
	{
		pop_token(ptr);
		return 0;
	}
	if(cmp_token(aux_ptr,"("))
	{
		get_token(ptr,&aux_ptr);
		type = check_type(aux_ptr);
		get_token(ptr,&aux_ptr);
	}
	else
		pop_token(ptr);

	ret1 = get_token(ptr,&aux_ptr);
	if(!ret1)
	{
		pop_token(ptr);
		return 0;
	}
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
					while(get_token(ptr,&aux_ptr))
					{
						if(cmp_token(aux_ptr,"/"))
						{
							break;
						}
						if(cmp_token(aux_ptr,"|"))
						{
							break;
						}
						else
							strval = (char*) val;
							nout = smixx_unescape(NULL,aux_ptr,0);
							nout = smixx_unescape(&strval[strlen(strval)],aux_ptr,nout);
					}
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

int smiui_get_param_default_value(id, val)
int id;
void *val;
{
	STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR); 
	return(get_param_value(ptr->actions, val));
}

int smiui_get_obj_param_value(id, val)
int id;
void *val;
{
	STATECHANGE *ptr;

	ptr = (STATECHANGE *)id_get_ptr(id,SRC_USR);
	return(get_param_value(ptr->state, val));
}


int smiui_get_first_object_in_set(id, object)
int id;
char *object;
{
SETCHANGE *ptr;

	ptr = (SETCHANGE *)id_get_ptr(id,SRC_USR); 
	init_token(ptr->objs);
	return(smiui_get_next_object_in_set(id, object));
}

int smiui_get_next_object_in_set(id, object)
int id;
char *object;
{
	/*int i;*/
	char *aux_ptr;
	SETCHANGE *ptr;

	ptr = (SETCHANGE *)id_get_ptr(id,SRC_USR);
	strcpy(object,ptr->domain_ptr->domain);
	strcat(object,"::");
	if(first_token(ptr->objs))
	{
		if(get_token(ptr->objs,&aux_ptr))
		{
			strcat(object, aux_ptr);
			push_token(ptr->objs);
			return(1);
		}
	}
	else
	{
		while(get_token(ptr->objs,&aux_ptr))
		{
			if(cmp_token(aux_ptr,"|"))
			{
				get_token(ptr->objs,&aux_ptr);
				strcat(object, aux_ptr);
				push_token(ptr->objs);
				return(1);
			}
		}
	}
	return(0);
}

void cmnd_rout(tag, res)
int *tag;
int *res;
{
	tag = tag;
	Wake_up_cmd = (*res)+1;
#ifdef VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif
}

int smiui_send_command_wait(obj, cmnd)
char *obj, *cmnd;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];
/*	char obj_aux[MAX_NAME];
	char *ptr_domain, *ptr_obj;
	int ret; */
	
	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	capitalize_sel(cmnd);
	get_service_name(tmp_obj,serv_name,0,0);

	strcat(serv_name,"/CMD");
/*
	printf("Sending %s to %s\n",cmnd,serv_name);
*/
	Wake_up_cmd = 0;
	dic_cmnd_callback(serv_name, cmnd, strlen(cmnd)+1, cmnd_rout, 0);
	while(!Wake_up_cmd)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	return(Wake_up_cmd - 1);
}

int smiui_ep_send_command_wait(obj, cmnd)
char *obj, *cmnd;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];

	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	capitalize_sel(cmnd);
	get_service_name(tmp_obj,serv_name,1,0);
	strcat(serv_name,"/CMD");
/*
	printf("Sending %s to %s\n",cmnd,serv_name);
*/
	Wake_up_cmd = 0;
	dic_cmnd_callback(serv_name, cmnd, strlen(cmnd)+1, cmnd_rout, 0);
	while(!Wake_up_cmd)
	{
#ifdef VMS
		sys$hiber();
#else
		dim_wait();
#endif
	}
	return(Wake_up_cmd - 1);
}

int smiui_send_command(obj, cmnd)
char *obj, *cmnd;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];
/*	char obj_aux[MAX_NAME];
	char *ptr_domain, *ptr_obj;
	int ret;*/
			
	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	capitalize_sel(cmnd);
	get_service_name(tmp_obj,serv_name,0,0);

	strcat(serv_name,"/CMD");
/*
	printf("Sending %s to %s\n",cmnd,serv_name);
*/
	dic_cmnd_service(serv_name, cmnd, strlen(cmnd)+1);
	return(1);
}

int smiui_ep_send_command(obj, cmnd)
char *obj, *cmnd;
{
	char serv_name[MAX_NAME], tmp_obj[MAX_NAME];

	strcpy(tmp_obj, obj);
	capitalize(tmp_obj);
	capitalize_sel(cmnd);
	get_service_name(tmp_obj,serv_name,1,0);
	strcat(serv_name,"/CMD");
/*
	printf("Sending %s to %s\n",cmnd,serv_name);
*/
	dic_cmnd_service(serv_name, cmnd, strlen(cmnd)+1);
	return(1);
}

int smiui_allocate(domain)
char *domain;
{
	char serv_name[MAX_NAME];  /* cmnd[MAX_NAME];*/
/*	int ret, cmnd_id;*/
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);
	sprintf(serv_name,"SMI/%s/&ALLOC/CMD",tmp_domain);
	dic_cmnd_service(serv_name, "ALLOCATE", 9);
	return(1);
}

int smiui_allocate_to(domainA, domainB)
char *domainA, *domainB;
{
	char serv_name[MAX_NAME], cmnd[MAX_NAME];
	char tmp_domainA[MAX_NAME], tmp_domainB[MAX_NAME];

	strcpy(tmp_domainA, domainA);
	capitalize(tmp_domainA);
	strcpy(tmp_domainB, domainB);
	capitalize(tmp_domainB);

	sprintf(serv_name,"SMI/%s::%s/&ALLOC/CMD",tmp_domainA,tmp_domainB);
	sprintf(cmnd,"ALLOCATE/ID=%s",tmp_domainB);
	dic_cmnd_service(serv_name, cmnd, strlen(cmnd)+1);
	return(1);
}

int smiui_release(domain)
char *domain;
{
	char serv_name[MAX_NAME]; /* cmnd[MAX_NAME];*/
/*	int ret, cmnd_id; */
	char tmp_domain[MAX_NAME];

	strcpy(tmp_domain, domain);
	capitalize(tmp_domain);

	sprintf(serv_name,"SMI/%s/&ALLOC/CMD",tmp_domain);
	dic_cmnd_service(serv_name, "RELEASE", 8);
	return(1);
}

int smiui_release_from(domainA,domainB)
char *domainA, *domainB;
{
	char serv_name[MAX_NAME]; /* cmnd[MAX_NAME];*/
/*	int ret, cmnd_id; */
	char tmp_domainA[MAX_NAME], tmp_domainB[MAX_NAME];

	strcpy(tmp_domainA, domainA);
	capitalize(tmp_domainA);
	strcpy(tmp_domainB, domainB);
	capitalize(tmp_domainB);

	sprintf(serv_name,"SMI/%s::%s/&ALLOC/CMD",tmp_domainA,tmp_domainB);
	dic_cmnd_service(serv_name, "RELEASE", 8);
	return(1);
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
int token_buff_len;

	if(!ptr->curr_token_ptr)
	{
		ptr->curr_token_ptr = ptr->token_buff;
	}
	token_buff_len = strlen(ptr->token_buff);
	if(Actions_size < token_buff_len+1)
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

static int first_token(ptr)
TOKEN_STR *ptr;
{
	if(ptr->token_ptr == ptr->token_buff)
		return(1);
	return(0);
}
#define SMI_STRING_INITIAL_SIZE 256 
/* The following structure serves as a container for the pointer to the command string
   string buffer and the number of bytes allocated for the buffer (almost like
   the VMS descriptor). An another advantage is that as the buffer is extended
   and consequently the pointer to it is changing, the pointer to the structure
   itself remains constant
						B.Franek	June 2007    
   */
typedef struct {
	int magicWord;	/* =    0x01010101   and serves to identify this type
					structure.  Intended for future use */
	int size; 	/* number of bytes allocated in the command string 
			   buffer */
	char* pString;	/* pointer to the command string buffer*/
} commandString;
/*============================================================================*/
char* request_space(commandString* pComm, int nchar)
/*-----------------------------------------------------------------------------
Input arguments :	commandString* pComm ...pointer to the command string
						structure (obtained from
						smiui_create_command)
			int nchar ...		number of characters to be
						appended to the command string
This function is for an exclusive use of smiui_add_param_to_command.
It checks whether there is enough space left in the command string buffer 
for the extra nchar characters. If not, it will first extend the buffer by
at least SMI_STRING_INITIAL_SIZE. It returns the pointer to the terminating
zero of the current command string. This means that writing nchar characters
starting at the returned pointer will append them to the existing string and
there will be enough space for them.	
						B.Franek	June 2007 
-----------------------------------------------------------------------------*/
{
	int size;		/* current buffer allocation */
	int used;		/* no of bytes used in the buffer (length of
				   the current command*/
	int freeSpace;		/* no of bytes left unused in the buffer */
	
	int extension;		/* no of bytes used to extend the buffer */
	int newSize;		/* extended size of the buffer */
	char* pStringNew;	/* pointer to the new string (buffer) */
	
	size = pComm->size;
	used = strlen(pComm->pString) + 1;
	freeSpace = size - used ;
	
	if ( freeSpace > nchar )
	{
		return pComm->pString + used -1; /* pointer to the last zero */
	}
/* need to extended */	
	if ( nchar > SMI_STRING_INITIAL_SIZE )
	{
		extension = nchar;
	}
	else
	{
		extension = SMI_STRING_INITIAL_SIZE;
	}
	newSize = size + extension;
	pStringNew = malloc(newSize);
	strcpy(pStringNew,pComm->pString);
	free(pComm->pString);
	
	pComm->size = newSize;
	pComm->pString = pStringNew;
	
	return pComm->pString + used -1;
}
/*==============================================================================*/
void* smiui_create_command(char* action)
/*------------------------------------------------------------------------------
 The functions smiui_send_command... that are used to dispatch
 commands to remote SMI objects accept 2 arguments: 1) object name
 						    2)command string
 In its simplest form, command string is just action name.
 However if the action requires parameters, then these have to
 be built into the command string. Functions:
 	smiui_create_command (the first call) & smiui_add_param_to_command
are used to accomplish this task.
smiui_create_command 	a) creates structure 'commandString'
			b) creates buffer of sufficient length and
				stores its size and its pointer in 
				the structure.
			c) stores the action name (supplied as an argument) 
				in the buffer 
			d) returns the pointer to the structure created in a)
			which is then used as the command identifier in the
			following calls.
			
				B.Franek			June 2007 
------------------------------------------------------------------------------*/
{
	commandString* pComm;
	int actionLen;
	int size;
	
	actionLen = strlen(action);
	if ( (actionLen+1) > SMI_STRING_INITIAL_SIZE )
	{
		size = actionLen+1;
	}
	else
	{
		size = SMI_STRING_INITIAL_SIZE;
	}
	
	
	pComm = malloc(sizeof(commandString));
	
	pComm->magicWord = 0x01010101;
	pComm->size = size;
	pComm->pString = malloc(size);
	strcpy(pComm->pString,action);
	
	return (void*) pComm;
}
/*======================================================================================*/
int smiui_add_param_to_command(void* id, char* name, void* value, int type)
/*------------------------------------------------------------------------------
After smiui_create_command function was called once, then this function is
called for every parameter required by the action and adds the parameter 
contribution (see below) to the command string.
Input arguments :
	void* id	the identifier returned by smiui_create_command
	char* name	parameter name
	void* value	pointer to the parameter value
	int type	SMI_STRING, SMI_INTEGER or SMI_FLOAT
	
				B.Franek			June 2007 
------------------------------------------------------------------------------*/
{
	char* ptrComEnd;	/* pointer to the terminating zero of the current
				command string. Returned by 'request_space'
				function */

	int nchar;		/* for string parameters, this is the size of
				   the escaped string in byte */
	int intValue;		/* for integer parameters, this is the copy of the
				   value  */
	double floatValue;	/* for floating point parameters, this is the copy
				   of the value */
	commandString* pComm;	/* void* id is properly casted into pComm */
	
	int addSize;		/* size of the new parameter's contribution to 
			the command string.
		parameter conributions are:
		for int:    /<name>(I)=<string-representation-of-the-int-value>
		for float:  /<name>(F)=<string-representation-of-the-float-value>
		for string: /<name>(S)=<escaped-ascii-string>
		
		*/
	
	pComm = (commandString*) id;
	
	
	if (!pComm)
	{
		printf(" smi_add_par : command not initialised\n");
		return (0);
	}
	
	if ( type == SMI_STRING )
	{
		nchar = smixx_escape(NULL,value,0);
		addSize = 1+strlen(name)+4+nchar;
		ptrComEnd = request_space(pComm, addSize);
		strcpy(ptrComEnd,"/");
		strcat(ptrComEnd,name);
		strcat(ptrComEnd,"(S)=");
		nchar = smixx_escape(ptrComEnd+strlen(ptrComEnd),value,nchar);
	}
	else if ( type == SMI_INTEGER )
	{
		addSize = 1+strlen(name)+4+32;
		ptrComEnd = request_space(pComm, addSize);
		strcpy(ptrComEnd,"/");
		strcat(ptrComEnd,name);
		strcat(ptrComEnd,"(I)=");
		intValue = *((int*)value);
		sprintf(ptrComEnd+strlen(ptrComEnd),"%d",intValue);
	}
	else if ( type == SMI_FLOAT )
	{
		addSize = 1+strlen(name)+4+32;
		ptrComEnd = request_space(pComm, addSize);
		strcpy(ptrComEnd,"/");
		strcat(ptrComEnd,name);
		strcat(ptrComEnd,"(F)=");
		floatValue = *((double*)value);
		sprintf(ptrComEnd+strlen(ptrComEnd),"%e",floatValue);
	}
	else
	{
		printf("smiui_add_par_to_command : unknown type %d \n",type);
		return 0;
	}
			
	return 1;
		
}
/*=============================================================================*/
int smiui_delete_command(void* id)
/*------------------------------------------------------------------------------
Input arguments :
	void* id	the identifier returned by smiui_create_command
	
						B.Franek	June 2007 
-----------------------------------------------------------------------------*/
{
	commandString* pComm;
	
	pComm = (commandString*) id;
	
	free(pComm->pString);
	free(pComm);
	return 1;
}
/*============================================================================*/
char* smiui_get_command_string(void* id)
/*------------------------------------------------------------------------------
Input arguments :
	void* id	the identifier returned by smiui_create_command
	
Returns pointer to the command string
						B.Franek	June 2007 
------------------------------------------------------------------------------*/
{
	commandString* pComm;
	
	pComm = (commandString*) id;
	
	return pComm->pString;
}

#ifdef VMS
FCALLSCFUN3(INT, smiui_current_state, SMIUI_CURRENT_STATE,
				 smiui_current_state,
                 STRING,PSTRING,PSTRING)
FCALLSCFUN3(INT, smiui_book_statechange, SMIUI_BOOK_STATECHANGE,
				 smiui_book_statechange,
                 STRING,PVOID,INT)
FCALLSCFUN1(INT, smiui_cancel_statechange, SMIUI_CANCEL_STATECHANGE,
				 smiui_cancel_statechange,
                 INT)
FCALLSCFUN2(INT, smiui_get_name, SMIUI_GET_NAME,
				 smiui_get_name,
                 INT,PSTRING)
FCALLSCFUN4(INT, smiui_get_state, SMIUI_GET_STATE,
				 smiui_get_state,
                 INT,PINT,PSTRING,PINT)
FCALLSCFUN1(INT, smiui_connect_domain, SMIUI_CONNECT_DOMAIN,
				 smiui_connect_domain,
                 STRING)
FCALLSCFUN3(INT, smiui_book_connect_domain, SMIUI_BOOK_CONNECT_DOMAIN,
				 smiui_book_connect_domain,
                 STRING,PVOID,INT)
FCALLSCFUN1(INT, smiui_cancel_connect_domain, SMIUI_CANCEL_CONNECT_DOMAIN,
				 smiui_cancel_connect_domain,
                 STRING)
FCALLSCFUN2(INT, smiui_get_next_object, SMIUI_GET_NEXT_OBJECT,
				 smiui_get_next_object,
                 STRING,PSTRING)
FCALLSCFUN3(INT, smiui_get_next_action, SMIUI_GET_NEXT_ACTION,
				 smiui_get_next_action,
                 INT,PSTRING,PINT)
FCALLSCFUN2(INT, smiui_send_command, SMIUI_SEND_COMMAND,
				 smiui_send_command,
                 STRING,STRING)
FCALLSCFUN2(INT, smiui_get_next_attribute, SMIUI_GET_NEXT_ATTRIBUTE,
				 smiui_get_next_attribute,
                 STRING,PSTRING)
#endif


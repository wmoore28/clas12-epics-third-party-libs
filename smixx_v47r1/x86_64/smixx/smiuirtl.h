#ifndef __SMIUIRTLDEFS
#define __SMIUIRTLDEFS

#define MAX_NAME 		132

#ifndef solaris
#ifdef sun
#define solaris
#endif
#endif

#include <dic.h>

/*
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <dim_common.h>
#define pause() Sleep(10000)
#endif
*/

#ifndef _PROTO
#ifndef OSK		/* Temorary hack */
#	if defined(__cplusplus) /* || (__STDC__ == 1) || defined(_ANSI_EXT) || defined(ultrix) */
#		define	_PROTO(func,param)	func param
#	else
#		define _PROTO(func,param)	func ()
#	endif
#else
#	define _PROTO(func,param)	func ()
#endif
#ifdef WIN32
#ifdef SMIUILIB
#	define _PROTOE(func,param) __declspec(dllexport) _PROTO(func,param)
/*#	define DllExp __declspec(dllexport)
*/
#else
#	define _PROTOE(func,param) __declspec(dllimport) _PROTO(func,param)
/*#	define DllExp __declspec(dllimport) 
*/
#endif
#else
#	define _PROTOE(func,param) _PROTO(func,param)
/*#	define DllExp
*/
#endif
#endif

#ifdef __VMS
#define VMS
#endif

#ifdef VMS

/* part for CFORTRAN */

#define smiui_current_state		smiui_current_state_
#define smiui_book_statechange	smiui_book_statechange_
#define smiui_cancel_statechange	smiui_cancel_statechange_
#define smiui_get_name			smiui_get_name_
#define smiui_get_state			smiui_get_state_
#define smiui_connect_domain	smiui_connect_domain_
#define smiui_book_connect_domain	smiui_book_connect_domain_
#define smiui_cancel_connect_domain	smiui_cancel_connect_domain_
#define smiui_get_next_object	smiui_get_next_object_
#define smiui_get_next_attribute	smiui_get_next_attribute_
#define smiui_get_next_action	smiui_get_next_action_
#define smiui_send_command		smiui_send_command_

#ifndef SMIUILIB
#define SMIUI_CURRENT_STATE		smiui_current_state_
#define SMIUI_BOOK_STATECHANGE	smiui_book_statechange_
#define SMIUI_CANCEL_STATECHANGE	smiui_cancel_statechange_
#define SMIUI_GET_NAME			smiui_get_name_
#define SMIUI_GET_STATE			smiui_get_state_
#define SMIUI_CONNECT_DOMAIN	smiui_connect_domain_
#define SMIUI_BOOK_CONNECT_DOMAIN	smiui_book_connect_domain_
#define SMIUI_CANCEL_CONNECT_DOMAIN	smiui_cancel_connect_domain_
#define SMIUI_GET_NEXT_OBJECT	smiui_get_next_object_
#define SMIUI_GET_NEXT_ATTRIBUTE	smiui_get_next_attribute_
#define SMIUI_GET_NEXT_ACTION	smiui_get_next_action_
#define SMIUI_SEND_COMMAND		smiui_send_command_
#endif

#endif

#ifndef SMI_PAR_TYPES
#ifndef WIN32
typedef enum { STRING, INTEGER, FLOAT } SHORT_PAR_TYPES;
#endif
typedef enum { SMI_STRING, SMI_INTEGER, SMI_FLOAT } PAR_TYPES;
#define SMI_PAR_TYPES
#endif

_PROTOE( int smiui_current_state, 	(char *obj, char *state, char* action) );
_PROTOE( int smiui_send_command, 	(char *obj, char *cmnd) );
_PROTOE( int smiui_ep_send_command, 	(char *obj, char *cmnd) );
_PROTOE( int smiui_send_command_wait, 	(char *obj, char *cmnd) );
_PROTOE( int smiui_ep_send_command_wait, 	(char *obj, char *cmnd) );
_PROTOE( int smiui_number_of_objects, 	(char *domain) );
_PROTOE( int smiui_connect_domain, 	(char *domain) );
_PROTOE( int smiui_book_connect_domain, 	
	(char *domain, void (*comm_handler)(...), long par) );
_PROTOE( int smiui_cancel_connect_domain, 	
	(char *domain) );
_PROTOE( int smiui_shutdown_domain, 	(char *domain) );
_PROTOE( int smiui_check_proxy, 	(char *proxy) );
_PROTOE( int smiui_kill, 	        (char *server) );
_PROTOE( int smiui_get_next_object, 	(char *domain, char *object) );
_PROTOE( int smiui_get_next_attribute, 	(char *object, char *attribute) );
_PROTOE( int smiui_book_statechange, 	
	(char *object, void (*comm_handler)(...), long par) );
_PROTOE( int smiui_cancel_statechange, 	(int id) );
_PROTOE( int smiui_get_name, 	
	(int id, char *name) );
_PROTOE( int smiui_get_state, 	
	(int id, int *busy_flg, char *state, int *n_actions) );
_PROTOE( int smiui_get_action_in_progress, 	
	(int id, char *action) );
_PROTOE( int smiui_get_first_action, 	
	(int id, char *action, int *n_pars) );
_PROTOE( int smiui_get_next_action, 	
	(int id, char *action, int *n_pars) );
_PROTOE( int smiui_get_next_param, 	
	(int id, char *par, int *type, int *size) );
_PROTOE( int smiui_get_param_default_value, 	
	(int id, void *value) );
_PROTOE( int smiui_get_next_obj_param, 	
	(int id, char *par, int *type, int *size) );
_PROTOE( int smiui_get_obj_param_value, 	
	(int id, void *value) );
_PROTOE( int smiui_book_alloc_statechange, 	
	(char *domain, void (*comm_handler)(...), long par) );
_PROTOE( int smiui_cancel_alloc_statechange, 	(int id) );
_PROTOE( int smiui_get_alloc_state, 	
	(int id, int *busy_flg, char *state) );
_PROTOE( int smiui_allocate, 	(char *domain) );
_PROTOE( int smiui_release, 	(char *domain) );
_PROTOE( int smiui_allocate_to, 	(char *domainA, char *domainB) );
_PROTOE( int smiui_release_from, 	(char *domainA, char *domainB) );

_PROTOE( int smiui_get_next_objectset, 	(char *domain, char *objectset) );
_PROTOE( int smiui_book_objectsetchange, 	
	(char *object, void (*comm_handler)(...), long par) );
_PROTOE( int smiui_cancel_objectsetchange, 	(int id) );
_PROTOE( int smiui_get_first_object_in_set, 	
	(int id, char *obj) );
_PROTOE( int smiui_get_next_object_in_set, 	
	(int id, char *obj) );
_PROTOE( int smiui_get_setname, 	
	(int id, char *objset) );
_PROTOE( int smiui_number_of_objectsets, 	(char *domain) );

_PROTOE( void* smiui_create_command,	(char* action) );

_PROTOE( int smiui_add_param_to_command, 
	(void* id, char* name, void* value, int type) );
_PROTOE( int smiui_delete_command, 	(void* pointer) );

_PROTOE( char* smiui_get_command_string, 	(void* id) );

_PROTOE( int smiui_get_action_in_progress_size, 	
	(int id, int *size) );

#endif


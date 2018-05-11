#ifndef __SMIRTLDEFS
#define __SMIRTLDEFS

#ifndef solaris
#ifdef sun
#define solaris
#endif
#endif

#include <dic.h>
#include <dis.h>

/*
#ifdef WIN32
#include <windows.h>
#include <process.h>

#include <dim_common.h>
#define pause() Sleep(10000)
#endif
*/

#ifndef _PROTO
/*
#ifndef OSK	  
#	if (__STDC__ == 1)|| defined(_ANSI_EXT)  || defined(__cplusplus)
#		define	_PROTO(func,param)	func param
#	else
#		define _PROTO(func,param)	func ()
#	endif
#else
#	define _PROTO(func,param)	func ()
#endif
*/
#ifndef OSK		/* Temorary hack */
#	if defined(__cplusplus) /* || (__STDC__ == 1) || defined(_ANSI_EXT) || defined(ultrix) */
#		define _PROTO(func,param)	func param
#	else
#		define _PROTO(func,param)	func ()
#	endif
#else
#	define _PROTO(func,param)	func ()
#endif

#ifdef WIN32
#ifdef SMILIB
#	define _PROTOE(func,param) __declspec(dllexport) _PROTO(func,param)
/*
#	define DllExp __declspec(dllexport)
*/
#else
#	define _PROTOE(func,param) __declspec(dllimport) _PROTO(func,param)
/*
#	define DllExp __declspec(dllimport)
*/
#endif
#else
#	define _PROTOE(func,param) _PROTO(func,param)
/*
#	define DllExp
*/
#endif
#endif

#ifdef __VMS
#define VMS
#endif

#ifdef VMS

/* part for CFORTRAN */

#define smi_attach				smi_attach_
#define smi_volatile			smi_volatile_
#define smi_init_for			smi_init_for_
#define smi_associate			smi_associate_
#define smi_set_state			smi_set_state_
#define smi_get_state			smi_get_state_
#define smi_get_substate		smi_get_substate_
#define smi_terminate_command	smi_terminate_command_
#define smi_terminate_action	smi_terminate_action_
#define smi_get_command			smi_get_command_
#define smi_get_commandw		smi_get_commandw_
#define smi_get_commandw_for	smi_get_commandw_for_
#define smi_get_action			smi_get_action_
#define smi_test_action			smi_test_action_
#define smi_get_par_value		smi_get_par_value_
#define smi_get_next_par		smi_get_next_par_
#define smi_set_par				smi_set_par_
#define smi_send_command		smi_send_command_

#ifndef SMILIB
#define SMI_INIT_FOR			smi_init_for_
#define SMI_ASSOCIATE			smi_associate_
#define SMI_SET_STATE			smi_set_state_
#define SMI_GET_STATE			smi_get_state_
#define SMI_TERMINATE_COMMAND	smi_terminate_command_
#define SMI_SEND_COMMAND		smi_send_command_
#endif

#endif

#ifndef SMI_PAR_TYPES
#ifndef WIN32
typedef enum { STRING, INTEGER, FLOAT } SHORT_PAR_TYPES;
#endif
typedef enum { SMI_STRING, SMI_INTEGER, SMI_FLOAT } PAR_TYPES;
#define SMI_PAR_TYPES
#endif

_PROTOE( int smi_attach, 		(char *obj_name, void (*user_ast)()) );
_PROTOE( void smi_volatile, 	() );
_PROTOE( int smi_set_state, 	(char *state) );
_PROTOE( int smi_get_state, 	(char *state, int max_len) );
_PROTOE( int smi_terminate_action, 	(char *state) );
_PROTOE( int smi_get_action, 	(char *cmnd, int *n_pars) );
_PROTOE( int smi_get_command, 	(char *cmnd, int *size) );
_PROTOE( int smi_test_action, 	(char *cmnd) );
_PROTOE( int smi_get_next_par, 	(char *param, int *type, int *size) );
_PROTOE( int smi_get_par_value, (char *param, void *value) );
_PROTOE( int smi_set_par, 		(char *param, void *value, int type) );
_PROTOE( void smi_register_termination,(int (*exit_handler)(...), void *context) );

#endif


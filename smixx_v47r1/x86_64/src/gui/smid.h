#include <Mrm/MrmAppl.h>                /* Motif Toolkit and MRM */
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include "dui_colors.h"
/*
#include <dim.h>
*/
#include <dic.h>
#include <stdlib.h>

#define MAX_NAME 		132
#define MAX_OBJECTS		1024
#define MAX_STATES		50
#define MAX_ACTIONS		50
#define MAX_OBJECT_NAME	32
#define MAX_SUBOBJECTS	50
#define MAX_BUFFER		1024
#define MAX_ATTRS		10
#define MAX_FILE_NAME   256

#define LABEL_FONT "-*-NEW CENTURY SCHOOLBOOK-BOLD-R-*--*-140-*-*-*-*-ISO8859-1"
#define DEFAULT_FONT "-*-TIMES-BOLD-R-*--*-120-*-*-*-*-ISO8859-1"

#define MENU_FONT "-*-TIMES-BOLD-R-*--*-120-*-*-*-*-ISO8859-1"

typedef struct {
	char param[MAX_OBJECT_NAME+1];
	char value_str[MAX_NAME];
	int value_int;
	double value_float;
	int size;
	int type;
} PAR_TYPE;

typedef struct {
    int n_actions;
	char actions[MAX_ACTIONS][MAX_OBJECT_NAME+1];
	PAR_TYPE *params[MAX_ACTIONS];
} ACTION_LIST;

typedef struct {
	int state;
	char action[MAX_BUFFER];
}BUSY;

typedef struct {
	char attributes[MAX_ATTRS][MAX_OBJECT_NAME];
}ATTR_LIST;

typedef struct {
	char file_ident[MAX_FILE_NAME];
	char file_name[MAX_FILE_NAME];
}FILE_TYP;

typedef struct {
	char obj[MAX_NAME];
	char cmd[MAX_NAME];
}CMD_TYP;

typedef struct {
	char name[MAX_OBJECT_NAME];
	char state[MAX_OBJECT_NAME];
	char name_alias[MAX_OBJECT_NAME];
	int created;
	int connecting;
	int visible;
	int statechange_id;
	ACTION_LIST actions;
	PAR_TYPE *params;
	ATTR_LIST *attrs;
	int index_x;
	int index_y;
	Widget obj_id;
	Widget name_id;
	Widget state_id;
	Widget action_id;
	Widget menu_id;
	Widget action_ids[MAX_ACTIONS];
}OBJ;

typedef struct {
	char state[MAX_OBJECT_NAME];
	int color;
}STATE_COL;

STATE_COL State_colors[512];

/* VUIT routines for the user to call */
void s_error();

/* Motif Global variables */
Display         *display;			/* Display variable */
XtAppContext    app_context;		/* application context */
Widget			toplevel_widget;	/* Root widget ID of application */
MrmHierarchy	s_MrmHierarchy;		/* MRM database hierarchy ID */

void smid_exit();
void smid_kill_exit();
void smid_config();
void create_main();
void create_window();
void create_label();
void create_command();
void create_matrix();
void create_color();
void create_alloc_pop();
void create_file_button();
void activate_color();
void alloc_value_changed();
void ok_pop_up();                                                            
void cancel_pop_up();

XmString create_str();
XmFontList util_get_font();
Widget create_selection_dialog();
Widget create_file_selection_dialog();
Widget create_color_dialog();
Widget create_prompt_dialog();
Widget create_param_dialog();
Widget gui_toplevel();
Widget gui_initialize();
void gui_create_main_menu();
Widget gui_create_main_window();






#include <stdio.h>                   
#include <ctype.h>
#include <smiuirtl.h>
#include "smid.h"
/*
#include "utils.h"
*/
#ifndef RE_ENABLE_AST
#define RE_ENABLE_AST   sigset_t set, oset;sigemptyset(&set);sigaddset(&set,SIGIO);sigaddset(&set,SIGALRM);sigprocmask(SIG_UNBLOCK,&set,&oset);

#define RE_DISABLE_AST  sigprocmask(SIG_SETMASK,&oset,0);
#endif
/*
 * Global data
 */

typedef enum {OPEN_FULL, OPEN_CONFIG, SAVE_CONFIG, START_CONFIG, QUIT_CONFIG,
				INS_OBJ, REM_OBJ, COLOR, PARAM, PAR_VIEW, ALLOC, ALLOC_POP,
				PAR_FILE, MAX_CMNDS } CMND_LIST;


int First_time = 1;
Widget Pop_widget_id[MAX_CMNDS] = {0,0,0,0,0,0,0,0,0,0,0};
Widget Cmnds[MAX_CMNDS];
Widget Matrix_id = 0;
Widget Label_id = 0;
Widget Window_id = 0;
Widget Top_id = 0;
Widget No_link_button_id;
Widget Display_color_id;
Widget File_button_id = 0;
OBJ Object_matrix[MAX_OBJECTS/4][4];
OBJ Alloc_object;
int Configuring = 0;
static int N_objects = 0;
static int N_visible_objs;
static int N_connected_objs;
OBJ *Moving = 0;
OBJ *Curr_object;
int Curr_color;
int Curr_color_index;
OBJ *P_obj; 
int P_obj_act_index, P_obj_par_index;
Widget Par_label_ids[3];
int Top_width, Top_height;
static char Domain_name[MAX_NAME];
static char Config_name[132];
static char User[132];
static int Allocate_domain = 0;
static int Read_only = 0;
static FILE_TYP File_list[MAX_ATTRS];
static CMD_TYP Cmd_list[MAX_ATTRS];

static int Matrix_managed = 1;
Widget Alloc_pop_ids[3];

char Display_title[MAX_NAME];

static XmFontList smid_default_font, smid_menu_font, smid_label_font;
/*
#ifndef unix 
static char *db_filename_vec[] =   
{
	"delphi$online:[smixx.gui]smid.uid"
};

static int db_filename_num =
                (sizeof db_filename_vec / sizeof db_filename_vec [0]);

#else
static char *db_filename_vec[1];    
static int db_filename_num;
#endif
*/

/*
 * Forward declarations
 */
/*
 * Names and addresses of callback routines to register with Mrm
 */
/*
static MrmRegisterArg reglist [] = {
{"smid_exit", (caddr_t)smid_exit},
{"smid_kill_exit", (caddr_t)smid_kill_exit},
{"smid_config", (caddr_t)smid_config},
{"create_main", (caddr_t)create_main},
{"create_window", (caddr_t)create_window},
{"create_label", (caddr_t)create_label},
{"create_command", (caddr_t)create_command},
{"create_matrix", (caddr_t)create_matrix},
{"create_color", (caddr_t)create_color},
{"create_alloc_pop", (caddr_t)create_alloc_pop},
{"create_file_button", (caddr_t)create_file_button},
{"activate_color", (caddr_t)activate_color},
{"alloc_value_changed", (caddr_t)alloc_value_changed},
{"ok_pop_up", (caddr_t)ok_pop_up},
{"cancel_pop_up", (caddr_t)cancel_pop_up},
};

static int reglist_num = (sizeof reglist / sizeof reglist[0]);

static MrmRegisterArg identlist [] = {
{"index", (caddr_t)&srvr_index}};
static int identlist_num = (sizeof identlist / sizeof identlist[0]);
*/

/*
 * OS transfer point.  The main routine does all the one-time setup and
 * then calls XtAppMainLoop.
 */

int main(argc, argv)
    int argc;                  /* Command line argument count. */
    char *argv[];                       /* Pointers to command line args. */
{
	int i, j;
	/*
	XEvent event;
	*/
	XtInputMask mask;
	void app_initialize();

	/*
#ifdef linux
	*/
  dim_no_threads();
  /*
#endif
  */
	Domain_name[0] = '\0';
	Config_name[0] ='\0';
	User[0] = '\0';
	Read_only = 0;
	Allocate_domain = 0;
	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'a':
					Allocate_domain = 1;
					break;	
				case 'r':
					Read_only = 1;
					break;	
				case 'w':
					break;	
				case 'd':
					argv[i][1] = 's';
					for(j = 0; j < (int)strlen(argv[i+1]); j++)
						Domain_name[j] = toupper(argv[i+1][j]);
					Domain_name[j] = '\0';
					i++;
					break;	
				case 'c':
					strcpy(Config_name,argv[i+1]);
					i++;
					break;	
				case 'u':
					strcpy(User,argv[i+1]);
					i++;
					break;	
			}
		}
		else
		{
			if(Domain_name[0] == '\0')
			{
				for(j = 0; j < (int)strlen(argv[i]); j++)
					Domain_name[j] = toupper(argv[i][j]);
				Domain_name[j] = '\0';
			}
			else
			{
				strcpy(Config_name,argv[i]);
			}
		}
	}
  toplevel_widget = (Widget)gui_initialize(argc, argv);
  app_initialize();
    /* 
     * Sit around forever waiting to process X-events.  We never leave
     * XtAppMainLoop. From here on, we only execute our callback routines. 
     */
    /*    

	XtAppMainLoop(app_context);
     */

    while(1)
	{
		/*
		{
			DISABLE_AST
			XtAppPending(app_context);
			ENABLE_AST
		}
		XtAppNextEvent(app_context,&event);
		{
			DISABLE_AST
			XtDispatchEvent(&event);
			ENABLE_AST
		}
		*/
		{
			DISABLE_AST
			mask = XtAppPending(app_context);	
			ENABLE_AST
		}
		if(mask)
		{
			DISABLE_AST
			XtAppProcessEvent(app_context, mask);
			ENABLE_AST
		}
		else
		{
			usleep(100000);
		}
		/*		
		if(Set_new_size)
		{
			DISABLE_AST
			  printf("1\n");
			set_something(Top_id,XmNheight,Top_height);
			  printf("2\n");
			set_something(Top_id,XmNwidth,Top_width);
			  printf("3\n");
			Set_new_size = 0;
			XFlush(XtDisplay(Top_id));
			ENABLE_AST
		}
		*/
	       
	}
}

/*
 * All errors are fatal.
 */
void s_error(problem_string)
    char *problem_string;
{
    printf("%s\n", problem_string);
    exit(0);
}

void smid_exit (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  if((w) || (tag) || (reason)){}
	exit(0);
}

void x_start_timer(time, rout, par)
int time;
long par;
void (*rout)();
{
     XtAppAddTimeOut(app_context, time*1000, rout, (XtPointer)par);	
}

void smid_kill_exit (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  if((w) || (tag) || (reason)){}
	smiui_shutdown_domain(Domain_name);
	x_start_timer(5, smid_exit, 0);
}

void smid_config (w, tag, reason)
Widget		w;
int		tag;
unsigned long	*reason;
{
char str[2048], *txt_str;
int index, x_len = 0, y_len = 0;
Widget id,sel_id;
Widget create_button();
void connect_domain();
void get_something();
void set_something();
void delete_matrix_objects();
void change_matrix_objects();
int list_matrix_objects();
void put_popup();

  if((w) || (reason)){}
	switch(tag)
	{
		case OPEN_FULL:
			get_something(Top_id,XmNwidth,&x_len);
			get_something(Top_id,XmNheight,&y_len);
			Top_width = x_len;
			Top_height = y_len;
		    delete_matrix_objects();
			Config_name[0] = '\0';
			connect_domain();
			XtSetSensitive(Cmnds[START_CONFIG], True);
			break;
		case OPEN_CONFIG:
			put_popup(tag,"read_config_file","Selection");
			sel_id = Pop_widget_id[tag];
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
			txt_str = XmTextGetString(id);
			if(!txt_str[0])
			{
				XtFree(txt_str);
				return;
			}
			if(Config_name[0])
				strcpy(str,Config_name);
			else				
				sprintf(str,"%sconfig_%s.dat",txt_str,Domain_name);
			XtFree(txt_str);
			XmTextSetString(id, str);
			XtSetSensitive(Cmnds[START_CONFIG], True);
			break;
		case SAVE_CONFIG:
			put_popup(tag,"write_config_file","Selection");
			sel_id = Pop_widget_id[tag];
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
			txt_str = XmTextGetString(id);
			if(!txt_str[0])
			{
				XtFree(txt_str);
				return;
			}
			sprintf(str,"%sconfig_%s.dat",txt_str,Domain_name);
			XtFree(txt_str);
			XmTextSetString(id, str);
			break;
		case START_CONFIG:
			Configuring = 1;
			change_matrix_objects(1);
			XtSetSensitive(Cmnds[START_CONFIG], False);
			XtSetSensitive(Cmnds[QUIT_CONFIG], True);
			if(N_visible_objs == N_connected_objs)
				XtSetSensitive(Cmnds[INS_OBJ], False);
			else
				XtSetSensitive(Cmnds[INS_OBJ], True);
			if(!N_connected_objs)
				XtSetSensitive(Cmnds[REM_OBJ], False);
			else
				XtSetSensitive(Cmnds[REM_OBJ], True);
			XtSetSensitive(Cmnds[OPEN_FULL], False);
			XtSetSensitive(Cmnds[OPEN_CONFIG], False);
			break;
		case QUIT_CONFIG:
			Configuring = 0;
			change_matrix_objects(0);
			XtSetSensitive(Cmnds[START_CONFIG], True);
			XtSetSensitive(Cmnds[QUIT_CONFIG], False);
			XtSetSensitive(Cmnds[REM_OBJ], False);
			XtSetSensitive(Cmnds[INS_OBJ], False);
			XtSetSensitive(Cmnds[OPEN_FULL], True);
			XtSetSensitive(Cmnds[OPEN_CONFIG], True);
			break;
		case INS_OBJ:
			put_popup(tag,"ins_obj_info"," Objects");
			sel_id = Pop_widget_id[tag];
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_OK_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
			index = list_matrix_objects(id, 0);
			sprintf(str,"Objects NOT displayed\n\n");
			set_something(id,XmNlistItemCount,index);
			set_something(id,XmNlistVisibleItemCount,(index < 20) ? 
				index : 20);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
			set_something(sel_id,XmNlistLabelString,str);
			break;
		case REM_OBJ:
			put_popup(tag,"rem_obj_info"," Objects");
			sel_id = Pop_widget_id[tag];

			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_OK_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
			index = list_matrix_objects(id, 1);
			sprintf(str,"Displayed objects\n\n");
			set_something(id,XmNlistItemCount,index);
			set_something(id,XmNlistVisibleItemCount,
				(index < 20) ? index : 20);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST_LABEL);
			set_something(sel_id,XmNlistLabelString,str);
			break;
		default:
			break;
	}
}

void do_set_size()
{
  void set_something();
  /*  
	Set_new_size = 1;
	XFlush(XtDisplay(Top_id));
  	
       
#ifndef __alpha
  */
		set_something(Top_id,XmNwidth,Top_width);
		set_something(Top_id,XmNheight,Top_height);
		XFlush(XtDisplay(Top_id));
  /*
#endif
*/
}

void do_manage_matrix()
{
	if(!Matrix_managed)
	{
		XtManageChild(Matrix_id);
		Matrix_managed = 1;
		XFlush(XtDisplay(Matrix_id));
	}
}

void do_connect_objects()
{
 	int rows, ind_x, ind_y;
	OBJ *obj;
	int n_conns = 0;
	int get_all_rows();
	void connect_object();

	DISABLE_AST
	do_manage_matrix();
	rows = get_all_rows();
/*
	obj = &Alloc_object;
	if((obj->visible > 0) && (!obj->connecting))
	{
		obj->connecting = 1;
		connect_object(obj);
	}
*/
	for(ind_x = 0; ind_x < 4; ind_x++)
	{
		for(ind_y = 0; ind_y < rows; ind_y++)
		{
			obj = &Object_matrix[ind_y][ind_x];
			if((obj->visible > 0) && (!obj->connecting))
			{
				obj->connecting = 1;
				connect_object(obj);
				n_conns++;
			}
			if(n_conns == 20)
			{
				x_start_timer(1,do_connect_objects,0);
				ENABLE_AST
				return;
			}
		}
	}
/*
	dtq_start_timer(1,do_set_size,0);
*/
	ENABLE_AST
}

void do_create_objects(conn)
int conn;
{
 	int rows, ind_x, ind_y;
	int n_created = 0;
	OBJ *obj;
	int get_all_rows();
	void create_object();

/*
	obj = &Alloc_object;
	if(!obj->created)
	{
		obj->created = 1;
		create_object(obj);
	}
*/
	rows = get_all_rows();
	for(ind_x = 0; ind_x < 4; ind_x++)
	{
		for(ind_y = 0; ind_y < rows; ind_y++)
		{
			obj = &Object_matrix[ind_y][ind_x];
			if((obj->visible >= 0) && (!obj->created))
			{
				obj->created = 1;
				create_object(&Object_matrix[ind_y][ind_x]);
				n_created++;
			}
			if(n_created == 300)
			{
				x_start_timer(1,do_create_objects,conn);
				return;
			}
		}
	}
	if(conn)
		x_start_timer(1,do_connect_objects,0);
	else
		x_start_timer(1,do_manage_matrix,0);
}

extern Pixel rgb_colors[MAX_COLORS];

void create_main (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{

  if((tag) || (reason)){}
	Top_id = w;
}

void app_initialize()
{
char str[MAX_NAME], *ptr;
void connect_domain();
DISABLE_AST
void create_matrix_widget();
void get_all_colors();
 void set_title();
 void set_icon_title();
 int insert_object();

	if(!Matrix_id)
		create_matrix_widget();

	connect_domain();
	/*
	dtq_start_timer(2,connect_domain, 0);
	*/	
	XtSetSensitive(Cmnds[START_CONFIG], True);
	Alloc_object.created = 0;

	get_all_colors(display,Matrix_id);
 
	strcpy(str,"SMI_Display     ");
	dic_get_id(Display_title);
	strcpy(&str[16],Display_title);
	set_title(toplevel_widget,str);
	
	if( (ptr = getenv("SMIGUI_TITLE")) == NULL )
		set_icon_title(toplevel_widget,Domain_name);
	else if (*ptr == '\0')
		set_icon_title(toplevel_widget,Domain_name);
	else
		set_icon_title(toplevel_widget,ptr);
	/*
    MrmFetchIconLiteral(s_MrmHierarchy, "smid_icon", 
						DefaultScreenOfDisplay(display), 
						display, rgb_colors[BLACK], rgb_colors[GREEN], 
						&smid_icon);
    MrmFetchIconLiteral(s_MrmHierarchy, "backpix_icon", 
						DefaultScreenOfDisplay(display), 
						display, rgb_colors[BLACK], rgb_colors[NONE], 
						&Backpix_icon);
    MrmFetchIconLiteral(s_MrmHierarchy, "backpix_icon_dot", 
						DefaultScreenOfDisplay(display), 
						display, rgb_colors[BLACK], rgb_colors[NONE], 
						&Backpix_icon_dot);
    MrmFetchIconLiteral(s_MrmHierarchy, "backpix_icon_none", 
						DefaultScreenOfDisplay(display), 
						display, rgb_colors[BLACK], rgb_colors[NONE], 
						&Backpix_icon_none);
	set_something(toplevel_widget,XmNiconPixmap,smid_icon);
    MrmFetchLiteral(s_MrmHierarchy,"smid_default_font",
    						display,(char **)&smid_default_font, &type );
    MrmFetchLiteral(s_MrmHierarchy,"smid_menu_font",
    						display,(char **)&smid_menu_font, &type );
	
	smid_default_font = (XmFontList)util_get_font('d', toplevel_widget);
	smid_menu_font = (XmFontList)util_get_font('m', toplevel_widget);
	smid_label_font = (XmFontList)util_get_font('b', toplevel_widget);

 	put_popup(PARAM,"param","Parameter");
	XtUnmanageChild(Pop_widget_id[PARAM]);
*/
      
	insert_object(-1, -1, "ALLOCATION", 1, 0);
	
ENABLE_AST
}

void create_file_button (w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  if((tag) || (reason)){}
	File_button_id = w;
}

void create_window(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  if((tag) || (reason)){}
	Window_id = w;
}

void create_matrix(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{

  if((tag) || (reason)){}
	Matrix_id = w;
}

void create_command(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  if(reason){}
	Cmnds[*tag] = w;
}

void create_alloc_pop(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{

  if(reason){}
	Alloc_pop_ids[*tag] = w;
}

void alloc_value_changed(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
	int val = 0;
	int new_tag = ALLOC_POP;
	void get_something();

  if(reason){}
	if(*tag == 0)
	{
		get_something(w, XmNset, &val);
		if(val)
			XtSetSensitive(Alloc_pop_ids[2], False);
	}
	else if(*tag == 1)
	{
		get_something(w, XmNset, &val);
		if(val)
		{
			XtSetSensitive(Alloc_pop_ids[2], True);
		}
	}
	else
	{
		ok_pop_up(w, new_tag, reason);
		XtUnmanageChild(XtParent(w));
	}
}

void create_color(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  void set_color();

  if(reason){}
	if(*tag == 5)
		Display_color_id = w;
	else
		set_color(w,XmNbackground,*tag);
}

void activate_color(w, tag, reason)
Widget		w;
int		tag;
unsigned long	*reason;
{
  void set_color();

  if((w) || (reason)){}
	Curr_color = tag;
	set_color(Display_color_id,XmNbackground,tag);
}

void create_label(w, tag, reason)
Widget		w;
int		*tag;
unsigned long	*reason;
{
  void set_something();

  if(reason){}
	if(*tag == 0)
	{
		Label_id = w;
/*
		put_label();
*/
    }
	else if(*tag == 1)
	{
	 	set_something(w,XmNlabelString,Curr_object->state);	
	}
	else if(*tag >= 10)
	{
		Par_label_ids[*tag - 10] = w;
	}
}

void connect_object(obj)
OBJ *obj;
{
	char full_obj_name[MAX_NAME];
	char *obj_name;
	void update_obj_statechange();
	void set_something();

	obj_name = obj->name;
	if((obj->name_id) && (obj->menu_id))
	{
		if(obj->connecting == 1)
		{
			if(obj->name_alias[0])
				obj_name = obj->name_alias;
			set_something(obj->name_id,XmNlabelString, obj_name);
/*
#ifdef linux
			set_something(obj->menu_id,XmNrowColumnType, XmMENU_PULLDOWN);
#endif
*/
			set_something(obj->menu_id,XmNshadowThickness, 2);
/*
#ifdef linux
			set_something(obj->menu_id,XmNrowColumnType, XmMENU_BAR);
#endif
*/
			obj->connecting = 2;
		}
	}		
	strcpy(full_obj_name,Domain_name);
	strcat(full_obj_name,"::");
	strcat(full_obj_name,obj->name);

	obj->statechange_id = 
	smiui_book_statechange(full_obj_name, update_obj_statechange, obj);

	N_connected_objs++;
}

int get_state_color(state)
char *state;
{
	int index;
	int find_state_color();

	index = find_state_color(state);
	if(index != -1)
	{
		return(State_colors[index].color);
	}
	return(NONE);
}


void update_obj_statechange(id, objp)
int *id;
OBJ **objp;
{
	int busy;
	int n_actions, n_pars, a_index, p_index, act_index = 0;
	OBJ *obj;
	int i, ret;
	Widget create_button(), create_separator(), create_dummy_label();
	PAR_TYPE *par_list;
	void set_object_state();
	void put_obj_pars();

	DISABLE_AST

	  if(id) {}
	obj = *objp;

	if(!obj->statechange_id)
	{
		ENABLE_AST
		return;
	}
	ret = smiui_get_state(obj->statechange_id, &busy, obj->state, &n_actions);
	if(!ret)
	{
		strcpy(obj->state,"DEAD");
	}
	if(busy)
	{
		if((obj->state_id)&&(obj->name_id))
		{
			XtSetSensitive(obj->name_id, False);
			XtSetSensitive(obj->state_id, False);
		}
		XFlush(XtDisplay(Matrix_id));
		
		ENABLE_AST
		return;
	}
	else
	{
		if((obj->obj_id)&&(obj->state_id)&&(obj->name_id))
		{
			set_object_state(obj);
			XtSetSensitive(obj->name_id, True);
			XtSetSensitive(obj->state_id, True);
		}
		else
		{
			ENABLE_AST
			return;
		}
		/*
		XFlush(XtDisplay(Matrix_id));
		*/
	}

	obj->actions.n_actions = n_actions;
	a_index = 0;

	for(i = 0; obj->action_ids[i]; i++)
	{
		XtDestroyWidget(obj->action_ids[i]);
		obj->action_ids[i] = 0;
	}
	if(Configuring)
	{
		obj->action_ids[0] = create_button(obj,-1);
		obj->action_ids[1] = create_button(obj,-2);
		obj->action_ids[2] = create_button(obj,-3);
	}
	else
	{
		if(!obj->params)
		{
			obj->params = (PAR_TYPE *)malloc(sizeof(PAR_TYPE)*100);  
		}
		p_index = 0;
		par_list = obj->params; 
		par_list[p_index].param[0] = '\0';
		while( smiui_get_next_obj_param(obj->statechange_id, 
			par_list[p_index].param, 
			&par_list[p_index].type, &par_list[p_index].size))
		{
			if(par_list[p_index].size)
			{
				switch(par_list[p_index].type)
				{
					case SMI_STRING:
						smiui_get_obj_param_value(obj->statechange_id, 
							par_list[p_index].value_str);
						break;
					case SMI_INTEGER:
						smiui_get_obj_param_value(obj->statechange_id, 
							&par_list[p_index].value_int);
						sprintf(par_list[p_index].value_str,"%d",
							par_list[p_index].value_int);
						break;
					case SMI_FLOAT:
						smiui_get_obj_param_value(obj->statechange_id, 
							&par_list[p_index].value_float);
						sprintf(par_list[p_index].value_str,"%e",
							par_list[p_index].value_float);
						break;
				}
			}
			else
				strcpy(par_list[p_index].value_str," -- ");
			p_index++;
			par_list[p_index].param[0] = '\0';
		}
		if((Pop_widget_id[PAR_VIEW]) && (Curr_object == obj))
		{
	        put_obj_pars(obj);
		}
		if(p_index)
		{
			obj->action_ids[act_index++] = create_button(obj,-4);
			obj->action_ids[act_index++] = create_separator(obj->action_id);
		}
		obj->action_ids[act_index++] = create_dummy_label(obj->action_id);

		while( smiui_get_next_action(obj->statechange_id, 
			obj->actions.actions[a_index], &n_pars))
		{
			if(strncmp(obj->actions.actions[a_index],"X_",2))
				obj->action_ids[act_index++] = create_button(obj,a_index);

			if((long)(obj->actions.params[a_index]) > 1)
			{
				free(obj->actions.params[a_index]);
				obj->actions.params[a_index] = 0;
			}
			if(n_pars)
			{
				p_index = 0;
				par_list = obj->actions.params[a_index] = 
					(PAR_TYPE *)malloc(sizeof(PAR_TYPE)*100);
				while( smiui_get_next_param(obj->statechange_id, 
					par_list[p_index].param, 
					&par_list[p_index].type, &par_list[p_index].size))
				{
					if(par_list[p_index].size)
					{
						switch(par_list[p_index].type)
						{
							case SMI_STRING:
								smiui_get_param_default_value(obj->statechange_id, 
									par_list[p_index].value_str);
								break;
							case SMI_INTEGER:
								smiui_get_param_default_value(obj->statechange_id, 
									&par_list[p_index].value_int);
								sprintf(par_list[p_index].value_str,"%d",
									par_list[p_index].value_int);
								break;
							case SMI_FLOAT:
								smiui_get_param_default_value(obj->statechange_id, 
									&par_list[p_index].value_float);
								sprintf(par_list[p_index].value_str,"%4.4f",
									par_list[p_index].value_float);
								break;
						}
					}
					else
						par_list[p_index].value_str[0] = '\0';
					p_index++;
					par_list[p_index].param[0] = '\0';
				}
			}
			a_index++;
		}
	}
	XFlush(XtDisplay(Matrix_id));

	ENABLE_AST
	
}

void update_alloc_statechange(id, objp)
int *id;
OBJ **objp;
{
	int busy;
	OBJ *obj;
	int i, ret, index, down = 0;
	Widget create_button(), create_separator();
	static int first_time = 1;
	static int read_only;
	void update_all_objects();
	int alloc_pid, my_pid;
	void put_label();

	if(id){}
	obj = *objp;

	if(!obj->statechange_id)
	{
		return;
	}
	for(i = 0; obj->action_ids[i]; i++)
	{
		XtDestroyWidget(obj->action_ids[i]);
		obj->action_ids[i] = 0;
	}
	ret = smiui_get_alloc_state(obj->statechange_id, &busy, obj->state);
	if(!ret)
	{
		down = 1;
	}
	if(first_time)
	{
		read_only = Read_only;
		if((Allocate_domain) && (!Read_only))
			smiui_allocate(Domain_name);
		first_time = 0;
	}
	index = 0;
	if(!read_only)
		Read_only = 0;
	
	obj->action_id = File_button_id;
	
	obj->action_ids[index++] = create_separator(File_button_id);
	
	sscanf(Display_title,"%d",&my_pid); 
	sscanf(obj->state,"%d",&alloc_pid);
	if(!strcmp(obj->state,"NOT_ALLOCATED"))
	{
		if(!Read_only)
		{
			obj->action_ids[index++] = create_button(obj,-5);
			obj->action_ids[index++] = create_button(obj,-7);
		}
		obj->state[0] = 0;
	}
	/*
	else if(!strcmp(obj->state, Display_title))
	*/
#ifdef linux
	else if((alloc_pid >= my_pid) && (alloc_pid <= (my_pid+3)))
#else
	else if(alloc_pid == my_pid)
#endif
	{
		if(!Read_only)
		{
			obj->action_ids[index++] = create_button(obj,-6);
			obj->action_ids[index++] = create_button(obj,-7);
		}
		obj->state[0] = 1;
	  
	}
	else
	{
		Read_only = 1;
	}
	obj->action_ids[index++] = create_separator(File_button_id);
	
	obj->action_ids[index++] = create_button(obj,-8);
	if(!down)
	{
		put_label();
		if(N_objects)
			update_all_objects();
     	}
	if(Top_id)
	{
		XFlush(XtDisplay(Top_id));
	}
}

void set_object_state(obj)
OBJ *obj;
{
	int color;
	void set_color();
	void set_something();

	set_color(obj->state_id, XmNforeground, BLACK);
	set_something(obj->state_id,XmNlabelString,obj->state);
	color = get_state_color(obj->state);
	set_color(obj->state_id, XmNbackground, color);
	if(color == BLACK)
		set_color(obj->state_id, XmNforeground, WHITE);
}

Widget create_button(obj, index)
OBJ *obj;
int index;
{
	Widget w;
	Arg arglist[10];
	int i, n = 0, found = 0;
	char w_name[40];
	void do_command();
	XmString label;
	void delete_str();

	if(obj == &Alloc_object)
	{
		XtSetArg(arglist[n], XmNfontList, smid_menu_font);  n++;
	}
	else
	{
		XtSetArg(arglist[n], XmNfontList, smid_default_font);  n++;
	}
	XtSetArg(arglist[n],XmNuserData,index);n++;
	if(index == -1)
	{
	        label = create_str("Remove Object");
	}
	else if(index == -2)
	{
		label = create_str("Move Object");
	}
	else if(index == -3)
	{
		label = create_str("Select State Colour");
	}
	else if(index == -4)
	{
		label = create_str("Parameters");
	}
	else if(index == -5)
	{
	        label = create_str("Allocate Domain"); 
	}
	else if(index == -6)
	{
		label = create_str("Release Domain");
	}
	else if(index == -7)
	{
		label = create_str("Shutdown Domain & Exit");  
	}
	else if(index == -8)
	{
		label = create_str("Exit");
      	}
	else
	{
		label = create_str(obj->actions.actions[index]);
		if(Read_only)
		{
		  XtSetArg(arglist[n],XmNsensitive,False);n++;
		}
		else if(obj->attrs && User[0])
		{
			for(i = 0; obj->attrs->attributes[i][0]; i++)
			{
				if(!strcmp(obj->attrs->attributes[i],User))
				{
					found = 1;
					break;
				}
			}
			if(!found)
			  XtSetArg(arglist[n],XmNsensitive,False);n++;
		}
	}
	/*
	if(index < 0)
	*/
	    XtSetArg(arglist[n],XmNlabelString,label);n++;
	sprintf(w_name,"%s_action",obj->name);
	w = XmCreatePushButton(obj->action_id, w_name, arglist,n);
	XtManageChild(w);
	/*
	if(index < 0)
	*/
	  delete_str(label);
	  /*
	else
	{
		set_something(w,XmNlabelString,obj->actions.actions[index]);
		if(Read_only)
		{
			XtSetSensitive(w, False);
		}
		if(obj->attrs && User[0])
		{
			for(i = 0; obj->attrs->attributes[i][0]; i++)
			{
				if(!strcmp(obj->attrs->attributes[i],User))
				{
					found = 1;
					break;
				}
			}
			if(!found)
				XtSetSensitive(w, False);
		}
	}
	  */
	XtAddCallback(w,XmNactivateCallback, (XtCallbackProc)do_command, obj);
      	return(w);
}

Widget create_dummy_label(parent_id)
Widget parent_id;
{
	Widget w;
	Arg arglist[10];
	int n = 0;
	char w_name[40];
	void set_something();

	XtSetArg(arglist[n], XmNfontList, smid_default_font);  n++;
	sprintf(w_name,"action_label");
	w = XmCreateLabel(parent_id, w_name, arglist,n);
	XtManageChild(w);
	set_something(w,XmNlabelString,"Actions :");
	set_something(w,XmNalignment,XmALIGNMENT_BEGINNING);
	set_something(w,XmNshadowThickness,2);
	return(w);
}

Widget create_separator(parent_id)
Widget parent_id;
{
	Widget w;
	Arg arglist[10];
	int n = 0;

	w = XmCreateSeparator(parent_id, "separator",
		arglist,n);
	XtManageChild(w);
	return(w);
}

Widget create_vertical_separator(parent_id)
Widget parent_id;
{
	Widget w;
	Arg arglist[10];
	int n = 0;

	XtSetArg(arglist[n], XmNorientation, XmVERTICAL);  n++;
	w = XmCreateSeparator(parent_id, "separator",
		arglist,n);
	XtManageChild(w);
	return(w);
}

void activate_object(w, dst_obj, reason)
Widget w;
OBJ *dst_obj;
unsigned long *reason;
{
	OBJ *tmp_obj;
	OBJ *src_obj;
	int i;
	int src = 0, dst = 0;
	int disconnect_object();
	void set_object_button();
	void set_color();

	if((w) || (reason)){}
	if(Moving)
	{
		tmp_obj = (OBJ *)malloc(sizeof(OBJ) - (24 + MAX_ACTIONS));
		src_obj = Moving;
		if(disconnect_object(src_obj))
			src = 1;
		if(disconnect_object(dst_obj))
			dst = 1;
		memcpy(tmp_obj, dst_obj, sizeof(OBJ) - (24 + (MAX_ACTIONS*4)));
		memcpy(dst_obj, src_obj, sizeof(OBJ) - (24 + (MAX_ACTIONS*4)));
		memcpy(src_obj, tmp_obj, sizeof(OBJ) - (24 + (MAX_ACTIONS*4)));
		set_object_button(src_obj,1);
		set_object_button(dst_obj,1);
		if(dst)
		{
			connect_object(src_obj);
		}
		if(src)
		{
			connect_object(dst_obj);
		}
		if(dst_obj->attrs)
		{
			for(i = 0; dst_obj->attrs->attributes[i][0]; i++)
			{
				if(!strcmp(dst_obj->attrs->attributes[i],"ASSOCIATED"))
				{
				  
					if(strstr(dst_obj->name,"::"))
					  set_color(dst_obj->name_id,XmNbackground,LIGHTGRAY);
					  /*
						set_something(dst_obj->name_id,XmNbackgroundPixmap,Backpix_icon_dot);
					  */
					else
					  /*
						set_something(dst_obj->name_id,XmNbackgroundPixmap,Backpix_icon);
				  */
					   set_color(dst_obj->name_id,XmNbackground,GRAY);
				}
			}
		}
		if(src_obj->attrs)
		{
			for(i = 0; src_obj->attrs->attributes[i][0]; i++)
			{
				if(!strcmp(src_obj->attrs->attributes[i],"ASSOCIATED"))
				{
				  
					if(strstr(src_obj->name,"::"))
					  set_color(src_obj->name_id,XmNbackground,LIGHTGRAY);
					/*
						set_something(src_obj->name_id,XmNbackgroundPixmap,Backpix_icon_dot);
					*/
					else
					   set_color(src_obj->name_id,XmNbackground,GRAY);
					/*
						set_something(src_obj->name_id,XmNbackgroundPixmap,Backpix_icon);
					*/
				}
			}
		}
		Moving = 0;
	}
}

int disconnect_object(obj)
OBJ *obj;
{
	int i;

	if(obj->statechange_id)
	{
/*
		if(obj->index_y == -1)
			smiui_cancel_alloc_statechange(obj->statechange_id);
		else
*/
			smiui_cancel_statechange(obj->statechange_id);
		for(i = 0; obj->action_ids[i]; i++)
		{
			if((Matrix_id)/*||(obj->index_y == -1)*/)
				XtDestroyWidget(obj->action_ids[i]);
			obj->action_ids[i] = 0;
		}
		obj->statechange_id = 0;
		N_connected_objs--;
		return(1);
	}
	return(0);
}

void remove_object(obj)
OBJ *obj;
{
  void set_object_button();

	disconnect_object(obj);
	set_object_button(obj,1);
	obj->visible = 0;
	if(N_visible_objs == N_connected_objs)
		XtSetSensitive(Cmnds[INS_OBJ], False);
	else
		XtSetSensitive(Cmnds[INS_OBJ], True);
	if(!N_connected_objs)
		XtSetSensitive(Cmnds[REM_OBJ], False);
	else
		XtSetSensitive(Cmnds[REM_OBJ], True);
}

void delete_object(obj, disc)
OBJ *obj;
int disc;
{
	int i;

	if(disc)
		disconnect_object(obj);
	if(obj->obj_id)
	{
/*
		if(obj->index_y == -1)
			XtDestroyWidget(obj->obj_id);
*/
		obj->obj_id = 0;
		obj->state_id = 0;
		obj->action_id = 0;
		for(i = 0; obj->action_ids[i]; i++)
		{
			obj->action_ids[i] = 0;
		}
	}
	obj->created = 0;

}

void move_object(obj)
OBJ *obj;
{

	Moving = obj;
}

void put_obj_pars(obj)
OBJ *obj;
{
	int p_index;
	PAR_TYPE *par_list;
	char par_labels[3][512];
	void set_something();

		par_list = obj->params;
		p_index = 0;
		while(par_list[p_index].param[0])
		{
			if(!p_index)
			{
				strcpy(par_labels[0],"Type\n\n");
				strcpy(par_labels[1],"Name\n\n");
				strcpy(par_labels[2],"Value\n\n");
			}
			strcat(par_labels[1], par_list[p_index].param);
			strcat(par_labels[1], "\n");
			switch(par_list[p_index].type)
			{
				case SMI_STRING:
					strcat(par_labels[0], "STRING\n");
/*
					strcat(par_labels[2], par_list[p_index].value_str);
					strcat(par_labels[2], "\n");
*/
					break;
				case SMI_INTEGER:
					strcat(par_labels[0], "INTEGER\n");
/*
					sprintf(str,"%d\n",par_list[p_index].value_int);
					strcat(par_labels[2], str);
*/
					break;
				case SMI_FLOAT:
					strcat(par_labels[0], "FLOAT\n");
/*
					sprintf(str,"%e\n",par_list[p_index].value_float);
					strcat(par_labels[2], str);
*/
					break;
			}
			strcat(par_labels[2], par_list[p_index].value_str);
			strcat(par_labels[2], "\n");

			p_index++;
		}
		/*
		set_something(Par_label_ids[0],XmNlabelString,par_labels[0]);
		*/
		set_something(Par_label_ids[1],XmNlabelString,par_labels[1]);
		set_something(Par_label_ids[2],XmNlabelString,par_labels[2]);
}

int check_put_par_file(obj, action, param)
char *obj, *action, *param;
{
	char tmp[MAX_FILE_NAME], str[128];
	Widget id,sel_id;
	int i, index = 0;
	FILE *fptr;
	void put_popup();
	int XmListDeleteAllItems();
	int XmListAddItem();
	void set_something();

	strcpy(tmp,obj);
	strcat(tmp,"/");
	strcat(tmp,action);
	strcat(tmp,"/");
	strcat(tmp,param);
	for(i = 0; File_list[i].file_ident[0]; i++)
	{
		if(!strcmp(File_list[i].file_ident, tmp))
		{
			if( ( fptr = fopen( File_list[i].file_name, "r" ) ) == (FILE *)0 )
		  	{
		    	printf("Cannot open: %s for reading\n",File_list[i].file_name);
				return(0);
			}
	 		put_popup(PAR_FILE,"par_file",obj);
			sel_id = Pop_widget_id[PAR_FILE];
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_HELP_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_APPLY_BUTTON);
			XtUnmanageChild(id);
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_TEXT);
			set_something(id, XmNvalue,"");
			id = XmSelectionBoxGetChild(sel_id,XmDIALOG_LIST);
			sprintf(str,"%s: %s List:",action, param);
			set_something(sel_id, XmNlistLabelString,
				str);
			sprintf(str,"%s:",param);
			set_something(sel_id, XmNselectionLabelString,
				str);
			XmListDeleteAllItems(id);
			while(1)
			{
				fscanf(fptr,"%s",str);
				if(feof(fptr))
					break;
				XmListAddItem(id,create_str(str),0);
				index++;
			}
			fclose(fptr);
			set_something(id,XmNlistItemCount,index);
			set_something(id,XmNlistVisibleItemCount,(index < 20) ? 
				index : 20);
			return(1);
		}
	}
	return(0);
}

void do_command(w, obj, reason)
Widget w;
OBJ *obj;
unsigned long *reason;
{
	int index, its_file;
	PAR_TYPE *par_list;
	char full_obj_name[MAX_NAME];
	char str[MAX_NAME];
	void put_popup();
	void get_something();
	void set_something();

	if(reason){}
	get_something(w,XmNuserData,&index);
	if(index == -1)
	{
		remove_object(obj);
		return;
	}
	if(index == -2)
	{
		move_object(obj);
		return;
	}
	if(index == -3)
	{
		Curr_object= obj;
		put_popup(COLOR,"select_color_form","Colour Selection");
		return;
	}
	if(index == -4)
	{
		Curr_object = obj;
		put_popup(PAR_VIEW,"par_view_board",obj->name);
		put_obj_pars(obj);
		return;
	}
	if(index == -5)
	{
		smiui_allocate(Domain_name);
/*
		put_popup(ALLOC_POP,"alloc_form","Allocation");
		sprintf(str,"Allocate domain \"%s\" to this GUI",Domain_name);
		set_something(Alloc_pop_ids[0], XmNlabelString, str);
		sprintf(str,"Allocate a domain to domain \"%s\"",Domain_name);
		set_something(Alloc_pop_ids[1], XmNlabelString, str);
*/
		return;
	}
	if(index == -6)
	{
		smiui_release(Domain_name);
		return;
	}
	if(index == -7)
	{
		smid_kill_exit();
		return;
	}
	if(index == -8)
	{
		smid_exit();
		return;
	}
	if((long)(obj->actions.params[index]) > 1)
	{
		par_list = obj->actions.params[index];
		its_file = check_put_par_file(obj->name,obj->actions.actions[index],
			par_list[0].param);
        if(!its_file)
		{
	 		put_popup(PARAM,"param","Parameter");
			strcpy(str, par_list[0].param);
			switch(par_list[0].type)
			{
				case SMI_STRING:
					strcat(str, " (STRING)");
					break;
				case SMI_INTEGER:
					strcat(str, " (INTEGER)");
					break;
				case SMI_FLOAT:
					strcat(str, " (FLOAT)");
					break;
			}
			set_something(Pop_widget_id[PARAM],XmNselectionLabelString,
				str);
			if(par_list[0].value_str[0])
			{
				set_something(Pop_widget_id[PARAM],XmNtextString,
				par_list[0].value_str);
			}
			else
			{
				set_something(Pop_widget_id[PARAM],XmNtextString,
				"");
			}
		}
		P_obj = obj;
		P_obj_act_index= index;
		P_obj_par_index = 0;
    }
	else
	{
		strcpy(full_obj_name,Domain_name);
		strcat(full_obj_name,"::");
		strcat(full_obj_name,obj->name);
	
		smiui_send_command(full_obj_name,obj->actions.actions[index]);
	}
}

void create_matrix_widget()
{
Widget row_col_id, top_id;
Arg arglist[10];
int n = 0;
char w_name[40];

	top_id = Window_id;
	XtSetArg(arglist[n], XmNborderWidth, 0); n++;
	XtSetArg(arglist[n], XmNorientation, XmVERTICAL);  n++;
        XtSetArg(arglist[n], XmNnumColumns, 4);  n++;
	XtSetArg(arglist[n], XmNpacking, XmPACK_COLUMN);  n++;
        XtSetArg(arglist[n], XmNadjustLast, False); n++;
	sprintf(w_name,"matrix_row");
	row_col_id = XmCreateRowColumn(top_id,w_name,arglist,n);
	XtManageChild(row_col_id);
	Matrix_id = row_col_id;
	/*
	XmScrolledWindowSetAreas(Window_id,NULL, NULL, Matrix_id); 
	*/
}

void create_object(obj)
OBJ *obj;
{
Widget top_id, row_col_id, menu_id, state_id, cascade_id;
Arg arglist[10];
void activate_object();
int i, n = 0;
char w_name[40];
 void set_color(); 
	void set_something();

	if(!Matrix_id)
		create_matrix_widget();
	top_id = Matrix_id;
/*
	if(obj->index_y == -1)
	{
		top_id = Cmnds[ALLOC];
    }
*/
	XtSetArg(arglist[n], XmNentryAlignment, XmALIGNMENT_CENTER);  n++;
	XtSetArg(arglist[n], XmNborderWidth, 0); n++;
    XtSetArg(arglist[n], XmNspacing, 3);  n++;
	sprintf(w_name,"%s_row",obj->name);
	row_col_id = XmCreateRowColumn(top_id,w_name,arglist,n);
	XtManageChild(row_col_id);
	obj->obj_id = row_col_id;
	n = 0;
	XtSetArg(arglist[n], XmNentryAlignment, XmALIGNMENT_CENTER);  n++;
	XtSetArg(arglist[n], XmNorientation, XmVERTICAL);  n++;
	if(obj->visible != 1)    
	    XtSetArg(arglist[n], XmNshadowThickness, 0);  n++;
	sprintf(w_name,"%s_menu",obj->name);
	
	XtSetArg(arglist[n], XmNrowColumnType, XmMENU_BAR);  n++;
	XtSetArg(arglist[n],XmNpopupEnabled,False); n++;
	
	menu_id = XmCreateRowColumn(row_col_id,w_name,arglist,n);
	XtManageChild(menu_id);
	obj->menu_id = menu_id;
	n = 0;
    XtSetArg(arglist[n], XmNwidth, 200);  n++;
    XtSetArg(arglist[n], XmNheight, 30);  n++;
	XtSetArg(arglist[n], XmNfontList, smid_default_font);  n++;
	sprintf(w_name,"%s_state",obj->name);
	state_id = XmCreateLabel(row_col_id,w_name,arglist,n);
	set_something(state_id, XmNlabelString," ");
	XtManageChild(state_id);
	obj->state_id = state_id;
	n = 0;                                                    
    XtSetArg(arglist[n], XmNheight, 27);  n++;
  
    XtSetArg(arglist[n], XmNshadowThickness, 0);  n++;
  
    XtSetArg(arglist[n], XmNalignment, XmALIGNMENT_CENTER);  n++;
	XtSetArg(arglist[n], XmNfontList, smid_default_font);  n++;
	XtSetArg(arglist[n],XmNcascadePixmap,0); n++;
	sprintf(w_name,"%s",obj->name);
	cascade_id = XmCreateCascadeButton(menu_id,w_name,arglist,n);
	set_something(cascade_id,XmNalignment,XmALIGNMENT_CENTER);
	
	set_color(cascade_id, XmNforeground, BLACK);
	
	if(obj->visible == 1)
	{
/*
		if(obj->index_y == -1)
			set_something(cascade_id, XmNlabelString,"ALLOCATION");
		else
*/
			if(obj->name_alias[0])
				set_something(cascade_id, XmNlabelString,obj->name_alias);
			else
				set_something(cascade_id, XmNlabelString,obj->name);
	}
	else
	{
		if(Configuring)
			set_something(cascade_id,XmNlabelString,".");
		else
			set_something(cascade_id,XmNlabelString," ");
	}
	
	set_color(cascade_id,XmNbackground,NONE);
	
	if(obj->attrs)
	{
		for(i = 0; obj->attrs->attributes[i][0]; i++)
		{
		  
			if(!strcmp(obj->attrs->attributes[i],"ASSOCIATED"))
			{
				if(strstr(obj->name,"::"))
				  set_color(cascade_id,XmNbackground,LIGHTGRAY);				/*
					set_something(cascade_id,XmNbackgroundPixmap,Backpix_icon_dot);
				*/
				else
				  set_color(cascade_id,XmNbackground,GRAY);
				  /*
					set_something(cascade_id,XmNbackgroundPixmap,Backpix_icon);
				  */
			}
		}
	}
	XtAddCallback(cascade_id,XmNcascadingCallback, 
		      (XtCallbackProc)activate_object, obj);
	obj->name_id = cascade_id;
	n = 0;                   
	sprintf(w_name,"%s_pull",obj->name);
	XtSetArg(arglist[n],XmNpopupEnabled,False); n++;
	obj->action_id = XmCreatePulldownMenu(menu_id,w_name,arglist,n);
	set_something(cascade_id, XmNsubMenuId, obj->action_id);
	XtManageChild(cascade_id);
/*
	if(obj->index_y == -1)
	{
		set_something(top_id,XmNmenuHelpWidget,obj->obj_id);
    }
*/
	XFlush(XtDisplay(top_id));
}

void set_object_button(obj, config)
OBJ *obj;
int config;
{
  void set_color();
	void set_something();

	if(!obj->statechange_id)
	{
		obj->connecting = 1;
/*
#ifdef linux
		set_something(obj->menu_id,XmNrowColumnType, XmMENU_PULLDOWN);
#endif
*/
		set_something(obj->menu_id,XmNshadowThickness, 0);
/*
#ifdef linux
		set_something(obj->menu_id,XmNrowColumnType, XmMENU_BAR);
#endif
*/
		set_color(obj->state_id, XmNbackground, NONE);
		set_something(obj->state_id, XmNlabelString," ");
		if(config > 0)
			set_something(obj->name_id, XmNlabelString,".");
		else
			set_something(obj->name_id, XmNlabelString,"  ");
	}
	/*
	set_something(obj->name_id,XmNbackgroundPixmap,Backpix_icon_none);
	*/
	set_color(obj->name_id, XmNbackground, NONE);
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

void put_next_param(par_list)
PAR_TYPE *par_list;
{
char aux_str[MAX_NAME];
int its_file;
 void put_popup();
	void set_something();

		its_file = check_put_par_file(P_obj->name,
			P_obj->actions.actions[P_obj_act_index],
			par_list[P_obj_par_index].param);
        if(!its_file)
		{
		 		put_popup(PARAM,"param","Parameter");
				strcpy(aux_str, par_list[P_obj_par_index].param);
				switch(par_list[P_obj_par_index].type)
				{
					case SMI_STRING:
						strcat(aux_str, " (STRING)");
						break;
					case SMI_INTEGER:
						strcat(aux_str, " (INTEGER)");
						break;
					case SMI_FLOAT:
						strcat(aux_str, " (FLOAT)");
						break;
				}
				set_something(Pop_widget_id[PARAM],XmNselectionLabelString,
					aux_str);
				if(par_list[P_obj_par_index].value_str[0])
				{
					set_something(Pop_widget_id[PARAM],XmNtextString,
					par_list[P_obj_par_index].value_str);
				}
				else
				{
					set_something(Pop_widget_id[PARAM],XmNtextString,
					"");
				}

		}	       
		XFlush(XtDisplay(Matrix_id));
}

void ok_pop_up (w, tag, reason)
Widget		w;
int		tag;
unsigned long	*reason;
{
Widget id;
char *str, par_str[MAX_BUFFER], aux_str[MAX_NAME];
int index = 0, i, j, k, rows;
short x_len = 0, y_len = 0;
PAR_TYPE *par_list;
char full_obj_name[MAX_NAME];
void connect_domain();
OBJ *obj, *find_object();
int ind_x, ind_y, end, found = 0;
FILE *fptr;
	int get_all_rows();
	int get_visible_rows();
	void delete_matrix_objects();
	void insert_state_color();
	void set_color();
	void get_something();
    
	if(reason){}
	par_str[0] = 0;
	if((tag != COLOR) && (tag != PARAM) && (tag != ALLOC_POP))
	{                                       
		id = XmSelectionBoxGetChild(w,XmDIALOG_TEXT);
		str = XmTextGetString(id);
		if(!str[0])
		{
			XtFree(str);
			return;
		}
	}
	switch(tag)
	{
		case INS_OBJ:
			obj = find_object(str);
			set_object_button(obj,1);
			connect_object(obj);
			obj->visible = 1;
			if(N_visible_objs == N_connected_objs)
				XtSetSensitive(Cmnds[INS_OBJ], False);
			else
				XtSetSensitive(Cmnds[INS_OBJ], True);
			if(!N_connected_objs)
				XtSetSensitive(Cmnds[REM_OBJ], False);
			else
				XtSetSensitive(Cmnds[REM_OBJ], True);
			break;
		case REM_OBJ:
			obj = find_object(str);
			remove_object(obj);
			obj->visible = 0;
			break;
		case SAVE_CONFIG:
		  	if( ( fptr = fopen( str, "w" ) ) == (FILE *)0 )
		  	{
    			printf("Cannot open: %s for writing\n",str);
				return;
		  	}                   
			get_something(Top_id,XmNwidth,&x_len);
			get_something(Top_id,XmNheight,&y_len);

			fprintf(fptr,"%d %d\n",x_len, y_len);
/*
			fprintf(fptr,"%s\n",Alloc_object.name);
			if(Alloc_object.statechange_id)
				fprintf(fptr,"V\n");
			else
				fprintf(fptr,"NV\n");
*/
			rows = get_visible_rows();
			for(j = 0; j < rows; j++)
			{
				for(i = 0; i < 4; i++)
				{
					if(Object_matrix[j][i].statechange_id)
					{
						fprintf(fptr,"%s",Object_matrix[j][i].name);
						if(Object_matrix[j][i].attrs)
						{
							for(k = 0; Object_matrix[j][i].attrs->attributes[k][0]; k++)	
							{	
								fprintf(fptr," %s",
									Object_matrix[j][i].attrs->attributes[k]);
							}
						}
						fprintf(fptr,"\n");
						fprintf(fptr,"V\n");
					}
					else if(Object_matrix[j][i].visible != -1)
					{
						fprintf(fptr,".\n");
						fprintf(fptr,"NV\n");
					}
				}
			}
			fprintf(fptr,"STATE-COLORS\n");
			for(i = 0; State_colors[i].state[0]; i++)
			{
				fprintf(fptr,"%s\n",State_colors[i].state);
				switch(State_colors[i].color)
				{
					case 0:
						fprintf(fptr,"GREEN\n");
						break;
					case 1:
						fprintf(fptr,"BLUE\n");
						break;
					case 2:
						fprintf(fptr,"YELLOW\n");
						break;
					case 3:
						fprintf(fptr,"ORANGE\n");
						break;
					case 4:
						fprintf(fptr,"RED\n");
						break;
   				}
			}
			if(Pop_widget_id[PAR_VIEW])
			{
			    fprintf(fptr,"PARAMETER_WINDOW\n");
			    fprintf(fptr,"%s\n",Curr_object->name);
			}
			if(File_list[0].file_ident[0])
			{
			    fprintf(fptr,"PARAMETER_FILES\n");
				for(i = 0; File_list[i].file_ident[0]; i++)
				{
				    fprintf(fptr,"%s\n",File_list[i].file_ident);
				    fprintf(fptr,"%s\n",File_list[i].file_name);
				}
			}
			if(Cmd_list[0].obj[0])
			{
			    fprintf(fptr,"STARTUP_COMMANDS\n");
				for(i = 0; Cmd_list[i].obj[0]; i++)
				{
				    fprintf(fptr,"%s ",Cmd_list[i].obj);
				    fprintf(fptr,"%s\n",Cmd_list[i].cmd);
				}
			}
			end = get_all_rows();
			for(ind_y = 0; ind_y < end; ind_y++)
			{
				for(ind_x = 0; ind_x < 4; ind_x++)
				{
					obj = &Object_matrix[ind_y][ind_x];
					if(obj->name_alias[0])
					{
						if(!found)
						{
						    fprintf(fptr,"OBJECT_ALIASES\n");
							found = 1;
						}
						fprintf(fptr,"%s %s\n", obj->name, obj->name_alias);
					}
				}
			}
			fclose(fptr);
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case OPEN_CONFIG:
			strcpy(Config_name,str);
		    delete_matrix_objects();
			connect_domain();
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case COLOR:
			set_color(Curr_object->state_id,
				XmNbackground,Curr_color);
			insert_state_color(Curr_object->state,Curr_color);
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case PARAM:
			get_something(Pop_widget_id[PARAM],XmNtextString,par_str);
			par_list = P_obj->actions.params[P_obj_act_index];
            strcpy(par_list[P_obj_par_index++].value_str,par_str);
			if(par_list[P_obj_par_index].param[0])
			{
			  x_start_timer(1,put_next_param,(long)par_list);
			}
			else
			{
				strcpy(par_str,
					P_obj->actions.actions[P_obj_act_index]);
				for(i = 0; par_list[i].param[0]; i++)
				{
					strcat(par_str,"/");
					strcat(par_str,par_list[i].param);
					switch(par_list[i].type)
					{
						case SMI_STRING:
							break;
						case SMI_INTEGER:
							strcat(par_str,"(I)");
							sscanf(par_list[i].value_str,"%d",
								&par_list[i].value_int);
							break;
						case SMI_FLOAT:
							strcat(par_str,"(F)");
							sscanf(par_list[i].value_str,"%lf",
								&par_list[i].value_float);
							break;
					}
					strcat(par_str,"=");
					switch(par_list[i].type)
					{
						case SMI_STRING:
							strcat(par_str,"\"");
							strcat(par_str,par_list[i].value_str);
							strcat(par_str,"\"");
							break;
						case SMI_INTEGER:
							sprintf(aux_str,"%d",par_list[i].value_int);
							strcat(par_str,aux_str);
							break;
						case SMI_FLOAT:
							sprintf(aux_str,"%e",par_list[i].value_float);
							strcat(par_str,aux_str);
							break;
					}
				}
				strcpy(full_obj_name,Domain_name);
				strcat(full_obj_name,"::");
				strcat(full_obj_name,P_obj->name);
				smiui_send_command(full_obj_name,par_str);
			}
			break;
		case PAR_FILE:
			par_list = P_obj->actions.params[P_obj_act_index];
            strcpy(par_list[P_obj_par_index++].value_str,str);
			if(par_list[P_obj_par_index].param[0])
			{
			  x_start_timer(1,put_next_param,(long)par_list);
			}
			else
			{
				strcpy(par_str,
					P_obj->actions.actions[P_obj_act_index]);
				for(i = 0; par_list[i].param[0]; i++)
				{
					strcat(par_str,"/");
					strcat(par_str,par_list[i].param);
					switch(par_list[i].type)
					{
						case SMI_STRING:
							break;
						case SMI_INTEGER:
							strcat(par_str,"(I)");
							sscanf(par_list[i].value_str,"%d",
								&par_list[i].value_int);
							break;
						case SMI_FLOAT:
							strcat(par_str,"(F)");
							sscanf(par_list[i].value_str,"%lf",
								&par_list[i].value_float);
							break;
					}
					strcat(par_str,"=");
					switch(par_list[i].type)
					{
						case SMI_STRING:
							strcat(par_str,"\"");
							strcat(par_str,par_list[i].value_str);
							strcat(par_str,"\"");
							break;
						case SMI_INTEGER:
							sprintf(aux_str,"%d",par_list[i].value_int);
							strcat(par_str,aux_str);
							break;
						case SMI_FLOAT:
							sprintf(aux_str,"%e",par_list[i].value_float);
							strcat(par_str,aux_str);
							break;
					}
				}
				strcpy(full_obj_name,Domain_name);
				strcat(full_obj_name,"::");
				strcat(full_obj_name,P_obj->name);
				smiui_send_command(full_obj_name,par_str);
			}
			break;
		case ALLOC_POP:
			get_something(Alloc_pop_ids[0], XmNset, &index);
			if(index)
			{

				smiui_allocate(Domain_name);

				printf("allocate %s to me\n", Domain_name);
			}
			else
			{
				str = XmTextGetString(Alloc_pop_ids[2]);
				capitalize(str);

				smiui_allocate_to(Domain_name,str);

				printf("allocate %s to %s\n", str, Domain_name);
				XtFree(str);
			}
	}
	if((tag != COLOR) && (tag != PARAM) && (tag != ALLOC_POP))
		XtFree(str);
}

void domain_connected(tag, n_objs)
int *tag, *n_objs;
{
void do_open_full(), do_open_config();
void update_alloc_statechange();
 void put_label_down();
 void delete_matrix_objects();

 if(tag){}
	if(!Alloc_object.created)
	{
		Alloc_object.statechange_id = 
		smiui_book_alloc_statechange(Domain_name, 
			update_alloc_statechange, &Alloc_object);
		Alloc_object.created = 1;
	}
	if(!*n_objs)
	{
		put_label_down();
		delete_matrix_objects();
		XFlush(XtDisplay(Top_id));
		if( getenv("SMIGUI_VOLATILE") != NULL )
		  exit(1);
	}
	else
	{
		if(!Config_name[0])
			do_open_full();
		else
			do_open_config();
	}
}

void connect_domain()
{
int n_objs;
int tag = 0;

	State_colors[0].state[0] = '\0';
	n_objs = smiui_book_connect_domain(Domain_name, domain_connected, tag);
	if(n_objs)
		domain_connected(&tag, &n_objs);
}

void get_objects(all)
int all;
{
	char obj_name[132], attr[132];
	char *ptr;
	OBJ *find_object();
	int index = 0;
	ATTR_LIST *attrs = 0;
	void insert_matrix();

	while(smiui_get_next_object(Domain_name,obj_name))
	{
	  if((ptr = strstr(obj_name,"::")))
			ptr += 2;
		if(!find_object(ptr))
		{
			index = 0;
			attrs = 0;
			while(smiui_get_next_attribute(obj_name, attr))
			{
				if(index == 0)
				{
					attrs = malloc(sizeof(ATTR_LIST));
				}
				strcpy(attrs->attributes[index],attr);
				index++;
			}
			if(attrs)
				attrs->attributes[index][0] = '\0';
		
			insert_matrix(ptr, all, attrs);
		}
	}
}

void do_open_full()
{
	void do_create_objects();
	void pad_matrix();

/*
	insert_object(-1, -1, "ALLOCATION", 1);
*/
	get_objects(1);
	pad_matrix(0, 0);
	pad_matrix(8, -1);
	x_start_timer(1,do_create_objects,1);
}

int read_config_next(str)
char *str;
{
	int ret = 0;
	if(!strcmp(str,"STATE-COLORS"))
		ret = 1;
	if(!strcmp(str,"PARAMETER_WINDOW"))
		ret = 2;
	if(!strcmp(str,"PARAMETER_FILES")) 
		ret = 3;
	if(!strcmp(str,"OBJECT_ALIASES")) 
		ret = 4;
	if(!strcmp(str,"STARTUP_COMMANDS")) 
		ret = 5;
	return(ret);
}

int read_config_objects(fptr)
FILE *fptr;
{
int visible, index, ret = 0;
char str[80], visible_str[80];
ATTR_LIST *attrs = 0;
 void insert_matrix(); 
 void pad_matrix();

	while(1)
	{
		fscanf(fptr,"%s",str);
		if((ret = read_config_next(str)))
		{
			break;
		}
		index = 0;
		attrs = 0;
		while(!feof(fptr))
		{
			fscanf(fptr,"%s",visible_str);
			if(feof(fptr))
			break;
			if(!strcmp(visible_str,"V"))
			{
				visible = 1;
				break;
			}
			else if(!strcmp(visible_str,"NV"))
			{
				visible = 0;
				break;
			}
			else
			{
				if(index == 0)
				{
					attrs = malloc(sizeof(ATTR_LIST));
				}
				strcpy(attrs->attributes[index],visible_str);
				index++;
			}
		}
		if(attrs)
			attrs->attributes[index][0] = '\0';
		if(strcmp(str,"ALLOCATION"))
			insert_matrix(str, visible, attrs);
	}
	pad_matrix(0, 0);
	get_objects(-1);
	pad_matrix(0, -1);
	return(ret);
}

int read_config_state_colors(fptr)
FILE *fptr;
{
int color, ret = 0;
char str[80], visible_str[80];
 void insert_state_color();

	while(1)
	{
		fscanf(fptr,"%s",str);
		if((ret = read_config_next(str)))
		{
			break;
		}
		if(feof(fptr))
			break;
		fscanf(fptr,"%s",visible_str);
		if(feof(fptr))
			break;
		if(!strcmp(visible_str,"GREEN"))
			color = GREEN;
		else if(!strcmp(visible_str,"BLUE"))
			color = BLUE;
		else if(!strcmp(visible_str,"YELLOW"))
			color = YELLOW;
		else if(!strcmp(visible_str,"ORANGE"))
			color = ORANGE;
		else if(!strcmp(visible_str,"RED"))
			color = RED;
		insert_state_color(str, color); 
	}
	return(ret);
}

int read_config_par_window(fptr, par_win)
FILE *fptr;
char *par_win;
{
int ret = 0;
char str[80];

	fscanf(fptr,"%s",par_win);
	if(feof(fptr))
		return(0);
	fscanf(fptr,"%s",str);
	ret = read_config_next(str);
	return(ret);
}

int read_config_par_file(fptr)
FILE *fptr;
{
int file_index = 0, ret = 0;
char str[80];

	while(1)
	{
		fscanf(fptr,"%s",str);
		if((ret = read_config_next(str)))
		{
			break;
		}
		if(feof(fptr))
			break;
		strcpy(File_list[file_index].file_ident,str);
		fscanf(fptr,"%s",File_list[file_index].file_name);
		file_index++;
		if(feof(fptr))
			break;
	}
	File_list[file_index].file_ident[0] = '\0';
	return(ret);
}

int read_config_aliases(fptr)
FILE *fptr;
{
int ret = 0;
char str[80], alias[80];
OBJ *obj, *find_object();

	while(1)
	{
		fscanf(fptr,"%s",str);
		if((ret = read_config_next(str)))
		{
			break;
		}
		if(feof(fptr))
			break;
		fscanf(fptr,"%s",alias);
		if(feof(fptr))
			break;
		if((obj = find_object(str)))
	    {
			strcpy(obj->name_alias, alias);
		}
	}
	return(ret);
}

int read_config_commands(fptr)
FILE *fptr;
{
int cmd_index = 0, ret = 0;
char str[80];

	while(1)
	{
		fscanf(fptr,"%s",str);
		if((ret = read_config_next(str)))
		{
			break;
		}
		if(feof(fptr))
			break;
		strcpy(Cmd_list[cmd_index].obj,str);
		fscanf(fptr,"%s",Cmd_list[cmd_index].cmd);
		cmd_index++;
		if(feof(fptr))
			break;
	}
	Cmd_list[cmd_index].obj[0] = '\0';
	return(ret);
}

void do_open_config()
{
int next;
char par_win[80];
int i, x_len, y_len;
OBJ *obj, *find_object();
void do_parameter_window();
FILE *fptr;

	if( ( fptr = fopen( Config_name, "r" ) ) == (FILE *)0 )
  	{
    	printf("Cannot open: %s for reading\n",Config_name);
		return;
	}
	fscanf(fptr,"%d%d",&x_len, &y_len);
	Top_width = x_len;
	Top_height = y_len;

	par_win[0] = '\0';
	next = read_config_objects(fptr);
	while(next)
	{
		switch(next)
		{
			case 1:
				next = read_config_state_colors(fptr);
				break;
			case 2:
				next = read_config_par_window(fptr, par_win);
				break;
			case 3:
				next = read_config_par_file(fptr);
				break;
			case 4:
				next = read_config_aliases(fptr);
				break;
			case 5:
				next = read_config_commands(fptr);
				break;
		}
	}
	fclose(fptr);
        x_start_timer(1,do_create_objects,1);
	if(par_win[0])
	{
	  if((obj = find_object(par_win)))
	    {
	        x_start_timer(2, do_parameter_window, (long)obj);
	    }
	}
	for(i = 0; Cmd_list[i].obj[0]; i++)
	{
		smiui_send_command(Cmd_list[i].obj,Cmd_list[i].cmd);
	}
	XtSetSensitive(Cmnds[START_CONFIG], True);
	x_start_timer(1,do_set_size,0);
}

void do_parameter_window(obj)
OBJ *obj;
{
  void put_popup();
                          
    Curr_object = obj;
    put_popup(PAR_VIEW,"par_view_board",obj->name);
    /*
    put_obj_pars(obj);
    */
}

void insert_state_color(state, color)
char *state;
int color;
{
	int i, index = -1;

	for(i = 0; State_colors[i].state[0]; i++)
	{
		if(!strcmp(State_colors[i].state,state))
		{
			index = i;
		}
	}
	if(index == -1)
	{
		index = i;
		State_colors[index+1].state[0] = '\0';
	}
	strcpy(State_colors[index].state,state);
	State_colors[index].color = color;
}

int check_wild(index, state)
int index;
char *state;
{
	char str[MAX_OBJECT_NAME];
	char *ptr, *ptr1;
	int found = -1;
	int len;

	strcpy(str, State_colors[index].state);
	ptr = str;

	if(strchr(ptr,'*'))
	{
		if(*ptr == '*')
		{
			ptr++;
			if((ptr1 = strchr(ptr,'*')))
			{
				*ptr1 = '\0';
				if(strstr(state,ptr))
					found = index;
			}
			else
			{
				len = strlen(state);
				len -= strlen(ptr);
				if(!strcmp(&state[len],ptr))
					found = index;
			}
		}
		else
		{
		  if((ptr1 = strchr(ptr,'*')))
			{
				*ptr1 = '\0';
				if(!strncmp(state,ptr,strlen(ptr)))
					found = index;
			}
		}
	}
	return(found);
}

int find_state_color(state)
char *state;
{
	int i;
	int found = -1, res;

	for(i = 0; State_colors[i].state[0]; i++)
	{
		if(!strcmp(State_colors[i].state,state))
		{
			return(i);
		}
		if((res = check_wild(i, state)) != -1)
			found = res;
	}
	return(found);
}				

void cancel_pop_up (w, tag, reason)
Widget		w;
int		tag;
unsigned long	*reason;
{
  if((w) || (reason)){}
	switch(tag)
	{
		case INS_OBJ:
		case REM_OBJ:
			break;
		case SAVE_CONFIG:
		case OPEN_CONFIG:
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case COLOR:
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case PAR_VIEW:
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
			break;
		case PAR_FILE:
/*
			XtDestroyWidget(Pop_widget_id[tag]);
			Pop_widget_id[tag] = 0;
*/
			break;
	}
}

void put_label()
{
	char str[MAX_NAME];
	char code;
	int color;
	void set_color();
	void set_something();
			
	sprintf(str,"%s",Domain_name);
	if(Read_only)
		color = NONE;
	else
		color = BLUE;
	if((code = Alloc_object.state[0]))
	{
		if(code == 1)
		{
			color = GREEN;
		}
		else
		{
			color = NONE;
			strcat(str,"  used by  ");
			strcat(str,Alloc_object.state);
		}
	}
	set_color(Label_id, XmNbackground, color);
	set_something(Label_id,XmNlabelString,str);
}

void put_label_down()
{
	char str[MAX_NAME];
	void set_color();
	void set_something();
			
	sprintf(str,"%s  is down",Domain_name);
	set_color(Label_id, XmNbackground, RED);
	set_something(Label_id,XmNlabelString,str);
}

void put_popup(type,widget_name,title)
int type;
char *widget_name, *title;
{
    Widget id;
    void set_title();

    if(widget_name){}
	if(!Pop_widget_id[type])
	{
		switch(type)
		{
			case OPEN_CONFIG:
			case SAVE_CONFIG:
				id = create_file_selection_dialog(type);
				break;
			case INS_OBJ:
			case REM_OBJ:
			case PAR_FILE:
				id = create_selection_dialog(type);
				break;
		        case COLOR:
		                id = create_color_dialog();
				break;
		        case PARAM:
		                id = create_prompt_dialog();
				break;
		        case PAR_VIEW:
		                id = create_param_dialog();
				break;
		}
		Pop_widget_id[type] = id;
	}
	XtManageChild(Pop_widget_id[type]);
	set_title(XtParent(Pop_widget_id[type]),title);
}	

int insert_object(y, x, name, visible, attrs)
int y, x;
char *name;
int visible;
ATTR_LIST *attrs;
{
	OBJ *obj;
	int i;

	if(y == -1)
		obj = &Alloc_object;
	else
		obj = &Object_matrix[y][x];
	strcpy(obj->name,name);
	obj->obj_id = 0;
	obj->statechange_id = 0;
	obj->index_x = x;
	obj->index_y = y;
	obj->visible = visible;
	obj->attrs = attrs;
	obj->created = 0;
	obj->connecting = 0;
	obj->action_ids[0] = 0;
	sprintf(obj->state,"   ");
	obj->name_alias[0] = '\0';
	if(visible > 0)
	{
		N_visible_objs++;
		obj->params = 0;
		for( i=0 ; i < MAX_ACTIONS; i++)
		{
			obj->actions.params[i] = 0;
		}
	}
	return(1);
}

OBJ *find_object(obj_name)
char *obj_name;
{
 	int ind_x, ind_y;
	int get_all_rows();

	if(!strcmp(obj_name,"."))
		return(0);
	if(!strcmp(obj_name,"ALLOCATION"))
		return((OBJ *)&Alloc_object);
	for(ind_y = 0; ind_y < get_all_rows(); ind_y++)
	{
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			if(!strcmp(Object_matrix[ind_y][ind_x].name,obj_name))
				return(&Object_matrix[ind_y][ind_x]);
		}
	}
	return(0);
}

int get_visible_rows()
{
 	int rows, ind_x, ind_y;
 	int visible;

	rows = N_objects/4;
	for(ind_y = rows-1; ind_y >= 0; ind_y--)
	{
		visible = 0;
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			if(Object_matrix[ind_y][ind_x].visible > 0)
			{
				visible = 1;
				break;
			}
		}
		if(!visible)
			rows--;
		else
			break;
	}
	return(rows);
}

int get_all_rows()
{
 	int rows;

	rows = N_objects/4;
	return(rows);
}

void insert_matrix(name, visible, attrs)
char *name;
int visible;
ATTR_LIST *attrs;
{
static int Matrix_x, Matrix_y;

	if(!N_objects)
		Matrix_x = Matrix_y = 0;
	insert_object(Matrix_y,Matrix_x, name, visible, attrs);
	Matrix_x++;
	if(Matrix_x == 4)
	{
		Matrix_y++;
		Matrix_x = 0;
	}
	N_objects++;
}

void pad_matrix(pad, visible)
int pad, visible;
{
 	int n, i;

	n = 4 - (N_objects % 4);
	if( n == 4) n = 0;
	n += pad;
	for(i = 0; i < n; i++)
	{
		insert_matrix(".",visible, 0);
	}
}

void delete_matrix_objects()
{
 	int ind_x, ind_y;
/*
	XtUnmanageChild(Matrix_id);
	Matrix_managed = 0;
*/
	if(Matrix_id)
	{
		XtDestroyWidget(Matrix_id);
		Matrix_id = 0;
	}
/*
	delete_object(&Alloc_object,1);
	Alloc_object.name[0] = '\0';
*/
	for(ind_y = 0; ind_y < get_all_rows(); ind_y++)
	{
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			delete_object(&Object_matrix[ind_y][ind_x],1);
			Object_matrix[ind_y][ind_x].name[0] = '\0';

			if(Object_matrix[ind_y][ind_x].attrs)
    		{
				free(Object_matrix[ind_y][ind_x].attrs);
				Object_matrix[ind_y][ind_x].attrs = 0;
			}
		}
	}
	N_objects = 0;
}

void destroy_matrix_objects(config)
int config;
{
 	int ind_x, ind_y;
	OBJ *obj;

/*
	XtUnmanageChild(Matrix_id);
	Matrix_managed = 0;
*/
	XtDestroyWidget(Matrix_id);
	Matrix_id = 0;
/*
	delete_object(&Alloc_object,0);
*/
	for(ind_y = 0; ind_y < get_all_rows(); ind_y++)
	{
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			obj = &Object_matrix[ind_y][ind_x];
			delete_object(obj,0);
			if(config)
			{
				if(obj->visible == -1)
					obj->visible = 2;
			}
			else			
			{
				if(obj->visible == 2)
					obj->visible = -1;
			}
		}
	}
}

void change_matrix_objects(config)
int config;
{
OBJ *obj;

 	int ind_x, ind_y;
	int start, end;
	void do_change_matrix_objects();

/*
	if(Config_name[0])
	{
*/
		destroy_matrix_objects(config);
/*
	}
*/
	if(!config)
	{
		start = get_visible_rows();
		end = get_all_rows();
		for(ind_y = 0; ind_y < start; ind_y++)
		{
			for(ind_x = 0; ind_x < 4; ind_x++)
			{
				obj = &Object_matrix[ind_y][ind_x];
				if(obj->visible == -1)
					obj->visible = 0;
			}
		}
		for(ind_y = start; ind_y < end; ind_y++)
		{
			for(ind_x = 0; ind_x < 4; ind_x++)
			{
				obj = &Object_matrix[ind_y][ind_x];
				obj->visible = -1;
			}
		}
	}
	create_matrix_widget();
	XtUnmanageChild(Matrix_id);
	Matrix_managed = 0;
	x_start_timer(1, do_change_matrix_objects, (long)config);
}

void update_all_objects()
{
 	int ind_x, ind_y;
	int end;
	OBJ *obj;

	end = get_all_rows();
	for(ind_y = 0; ind_y < end; ind_y++)
	{
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			obj = &Object_matrix[ind_y][ind_x];
			if(obj->statechange_id)
			{
  				update_obj_statechange((int *)&obj, &obj);
			}
		}
	}
}

void do_change_matrix_objects(config)
int config;
{
  if(config){}
	do_create_objects(0);
	update_all_objects();
}

int list_matrix_objects(id, visible)
Widget id;
int visible;
{
OBJ *obj;
	int XmListDeleteAllItems();
	int XmListAddItem();

 	int ind_x, ind_y, index;
	char str[2048];

	XmListDeleteAllItems(id);
	index = 1;
	for(ind_y = 0; ind_y < get_all_rows(); ind_y++)
	{
		for(ind_x = 0; ind_x < 4; ind_x++)
		{
			obj = &Object_matrix[ind_y][ind_x];
			if(visible)
			{
				if(obj->statechange_id)
				{
					sprintf(str,obj->name);
					XmListAddItem(id,create_str(str),index++);
				}
			}
			else
			{
				if(!obj->statechange_id)
				{
					if(strcmp(obj->name,"."))
					{
						sprintf(str,obj->name);
						XmListAddItem(id,create_str(str),index++);
					}
				}
			}
		}
	}
	return(--index);
}

Widget create_selection_dialog(type)
int type;
{
Widget sd;
XmString xmOk, xmApply, xmCancel, xmList, xmSelection;
Arg ar[20];
int n;
        
	if(type == INS_OBJ)
	{
		xmList = create_str ("");
		xmOk = create_str ("");
		xmApply = create_str ("Insert");
		xmCancel = create_str ("Dismiss");
		xmSelection = create_str ("Insert Object");
	}
	else if(type == REM_OBJ)
	{
		xmList = create_str ("");
		xmOk = create_str ("");
		xmApply = create_str ("Remove");
		xmCancel = create_str ("Dismiss");
		xmSelection = create_str ("Remove Object");
	}
	else if(type == PAR_FILE)
	{ 
		xmList = create_str ("Parameter List :");
		xmOk = create_str ("Ok");
		xmApply = create_str ("");
		xmCancel = create_str ("Cancel");
		xmSelection = create_str ("Parameter :");
	}
    n = 0;
    /*
    XtSetArg(ar[n],XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(ar[n],XmNmwmFunctions,MWM_FUNC_MOVE); n++;
    */
    XtSetArg(ar[n],XmNtitle,"Selection"); n++;
    XtSetArg(ar[n],XmNlabelFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNtextFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNlistLabelString, xmList); n++;
    XtSetArg(ar[n],XmNokLabelString, xmOk); n++;
    XtSetArg(ar[n],XmNapplyLabelString, xmApply); n++; 
    XtSetArg(ar[n],XmNcancelLabelString, xmCancel); n++; 
    XtSetArg(ar[n],XmNselectionLabelString, xmSelection); n++;
    sd = XmCreateSelectionDialog ( toplevel_widget, "Selection", ar, n );
    XmStringFree(xmList);
    XmStringFree(xmOk);
    XmStringFree(xmApply);
    XmStringFree(xmCancel);
    XmStringFree(xmSelection);
    XtAddCallback ( sd, XmNcancelCallback, 
		    (XtCallbackProc)cancel_pop_up, (XtPointer)(long)type );
    XtAddCallback ( sd, XmNapplyCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)(long)type );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)(long)type );
    return(sd);
}

Widget create_file_selection_dialog(type)
int type;
{
Widget sd;
XmString xm1;
Arg ar[20];
int n;
        
    xm1 = create_str ("");
    n = 0;
    /*
    XtSetArg(ar[n],XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL); n++;
    XtSetArg(ar[n],XmNmwmFunctions,MWM_FUNC_MOVE); n++;
    */
    XtSetArg(ar[n],XmNtitle,"FileSelection"); n++;
    XtSetArg(ar[n],XmNlabelFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNbuttonFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNtextFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNwidth, 500); n++;
    XtSetArg(ar[n],XmNdirMask, xm1); n++;
    sd = XmCreateFileSelectionDialog ( toplevel_widget, "FileSelection", ar, n );
    
    XmStringFree(xm1);

    XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)(long)type );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)(long)type );

	return(sd);
}

Widget create_color_rowcol(parent, ar, n)
     Widget parent;
     Arg *ar;
     int n;
{
Widget rc;
        
    rc = XmCreateRowColumn ( parent, "rowcol", ar, n );

    return(rc);
}

Widget create_color_button(parent, index, str)
     Widget parent;
     int index;
     char *str;
{
Widget b;
XmString xm1;
Arg ar[20];
int n;
        
    n = 0; 
    xm1 = create_str (str);
    XtSetArg(ar[n],XmNfontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNlabelString, xm1); n++;
    b = XmCreatePushButton ( parent, "button", ar, n );
 
    XmStringFree(xm1);

    if (index == -1)
    {
      XtAddCallback ( b, XmNactivateCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)COLOR ); 
    }
    else if (index == -2)
    {
      XtAddCallback ( b, XmNactivateCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)COLOR );
    }
    else if (index == -3)
    {
      XtAddCallback ( b, XmNactivateCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)PAR_VIEW );
    }
    else
    {
      create_color(b, &index, 0);

      XtAddCallback ( b, XmNactivateCallback, 
		(XtCallbackProc)activate_color, (XtPointer)(long)index );
    }
    return(b);
}

Widget create_color_label(parent, str)
     Widget parent;
     char *str;
{
Widget b;
XmString xm1;
Arg ar[20];
int n;
        
    n = 0; 
    xm1 = create_str (str);
    XtSetArg(ar[n],XmNfontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNlabelString, xm1); n++;
    XtSetArg(ar[n],XmNalignment, XmALIGNMENT_CENTER); n++;
    if(strlen(str) > 20)
      XtSetArg(ar[n],XmNborderWidth, 2); n++;
    b = XmCreateLabel ( parent, "label", ar, n );
 
    XmStringFree(xm1);

    return(b);
}

Widget create_color_dialog()
{
Widget fd, rc, rc1, rc2, rc3, lb, bt;
Arg ar[20];
int n, par;
        
    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_ANY); n++;
    fd = XmCreateFormDialog ( toplevel_widget, "Form", ar, n );
    XtManageChild(fd);

    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNbottomOffset, 0); n++;
    XtSetArg(ar[n],XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNrightOffset, 0); n++;
    XtSetArg(ar[n],XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNtopOffset, 0); n++;
    XtSetArg(ar[n],XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNleftOffset, 0); n++;
    rc = create_color_rowcol ( fd, ar, n );
    XtManageChild(rc);

    lb = create_color_label(rc, "Color Selection");
    XtManageChild(lb);

    n = 0;
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNorientation, XmHORIZONTAL); n++;
    rc1 = create_color_rowcol(rc, ar, n);
    XtManageChild(rc1);

    n = 0;
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    rc2 = create_color_rowcol(rc1, ar, n);
    XtManageChild(rc2 );

    bt = create_color_button(rc2, 0, "                ");
    XtManageChild(bt );
    bt = create_color_button(rc2, 1, "                ");
    XtManageChild(bt );
    bt = create_color_button(rc2, 2, "                ");
    XtManageChild(bt );
    bt = create_color_button(rc2, 3, "                ");
    XtManageChild(bt );
    bt = create_color_button(rc2, 4, "                ");
    XtManageChild(bt );

    n = 0;
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    rc3 = create_color_rowcol(rc1, ar, n);
    XtManageChild(rc3 );

    lb = create_color_label(rc3, "State:");
    XtManageChild(lb );
    lb = create_color_label(rc3, " \n");
    XtManageChild(lb );
    par = 1;
    create_label(lb, &par, 0);

    lb = create_color_label(rc3, "Selected Color:");
    XtManageChild(lb );
    lb = create_color_label(rc3, "           \n     \n                ");
    XtManageChild(lb );
 
    /* height = 75
       width = 2
    */
 
    par = 5;
    create_color(lb, &par, 0);
 
    bt = create_separator(rc);
    XtManageChild(bt );

    n = 0;
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(ar[n],XmNnumColumns, 1); n++;
    XtSetArg(ar[n],XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(ar[n],XmNspacing, 120); n++;
    rc1 = create_color_rowcol(rc, ar, n);
    XtManageChild(rc1 );
    
    bt = create_color_button(rc1, -1, "Apply");
    XtManageChild(bt );
    bt = create_color_button(rc1, -2, "Dismiss");
    XtManageChild(bt );

    return(fd);
}

Widget create_prompt_dialog()
{
Widget sd;
XmString xm1;
Arg ar[20];
int n;
        
    xm1 = create_str ("PARAM");
    n = 0;
    XtSetArg(ar[n],XmNlabelFontList, smid_menu_font); n++;
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNwidth, 450); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_NONE); n++;
    XtSetArg(ar[n],XmNselectionLabelString, xm1); n++;
    sd = XmCreatePromptDialog ( toplevel_widget, "Prompt", ar, n );
    
    XmStringFree(xm1);

    XtAddCallback ( sd, XmNcancelCallback, 
		(XtCallbackProc)cancel_pop_up, (XtPointer)PARAM );
    XtAddCallback ( sd, XmNokCallback, 
		(XtCallbackProc)ok_pop_up, (XtPointer)PARAM );

	return(sd);
}

Widget create_param_dialog()
{
Widget fd, rc, rc1, lb, bt;
Arg ar[20];
int n, par;
        
    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 1); n++;
    XtSetArg(ar[n],XmNresizePolicy, XmRESIZE_ANY); n++;
    fd = XmCreateFormDialog ( toplevel_widget, "Form", ar, n );
    XtManageChild(fd);

    n = 0; 
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    XtSetArg(ar[n],XmNentryAlignment, XmALIGNMENT_CENTER); n++;
    XtSetArg(ar[n],XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNbottomOffset, 0); n++;
    XtSetArg(ar[n],XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNrightOffset, 0); n++;
    XtSetArg(ar[n],XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNtopOffset, 0); n++;
    XtSetArg(ar[n],XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(ar[n],XmNleftOffset, 0); n++;
    rc = create_color_rowcol ( fd, ar, n );
    XtManageChild(rc);

    lb = create_color_label(rc, "Parameters");
    XtManageChild(lb);

    bt = create_separator(rc);
    XtManageChild(bt );

    n = 0;
    XtSetArg(ar[n],XmNborderWidth, 0); n++;
    XtSetArg(ar[n],XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(ar[n],XmNnumColumns, 1); n++;
    XtSetArg(ar[n],XmNpacking, XmPACK_COLUMN); n++;
    rc1 = create_color_rowcol(rc, ar, n);
    XtManageChild(rc1 );

    lb = create_color_label(rc1, "");
    XtManageChild(lb );
    par = 11;
    create_label(lb, &par, 0);

    bt = create_vertical_separator(rc1);
    XtManageChild(bt );

    lb = create_color_label(rc1, "");
    XtManageChild(lb );
    par = 12;
    create_label(lb, &par, 0);

    bt = create_separator(rc);
    XtManageChild(bt );

    bt = create_color_button(rc, -3, "Dismiss");
    XtManageChild(bt );

    return(fd);
}

Widget gui_toplevel(argv)
     char **argv;
{
int n;
Arg arglist[6];
    n = 0;
    XtSetArg ( arglist[n], XmNallowShellResize, True); n++;
    XtSetArg ( arglist[n], XmNiconName, "SMI++"); n++;
    XtSetArg ( arglist[n], XmNtitle, "xSmiGui");  n++;
    /*
    XtSetArg ( arglist[n], XmNminWidth, 650); n++;
    XtSetArg ( arglist[n], XmNminHeight,300); n++;
    XtSetArg ( arglist[n], XmNgeometry,"-0+0"); n++;
    */
    /*
    XtSetArg ( arglist[n], XmNwidth, 650); n++;
    XtSetArg ( arglist[n], XmNheight,300); n++;
    */
    XtSetArg ( arglist[n], XmNtraversalOn,True); n++;
    return XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
                            display, arglist, n);
     
}

Widget gui_initialize (argc, argv)
int argc;
char **argv;
{
Widget toplevel;

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "SMIGUI",
                            NULL, 0, &argc, argv);
    if (display == NULL) 
	{
        printf("%s:  Can't open display\n", argv[0]);
        exit(1);
	}
    toplevel = gui_toplevel(argv);
   
    smid_default_font = (XmFontList)util_get_font('d', toplevel);
    smid_menu_font = (XmFontList)util_get_font('m', toplevel);
    smid_label_font = (XmFontList)util_get_font('b', toplevel);
   
    gui_create_main_window(toplevel);

    XtRealizeWidget ( toplevel );
    return toplevel;
}

Widget gui_create_main_window(parent)
     Widget parent;
{

Widget mw;
Widget mb;
Widget mf;
Widget tl;
Widget sl;
Widget f;
XmString xms;
int par;
int reason;
Arg ar[20];
int n;

    mw = XmCreateMainWindow ( parent, "SmiMainWindow", NULL, 0 );
    XtVaSetValues( mw,
        XmNresizePolicy, XmRESIZE_ANY,
		  
        XmNfontList,            smid_menu_font,
        NULL);
    XtManageChild( mw );
    /* create menu bar */
    mb = XmCreateMenuBar ( mw, "SmiMenuBar", NULL, 0 );
    XtVaSetValues( mb,
        XmNmarginHeight,2,
        XmNborderWidth, 0,
        XmNfontList,            smid_menu_font,
        NULL);
        
    gui_create_main_menu( mb );
    XtManageChild ( mb );

    /* create main form */
    mf = XmCreateForm ( mw, "SmiMainForm", NULL, 0 );
    XtVaSetValues ( mf, 
		    XmNresizePolicy, XmRESIZE_NONE, 
		    		   
		    NULL );
    XtManageChild ( mf );

    /* create top label */
    xms = create_str(" ");
    
    tl = XmCreateLabel ( mf, "SmiTitle", NULL, 0 );
    XtVaSetValues( tl,
        XmNtopAttachment,           XmATTACH_FORM,
        XmNbottomAttachment,        XmATTACH_NONE,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNleftOffset,              0,
        XmNtopOffset,               0,
        XmNbottomOffset,            0,
        XmNrightOffset,             0,
        XmNborderWidth,             0,
        XmNlabelString,             xms,          
        XmNshadowThickness,         0,
        XmNhighlightThickness,      0,
        XmNalignment,               XmALIGNMENT_CENTER,
        XmNfontList,            smid_label_font,
        NULL);
    XtManageChild( tl );
/*
    tl = XtVaCreateManagedWidget( "SmiTitle",
        xmPushButtonWidgetClass,    mw,
        XmNborderWidth,             0,
        XmNlabelString,             xms,          
        XmNshadowThickness,         0,
        XmNhighlightThickness,      0,
        XmNalignment,               XmALIGNMENT_CENTER,
        XmNfontList,            smid_label_font,
        NULL);
*/
    XmStringFree ( xms );
    /*
	XtAddCallback(tl, MrmNcreateCallback, 
		(XtCallbackProc)create_label, 0);
    */
    par = 0;
    reason = 0;
    create_label(tl, &par, &reason);

    /* create main form */
    /*
    mf = (Widget)XmCreateForm ( mw, "SmiMainForm", NULL, 0 );
    XtVaSetValues ( mf,
        XmNborderWidth,1,
        XmNshadowThickness, 2,
		    XmNwidth,806,
		    XmNheight,300, 
		    
       XmNresizePolicy, XmRESIZE_NONE, NULL );
    XtManageChild ( mf );
    */
    /*
	XtAddCallback(mf, MrmNcreateCallback, 
			(XtCallbackProc)create_main, 0);
    */
    
    create_main(mf, &par, &reason);
    
    f = XmCreateForm( mf, "ScrollForm", NULL, 0 );
    XtVaSetValues ( f, 
		    XmNwidth, 806,
		    XmNheight,300,		    		  
        XmNtopAttachment,           XmATTACH_WIDGET,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
        XmNtopWidget, tl,
        XmNtopOffset, 0,
        XmNshadowThickness, 2,
        XmNbottomOffset, 0,
		    /*
        XmNshadowType, XmSHADOW_OUT,
		    */
        XmNborderWidth,0,
        NULL);

    /*
    f = XtVaCreateManagedWidget ( "XDScrolledForm",
        xmFormWidgetClass,          mf,
        XmNtopAttachment,           XmATTACH_WIDGET,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
        XmNtopWidget, tl,
        XmNtopOffset, 0,
        XmNshadowThickness, 2,
        XmNbottomOffset, 0,
        XmNshadowType, XmSHADOW_OUT,
        XmNborderWidth,1,
        NULL);
*/
    /*
	XtAddCallback(f, MrmNcreateCallback, 
		(XtCallbackProc)create_window, 0);
    */

    XtManageChild ( f );
    /* create scrolled list */
    
    n = 0;
    XtSetArg ( ar[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg ( ar[n], XmNrightOffset, 0); n++;
    XtSetArg ( ar[n], XmNleftOffset, 0); n++;
    XtSetArg ( ar[n], XmNbottomOffset, 0); n++;
    XtSetArg ( ar[n], XmNtopOffset, 0); n++;
    /*
    XtSetArg ( ar[n], XmNvisualPolicy, XmCONSTANT); n++;
    */	    
    XtSetArg ( ar[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED); n++;
		   
    XtSetArg ( ar[n], XmNscrollingPolicy, XmAUTOMATIC); n++;

    sl = XmCreateScrolledWindow ( f, "ScrollWin", ar, n );
    /*
    XtVaSetValues ( sl, 
        XmNtopAttachment,           XmATTACH_FORM,
        XmNbottomAttachment,        XmATTACH_FORM,
        XmNleftAttachment,          XmATTACH_FORM,
        XmNrightAttachment,         XmATTACH_FORM,
        XmNrightOffset,             0,
        XmNleftOffset,              0,
        XmNbottomOffset,            0,
	XmNtopOffset,               0,
		    		   
        XmNvisualPolicy,        XmCONSTANT,
		    
	XmNscrollBarDisplayPolicy, XmSTATIC,
		   
        XmNscrollingPolicy, XmAUTOMATIC,

        NULL);
    */
    XtManageChild ( sl );
    create_window(sl, &par, &reason);
    /*    
    create_matrix_widget();
    */
/* 
    sl = XtVaCreateWidget ( "DidServersScrl",
        xmScrolledWindowWidgetClass, f,
        XmNscrollingPolicy,     XmAUTOMATIC,
        XmNscrollBarDisplayPolicy, XmSTATIC,
        XmNtopAttachment,       XmATTACH_FORM,
        XmNleftAttachment,      XmATTACH_FORM,
        XmNrightAttachment,     XmATTACH_FORM,
        XmNbottomAttachment,    XmATTACH_FORM,
        XmNvisualPolicy,        XmCONSTANT,
        XmNtopOffset,           4,
        XmNleftOffset,          4,
        XmNbottomOffset,        4,
        XmNrightOffset,         4,
        NULL );
    XtManageChild ( sl );
*/
    /*
    XtVaSetValues( mw,
        XmNworkWindow,mf,
        XmNcommandWindow, tl,
        NULL);
    */
    return(0);
}

void gui_create_main_menu( mb )
     Widget mb;
{
Widget cb;
Widget mn;
Widget bt;
XmString xms, xma;
int par;
int reason;

    /* File */
    mn = XmCreatePulldownMenu ( mb, "FileMB", NULL, 0 );
    cb = XmCreateCascadeButton(mb, "File", NULL, 0);
    XtVaSetValues ( cb, 
        XmNsubMenuId,       mn,
        NULL);
    XtManageChild ( cb );
/*
    cb = XtVaCreateManagedWidget ( "File",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "F", 2,
        XmNsubMenuId,       mn,
        NULL);
*/
        XtVaSetValues ( mn,
            XmNradioAlwaysOne, True,
            XmNradioBehavior, True,
            NULL);
        XtVaSetValues ( cb,
	    XmNfontList,            smid_menu_font,
            NULL);
	/*
        XtAddCallback ( mb, MrmNcreateCallback, 
			(XtCallbackProc)create_file_button, 0 );
	*/
	par = 0;
	reason = 0;
	create_file_button(mn, &par, &reason);

        /* buttons */

        xms = create_str ("Open Full Domain");
        xma = create_str ("Ctrl+F");
/*
        bt = XtVaCreateManagedWidget ( "V_MenuFullButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>F",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
            NULL);
*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
            NULL);
        XtManageChild( bt );


        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, OPEN_FULL );
	*/
	par = OPEN_FULL;
	reason = 0;
	create_command(bt, &par, &reason);

        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, OPEN_FULL );

        xms = create_str ("Open Configuration");
        xma = create_str ("Ctrl+C");
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
            NULL);
        XtManageChild( bt );
/*
        bt = XtVaCreateManagedWidget ( "V_MenuConfButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>C",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
            NULL);
*/
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, (XtPointer)OPEN_CONFIG );
	*/
	par = OPEN_CONFIG;
	reason = 0;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)OPEN_CONFIG );

        xms = create_str ("Save Configuration");
        xma = create_str ("Ctrl+S");
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
            NULL);
        XtManageChild( bt );
/*
        bt = XtVaCreateManagedWidget ( "V_MenuSaveButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>S",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
            NULL);
*/
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, SAVE_CONFIG );
	*/
	par = SAVE_CONFIG;
	reason = 0;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)SAVE_CONFIG );

    /* Configure */
    mn = XmCreatePulldownMenu ( mb, "ConfigureMB", NULL, 0 );
    cb = XmCreateCascadeButton(mb, "Configure", NULL, 0);
    XtVaSetValues ( cb, 
        XmNsubMenuId,       mn,
        NULL);
    XtManageChild ( cb );
/*
    cb = XtVaCreateManagedWidget ( "Configure",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "C", 2,
        XmNsubMenuId,       mn,
        NULL);
*/
        XtVaSetValues ( mn,
            XmNradioAlwaysOne, True,
            XmNradioBehavior, True,
            NULL);
        XtVaSetValues ( cb,
	    XmNfontList,            smid_menu_font,
            NULL);
	/*
        XtAddCallback ( mn, MrmNcreateCallback, 
			(XtCallbackProc)create_file_button, 0 );
	*/

        /* buttons */

        xms = create_str ("Start Configure Mode");
        xma = create_str ("Ctrl+C");
/*
        bt = XtVaCreateManagedWidget ( "V_MenuStartButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>C",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False, 
            NULL);
*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False,
            NULL);
        XtManageChild( bt );

        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, START_CONFIG );
	*/
	par = START_CONFIG;
	reason = 0;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)START_CONFIG );

        xms = create_str ("Quit Configure Mode");
        xma = create_str ("Ctrl+Q");
/*
        bt = XtVaCreateManagedWidget ( "V_MenuQuitButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>Q",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False, 
            NULL);
*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, QUIT_CONFIG );
	*/
	par = QUIT_CONFIG;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)QUIT_CONFIG );

        xms = create_str ("Remove Object");
        xma = create_str ("Ctrl+R");
/*
        bt = XtVaCreateManagedWidget ( "V_MenuRemButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>R",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False, 
            NULL);
*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, REM_OBJ );
	*/
	par = REM_OBJ;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)REM_OBJ );

        xms = create_str ("Insert Object");
        xma = create_str ("Ctrl+I");
/*
        bt = XtVaCreateManagedWidget ( "V_MenuInsertButton",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>I",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False, 
            NULL);
*/
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
	    XmNsensitive, False,
            NULL);
        XtManageChild( bt );
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, MrmNcreateCallback, 
			(XtCallbackProc)create_command, INS_OBJ );
	*/
	par = INS_OBJ;
	create_command(bt, &par, &reason);
        XtAddCallback ( bt, XmNactivateCallback, 
			(XtCallbackProc)smid_config, (XtPointer)INS_OBJ );


    
    /* Help */
    mn = XmCreatePulldownMenu ( mb, "HelpMB", NULL, 0 );
/*
    cb = XtVaCreateManagedWidget ( "Help",
        xmCascadeButtonWidgetClass, mb,
        XtVaTypedArg,       XmNmnemonic, XmRString, "H", 2,
        XmNsubMenuId,       mn,
	XmNfontList,            smid_menu_font,
        NULL);
*/
        cb = XmCreateCascadeButton ( mb, "Help", NULL, 0 );
        XtVaSetValues( cb,
	    XmNsubMenuId,       mn,
	    XmNfontList,            smid_menu_font,
            NULL);
        XtManageChild( cb );
        xms = create_str ("About XD");
        xma = create_str ("Ctrl+H");
        bt = XmCreatePushButton ( mn, "button", NULL, 0 );
        XtVaSetValues( bt,
            XmNlabelString, xms,
	    XmNfontList,            smid_menu_font,
            NULL);
        XtManageChild( bt );
/*
        bt = XtVaCreateManagedWidget ( "C_XDAbout",
            xmPushButtonWidgetClass, mn,
            XmNlabelString, xms,
            XmNaccelerator, "Ctrl<Key>H",
            XmNacceleratorText, xma,
	    XmNfontList,            smid_menu_font,
            NULL);
*/
        XmStringFree ( xms );
        XmStringFree ( xma );
	/*
        XtAddCallback ( bt, XmNactivateCallback, (XtCallbackProc)about_xd_callback, NULL );
	*/
    /* set help menu */
    XtVaSetValues ( mb, XmNmenuHelpWidget, cb, NULL );
}

XmFontList util_get_font( size, top )
     char size;
     Widget top;
{
XFontStruct * mf;
XmFontList font;
char * fontname;

  if ( size == 'm' ) {
    fontname = MENU_FONT;
  }
  else if ( size == 'b' ) {
    fontname = LABEL_FONT;
  }
  else {
    fontname = DEFAULT_FONT;
  }
  if ( (mf = XLoadQueryFont(XtDisplay(top),fontname))==NULL) {
        printf("Couldn't open the following fonts:\n\t%s\n",
	    fontname);
        XtVaGetValues ( top, XmNdefaultFontList, &font, NULL );
  }
  else  {
     font = XmFontListCreate (mf, XmSTRING_DEFAULT_CHARSET);
  }
  return font;
}

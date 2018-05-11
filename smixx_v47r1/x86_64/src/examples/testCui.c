#include <stdio.h>
#include <smiuirtl.h>

void state_cb(int *id)
{
	char obj[132], state[132];
	int busy, n;

	smiui_get_name(*id, obj);
	smiui_get_state(*id, &busy, state, &n);
	printf("\tOBJECT: %s in_state %s\n",obj, state);
}

void set_cb(int *id)
{
	char set[132], obj[132];

	smiui_get_setname(*id, set);
	printf("SET: %s\n",set);
	if(smiui_get_first_object_in_set(*id, obj))
	{
		printf("\t%s\n",obj);
		while(smiui_get_next_object_in_set(*id, obj))
		{
			printf("\t%s\n",obj);
		}
	}
}

void domain_cb(char **name, int *nobjs)
{
	char str[132];
	static int *statechange_ids = 0, *setchange_ids = 0;
	int i, nobjsets;

	printf("DOMAIN: %s",*name);
	if(*nobjs)
	{
		printf(" up\n");
		printf("OBJECTS %d:\n", *nobjs);
		statechange_ids = (int *)malloc(((*nobjs)+1) * sizeof(int));
		i = 0;
		while(smiui_get_next_object(*name,str))
		{
			printf("\t%s\n",str);
			statechange_ids[i++] = smiui_book_statechange(str, state_cb, 0);
		}
		statechange_ids[i] = 0;
	}
	else
	{
		printf(" down\n");
		if(statechange_ids)
		{
			for(i = 0; statechange_ids[i]; i++)
				smiui_cancel_statechange(statechange_ids[i]);
			free(statechange_ids);
			statechange_ids = 0;
		}
	}
	nobjsets = smiui_number_of_objectsets(*name);
	if(nobjsets)
	{
		printf("OBJECTSETS:\n");
		setchange_ids = (int *)malloc((nobjsets+1) * sizeof(int));
		i = 0;
		while(smiui_get_next_objectset(*name,str))
		{
			printf("\t%s\n",str);
			setchange_ids[i++] = smiui_book_objectsetchange(str, set_cb, 0);
		}
		setchange_ids[i] = 0;
	}
	else
	{
		if(setchange_ids)
		{
			for(i = 0; setchange_ids[i]; i++)
				smiui_cancel_objectsetchange(setchange_ids[i]);
			free(setchange_ids);
			setchange_ids = 0;
		}
	}
}

main(int argc, char *argv[])
{
	char name[132];
	void *cmd;

	strcpy(name, argv[1]);
	smiui_book_connect_domain(name, domain_cb, name);
	sleep(10);
	cmd = smiui_create_command("START");
	smiui_add_param_to_command(cmd, "TYPE","PHYSICS",SMI_STRING);
	smiui_send_command("RUNCON::EVT_BUILDER",smiui_get_command_string(cmd));
	smiui_delete_command(cmd);
	while(1)
		pause();
}

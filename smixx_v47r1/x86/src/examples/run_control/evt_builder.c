#include <smirtl.h>
//#include <dim.h>

int Timer_on = 0;

main(argc, argv)
int argc;
char **argv[];
{
	int num;

	char proxy_name[128];
	void handle_command();
	smi_volatile();
	sprintf(proxy_name,"%s::EVT_BUILDER",argv[1]);
	smi_attach(proxy_name, handle_command);
	num = 1;
	smi_set_par("NUMBER_T", &num, SMI_INTEGER);
	smi_set_par("NUMBER_P", &num, SMI_INTEGER);
	smi_set_state("READY");
/*
	smi_set_state("READY");

	smi_set_state("RUNNING");

	smi_set_state("ERROR");

*/
	while(1)
	{
		pause();
	}
}

set_in_error(tag)
int tag;
{
	smi_set_state("ERROR");
	Timer_on = 0;
}

void handle_command()
{
	char type[32];
	int num;
	char state[132];

	smi_get_state(state,132);
	printf("Current_state is %s\n",state);
	if(smi_test_action("START"))
	{
		dtq_start_timer(/*rand_tmout(10,30)*/2,set_in_error,0);
		smi_get_par_value("TYPE",type);
		smi_get_par_value("NR",&num);
		if(!strcmp(type,"TEST"))
		{
			num++;
			smi_set_par("NUMBER_T",&num,SMI_INTEGER);
		}
		else
		{
			num++;
			smi_set_par("NUMBER_P",&num,SMI_INTEGER);
		}
		Timer_on = 1;
		/*
		sleep(1);
		*/
		smi_terminate_action("RUNNING");
	}
	else if(smi_test_action("STOP"))
	{
		if(Timer_on)
			dtq_stop_timer(0);
		sleep(1);
		smi_terminate_action("READY");
	}
	else if(smi_test_action("RECOVER"))
	{
		/*
		sleep(3);
		*/
		smi_terminate_action("READY");
	}
/*
	smi_terminate_action("READY");

	smi_terminate_action("RUNNING");

	smi_terminate_action("ERROR");

*/
}

#ifndef unix
int rand_tmout( min, max )
int min, max;
{
	int aux;

	aux = rand();
	aux %= (max - min);
	aux += min;
	return(aux);
}
#endif

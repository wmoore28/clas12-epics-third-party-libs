#include <stdio.h>
#include <smirtl.h>

main(argc, argv)
int argc;
char **argv[];
{

	char proxy_name[64];
	void handle_command();
	smi_volatile();
	sprintf(proxy_name,"%s::LOGGER",argv[1]);
	smi_attach(proxy_name, handle_command);
	smi_set_state("NOT_LOGGING");
/*
	smi_set_state("NOT_LOGGING");

	smi_set_state("LOGGING");

	smi_set_state("WRITING");

*/
	while(1)
	{
		pause();
	}
}

void kick_it()
{
	smi_set_state("NOT_LOGGING");
}

void handle_command()
{

	if(smi_test_action("LOG"))
	{
		smi_terminate_action("LOGGING");
	}
	if(smi_test_action("KICK"))
	{
		dtq_start_timer(1, kick_it, 0);
		smi_terminate_action("LOGGING");
	}
	else if(smi_test_action("NOLOG"))
	{
		smi_terminate_action("NOT_LOGGING");
	}
	else if(smi_test_action("X_OPEN_FILE"))
	{
		smi_terminate_action("WRITING");
	}
	else if(smi_test_action("X_CLOSE_FILE"))
	{
		smi_terminate_action("LOGGING");
	}
/*
	smi_terminate_action("NOT_LOGGING");

	smi_terminate_action("LOGGING");

	smi_terminate_action("WRITING");

*/
}

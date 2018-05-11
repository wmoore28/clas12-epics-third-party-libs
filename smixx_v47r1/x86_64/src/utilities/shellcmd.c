/*------------------------------------------------------------------------
     Proxy for execution of shell commands
                                        Boda Franek   16 June 1999
--------------------------------------------------------------------------*/
#include "stdlib.h"
#include "stdio.h"
#include "smirtl.h"

#define MAXNAME 80
#include <string.h>

int main(int argc, char **argv)
{
	if ( argc == 0 ) {}

	static char proxy_name[MAXNAME+1] = "SHELLCMD";
        char domain[MAXNAME+1];
        char proxy_fullname[2*MAXNAME+2+1];
	void handle_command();

/* read the domain name from the command line */
	sprintf(domain,"%s",argv[1]);

/* form full name of the proxy ; format :   'domain-name'::'proxy-name' */

        strcpy(proxy_fullname,domain);
        strcat(proxy_fullname,"::");
        strcat(proxy_fullname,proxy_name);

/* the following call will attach the proxy to the mirror( associated) object 
   in the SMI domain;  handle_command is call-back routine that is called
   every time a SMI command arrives  */

	smi_attach(proxy_fullname, handle_command); 
        smi_volatile();
        smi_set_state("READY");

	while(1)
	{
		dim_wait();
	}
}

void handle_command()
{

        char action[MAXNAME+1];
        int noOfParameters;

        char shellCommand[256],parValue[133],shellCommand1[256];


        smi_get_action(action,&noOfParameters);        
/*	printf("SMI command  action name = %s\n",action);
        printf(" No. of parameters = %d\n",noOfParameters);*/
        
        smi_get_par_value("COMMAND",shellCommand);
/*        printf(" Parameter : %s\n",shellCommand);*/
        
        smi_get_par_value("PARAMETER",parValue);
/*        printf(" Parameter : %s\n",parValue);*/
        
        strcpy(shellCommand1,shellCommand);
        strcat(shellCommand1," ");
        strcat(shellCommand1,parValue);
        
        system(shellCommand1);
        
        
        smi_terminate_action("READY");


}

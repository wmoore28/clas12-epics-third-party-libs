#include <stdlib.h>
#include <stdio.h>
#include <smiuirtl.h>


void handler(int* pId, long* pParam)
{
	int Id; long Param;
/*	char objsetName[32]; */
	char objName[32];
	int iflg;

  
	Id =  *pId;
	Param = *pParam;
  
/*  printf("    (*pId)  : %d      (*pParam) : %d \n",Id,Param);
  smiui_get_setname(Id,objsetName);
printf("%s",objsetName);*/
  
	smiui_get_first_object_in_set(Id,objName);
	printf("%s\n",objName);
  
	while (1)
	{ 
		iflg = smiui_get_next_object_in_set(Id,objName);
	 	/* iflg = 1 normal return;  iflg = 0 when no more objects;*/
	
		if (!iflg) { fflush(stdout); exit(EXIT_SUCCESS);}
  		printf("%s\n",objName);
  	}

	return;
}


int main(int argc, char* argv[])
{
	if ( argc == 0 ) {}
	
        int Id; long param = 12345;

	Id = smiui_book_objectsetchange( argv[1], handler ,param);
			
	sleep(10);

	exit(EXIT_FAILURE);
	return 0;
}

//----------------------   M a i n   P r o g r a m  --------------------------

#include <stdlib.h>
#include "smixx_common.hxx"
#include <ctype.h>


#define MAXNAMESIZE 133

#include "smiuirtl.hxx"

int main(int , char* argv[]) {

  char smiDomainStr[MAXNAMESIZE];

  if ( strlen(argv[1]) > (MAXNAMESIZE-1) ) {
     cout << " The domain name is too long " << endl;
     cout.flush();
     exit(2);
  }

    int i;
    int stringLength = strlen(argv[1]);
    
    for(i = 0; i < stringLength; i++) {
          smiDomainStr[i] = toupper(argv[1][i]);
    }
    smiDomainStr[i] = '\0';

  cout << "               SMI Domain : " << smiDomainStr << "\n \n";


   int numOfObjects = smiui_connect_domain(smiDomainStr);

   cout << " Num. of objects : " << numOfObjects << endl << endl;
   cout.flush();

   for ( int io=0; io < numOfObjects; io++ ) {
      char object_nm[MAXNAMESIZE],state_nm[MAXNAMESIZE],action_nm[MAXNAMESIZE];
      int iflg = smiui_get_next_object(smiDomainStr,object_nm) ;
      smiui_current_state(object_nm,state_nm,action_nm);

      cout << io << "  " << object_nm << "  " << state_nm << " " << action_nm << endl;
      if ( iflg == 0 ) {break;}
   }

   exit(0);


}

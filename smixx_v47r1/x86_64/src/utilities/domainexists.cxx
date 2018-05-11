//----------------------   M a i n   P r o g r a m  --------------------------

extern "C" int smiui_connect_domain(char* smiDomainStr);

#include <stdlib.h>
#include "smixx_common.hxx"
#include <string.h>
#include <ctype.h>

#define MAXNAMESIZE 133


int main(int , char* argv[]) {


  char smiDomainStr[MAXNAMESIZE];
  int domainNameLength = strlen(argv[1]);
  
  if ( domainNameLength > (MAXNAMESIZE-1) ) {
     cout << " The domain name is too long " << endl;
     cout.flush();
     exit(1);
  }

    int i;
   
    for(i = 0; i < domainNameLength; i++) {
          smiDomainStr[i] = toupper(argv[1][i]);
    }
    smiDomainStr[i] = '\0';

    int numOfObjects = smiui_connect_domain(smiDomainStr);
    if ( numOfObjects == 0 ) {
        cout << "setenv SMI_DOMAIN_EXISTS no " ;
    }
    else {
        cout << "setenv SMI_DOMAIN_EXISTS yes " ;
    }
    
   exit(0);


}

//----------------------   M a i n   P r o g r a m  --------------------------

extern "C" int smiui_check_proxy(char* proxyStr);

#include <stdlib.h>
#include "smixx_common.hxx"
#include <string.h>
#include <ctype.h>

#define MAXNAMESIZE 133


int main(int , char* argv[]) {


  char proxyStr[MAXNAMESIZE];

  int proxyNameLength = strlen(argv[1]);
  
  if ( proxyNameLength > (MAXNAMESIZE-1) ) {
     cout << " The proxy name is too long " << endl;
     cout.flush();
     exit(2);
  }

    int i;
    for(i = 0; i < proxyNameLength; i++) {
          proxyStr[i] = toupper(argv[1][i]);
    }
    proxyStr[i] = '\0';

    int check = smiui_check_proxy(proxyStr);
    if ( check == 0 ) {
        cout << "setenv SMI_PROXY_EXISTS no " ;
    }
    else {
        cout << "setenv SMI_PROXY_EXISTS yes " ;
    }
    
   exit(0);


}

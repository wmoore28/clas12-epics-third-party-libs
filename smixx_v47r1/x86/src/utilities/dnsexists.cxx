#include <stdlib.h>
#include <dic.hxx>
#include "smixx_common.hxx"

int main()
{
	int version = 0;
	DimCurrentInfo dns("DIS_DNS/VERSION_NUMBER",2,-1);
	
	version = dns.getInt();
	if(version == -1) {
              cout << " DIM name server is not running" << endl;
              exit(11);
        }
	else {
              cout << " DIM name server is running" << endl;
              exit(22);
        }
}

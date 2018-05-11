#include <stdlib.h>
#include <dic.hxx>
#include <iostream>
#include <string>
using namespace std;

int main()
{
	string S1("Hello");
//	cout << S1 << endl;

	int version = 0;
	DimCurrentInfo dns("DIS_DNS/VERSION_NUMBER",5,-1);
	
	version = dns.getInt();
	if(version == -1) {
              cout << "NO" ;
//              exit(11);
        }
	else {
              cout << "YES";
 //             exit(22);
        }
	return 0;
}

#include <iostream>
#include <dic.hxx>
#include <string>
using namespace std;


int main()
{
	string smiDomain;
    char *server, *ptr;

    DimBrowser br;

	br.getServers();
	while(br.getNextServer(server, ptr))
	{
		int ll = strlen(server);

		if ( ll > 4 && strcmp(&server[ll-4],"_SMI") == 0 ) {
			smiDomain = server;
			smiDomain.erase(ll-4,4);
//			smiDomain.copy(server,ll-4);
			cout << smiDomain  << "/" << ptr << endl;
		}
	}

	return 0;
}

#include <iostream>
#include <dic.hxx>
#include <string>
using namespace std;


int main()
{
	string smiDomain;
    char *service, *format;
    int type;

    DimBrowser brsm, br;

    brsm.getServices("*_SMI/VERSION_NUMBER");
    while( type = brsm.getNextService(service,format) )
    {
        DimCurrentInfo ver(service,-1);

        cout << ver.getInt() << "  " << service << endl;    
    }

    br.getServices("*::*/VERSION_NUMBER");
    while( type = br.getNextService(service,format) )
    {
        DimCurrentInfo ver(service,-1);

        cout << ver.getInt() << "  " << service << endl;    
    }

    return 0;
}

//----------------------   M a i n   P r o g r a m  --------------------------
//extern "C" int smiui_connect_domain(char* smiDomainStr);
#include <iostream>
#include <string>
using namespace std;
#include <stdlib.h>
#include <smiuirtl.hxx>
#include "smixx_common.hxx"

#include <ctype.h>

//==========================================================================
class MyObject: public SmiObject
{    
    void  smiStateChangeHandler();
    void smiExecutingHandler();
    
public:
    MyObject(char* fullObjName): SmiObject(fullObjName) { }
};
//==========================================================================
void MyObject::smiStateChangeHandler() {
	char *state = getState();
	char *name = getName();

	if(state != 0)
		cout << getName() << " in state " << getState() << endl;
	else
		cout << getName() << " DEAD " << endl;
}
//===========================================================================
void MyObject::smiExecutingHandler() {
    cout << getName() << " Busy" << endl;
}
//===========================================================================
int main(int argc, char* argv[]) {

    string fullObjectName;
    string smiDomain;
    string objectName;

    if ( argc <= 1 ) {
        cerr << "Error No object specified" << endl;
        exit(1);
    }
    
    if ( getenv("DIM_DNS_NODE") == 0 ) {
        cerr << "***Error getObjectState DIM_DNS_NODE is undefined" << endl;
        exit(11);
    } 
    
    DimCurrentInfo dns("DIS_DNS/VERSION_NUMBER",2,-1);
    if ( dns.getInt() == -1 ) {
        cerr << "***Error getObjectState Name server is not running" << endl;
        exit(12);
    }
    
    int i;
    for(i = 0; i < strlen(argv[1]); i++) {
          fullObjectName += toupper(argv[1][i]);
    }

    int ipos = fullObjectName.find("::");

    if ( ipos < 0 ) {
        cerr << "***Error Domain not specified" << endl;
        exit(2);
    }
    
    smiDomain = fullObjectName.substr(0,ipos);
    
    objectName = fullObjectName.substr(ipos+2,fullObjectName.length());

    char* ptnr = const_cast<char*> (smiDomain.c_str());
    int numOfObjects = smiui_connect_domain(ptnr);
    if ( numOfObjects == 0 ) {
        cerr << "***Error Domain is not running" << endl;
        exit(3);
    }

    ptnr = const_cast<char*> (fullObjectName.c_str());
    
    MyObject object(ptnr);
    

    if ( object.getState() == 0 ) {
        cerr << "***Error Object does not exists" << endl;
        exit(4);
    }

/*    int busyflg = object.getBusy();
    
    if ( busyflg == 1 ) {
        cout <<"Busy";
        exit(0);
    }
    
    cout << object.getState() ;*/
    while ( 1 ) { pause();}
    
    exit(0);


}

//----------------------   M a i n   P r o g r a m  --------------------------
// $Id
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
    char* state = getState();
    if (state != 0) {
        cout << getName() << " in state " << state << endl;
    }
    else {
        cout << getName() << " in state " << "*DEAD" << endl;
    }
}
//===========================================================================
void MyObject::smiExecutingHandler() {
    if ( getBusy() ) {
        cout << getName() << " Busy" << endl;
    }
}
//===========================================================================
int main(int argc, char* argv[]) {
// waitObjectNotBusy 'domain'::'object  'timeout'
//         0                  1             2

    string fullObjectName;
    string smiDomain;
    string objectName;
    int timeout;

    if ( argc <= 1 ) {
        cerr << "Error No object specified" << endl;
        exit(1);
    }
    
    if ( argc >= 3 ) {
        sscanf(argv[3],"%d",timeout);
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
    
    SmiDomain domain(ptnr);
    
    int numOfObjects = domain.getNObjects();
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

    while ( 1 ) { sleep(10);}
    
    exit(0);


}

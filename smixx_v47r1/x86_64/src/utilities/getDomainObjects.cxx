#include <iostream>
#include <string>
#include <smiuirtl.hxx>
using namespace std;

class Domain: public SmiDomain
{
	public:
		Domain(char* name): SmiDomain(name) {};
};

int main(int , char* argv[])
{
	Domain *domainp;
	domainp = new Domain(argv[1]);

//	cout << domainp->getNObjects() << endl;

	SmiObject* pSmiObject;
	
	while ( (pSmiObject = domainp->getNextObject()) )
	{
		cout << pSmiObject->getName() << endl;
	}
	
	return 0;
}

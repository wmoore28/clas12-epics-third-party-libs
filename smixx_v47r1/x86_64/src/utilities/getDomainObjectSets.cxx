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

	SmiObjectSet* pSmiObjectSet;
	
	while ( (pSmiObjectSet = domainp->getNextObjectSet()) )
	{
		cout << pSmiObjectSet->getName() << endl;
	}
	
	return 0;
}

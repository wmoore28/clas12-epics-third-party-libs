#include "logger.hxx"

class LOGGER: public LOGGERProxy
{
protected :
// Actions
	virtual void LOG();
	virtual void NOLOG();
	virtual void X_OPEN_FILE();
	virtual void X_CLOSE_FILE();
public:
// Constructors
	LOGGER(char *domain): LOGGERProxy(domain) { };
};

void LOGGER::LOG()
{
//	User code for action LOG
	setLOGGING();
}

void LOGGER::NOLOG()
{
//	User code for action NOLOG
	setNOT_LOGGING();
}

void LOGGER::X_OPEN_FILE()
{
//	User code for action X_OPEN_FILE
	setWRITING();
}

void LOGGER::X_CLOSE_FILE()
{
//	User code for action X_CLOSE_FILE
	setLOGGING();
}

int main(int argc, char *argv[])
{

	LOGGER logger(argv[1]);
	logger.setNOT_LOGGING();
	while(1)
	{
		pause();
	}
}


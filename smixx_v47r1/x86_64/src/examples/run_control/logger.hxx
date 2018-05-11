#include <smirtl.hxx>

class LOGGERProxy: public SmiProxy
{
protected :
// Actions
	virtual void LOG() {};
	virtual void NOLOG() {};
	virtual void X_OPEN_FILE() {};
	virtual void X_CLOSE_FILE() {};
// Command Handler
	void smiCommandHandler();
public:
// States
	void setNOT_LOGGING() {setState("NOT_LOGGING");};
	void setLOGGING() {setState("LOGGING");};
	void setWRITING() {setState("WRITING");};
// Object Parameters
// Constructors
	LOGGERProxy(char *domain): SmiProxy("LOGGER") { attach(domain);};
};

void LOGGERProxy::smiCommandHandler()
{
	if(testAction("LOG"))
		LOG();
	else if(testAction("NOLOG"))
		NOLOG();
	else if(testAction("X_OPEN_FILE"))
		X_OPEN_FILE();
	else if(testAction("X_CLOSE_FILE"))
		X_CLOSE_FILE();
}

#include <smirtl.hxx>

class EVT_BUILDER: public SmiProxy
{
protected :
/* Actions */
#ifdef SMI_INHERIT
	virtual void START() {};
	virtual void STOP() {};
	virtual void RECOVER() {};
#else
	virtual void START();
	virtual void STOP();
	virtual void RECOVER();
#endif
/* Parameters of action START */
	char *getParTYPE() {return getParameterString("TYPE");};
	int getParNR() {return getParameterInt("NR");};
/* Command Handler */
	void smiCommandHandler();
public:
/* States */
	void setREADY() {setState("READY");};
	void setRUNNING() {setState("RUNNING");};
	void setERROR() {setState("ERROR");};
/* Object Parameters */
	int setParNUMBER_T(int val) {return setParameter("NUMBER_T", val);};
	int setParNUMBER_P(int val) {return setParameter("NUMBER_P", val);};
/* Constructors */
	EVT_BUILDER(): SmiProxy("EVT_BUILDER") {};
	EVT_BUILDER(char *domain): SmiProxy("EVT_BUILDER") { attach(domain);};
};

void EVT_BUILDER::smiCommandHandler()
{
	if(testAction("START"))
		START();
	else if(testAction("STOP"))
		STOP();
	else if(testAction("RECOVER"))
		RECOVER();
}

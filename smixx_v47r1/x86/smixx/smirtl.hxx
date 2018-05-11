#ifndef __SMIRTLHHDEFS
#define __SMIRTLHHDEFS

#include <dis.hxx>
#include <sllist.hxx>
#include <tokenstring.hxx>
#include "smirtl_core.hxx"

#ifdef WIN32
#ifdef SMILIB
#	define SmiDllExp __declspec(dllexport)
#else
#	define SmiDllExp __declspec(dllimport)
#endif
#else
#	define SmiDllExp
#endif

#ifndef SMI_PAR_TYPES
#ifndef WIN32
typedef enum { STRING, INTEGER, FLOAT } SHORT_PAR_TYPES;
#endif
typedef enum { SMI_STRING, SMI_INTEGER, SMI_FLOAT } PAR_TYPES;
#define SMI_PAR_TYPES
#endif

class ParInfo : public SLLItem {
	char *itsName;
	int itsType;
	char *itsData;
	char *itsEscData;
	char itsTypeStr[8];
public:
	ParInfo(char *name, int type);
	~ParInfo();
	int setInfo(void *data, int type);
	int setStrInfo(char *data, int type);
	char *getName() { return itsName; }
	int getType() { return itsType; }
	int getDataSize() { return strlen(itsData)+1; }
	int getEscDataSize() { return strlen(itsEscData)+1; }
	char *getTypeStr() { return itsTypeStr; }
	char *getData() { return itsData; }
	char *getEscData() { return itsEscData; }
	
};

class SmiDllExp SmiProxy: public DimCommandHandler, public DimClientExitHandler
{
private:
	char *itsDomain;
	char *itsName;
	char *itsFullName;
	char *itsState;
	char *itsShortState;
	char *itsAction;
	char *itsCommand;
	char *itsParValue;
	DimService *itsDimState;
	DimCommand *itsDimCmnd;
	DimService *itsDimVersion;
	void capitalize(char *str);
	SLList itsParList;
	SLList itsCmndParList;
	ParInfo *currCmndPar;
//	int findPar(char *name);
	int addPar(char *name, void *value, int type);
	TokenString *cmndTokens;
	int getType(char *str);
	int getStateLen(char *state);
	void formState(char *state);
	int storeCmndPar();
	char *getCmndPar(char *name, int type);
	void publish_it(int later);
	void clientExitHandler() {smiDomainExitHandler();}
	void commandHandler();
	int infoPrint;
public:
	void attach(char *domain);
	void attach(char *domain, char *name);
	void attach(char *domain, char *name, int later);
	void setVolatile();
	int setState(char *state);
	char *getState();
	int setParameter(char *parName, int parValue);
	int setParameter(char *parName, double parValue);
	int setParameter(char *parName, char *parValue);
	int getParameterInt(char *parName);
	double getParameterFloat(char *parName);
	char *getParameterString(char *parName);
	int getParameterType(char *parName);
	char *getNextParameter();
	int testAction(char *action);
	char *getAction();
	char *getCommand() {return itsCommand;};
	void setPrintOn() { infoPrint = 1;}
	void setPrintOff() { infoPrint = 0;}
	void printDateTime();
	SmiProxy(char *name);
	SmiProxy();
	virtual ~SmiProxy(); 
	virtual void smiCommandHandler() {};
	virtual void smiDomainExitHandler(); 
};

#endif

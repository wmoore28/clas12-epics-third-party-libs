#ifndef __SMIUIRTLHHDEFS
#define __SMIUIRTLHHDEFS

#include "smixx_common.hxx"
#include <dic.hxx>
#include <sllist.hxx>
#include "smiuirtl_core.hxx"

//using namespace std;

#ifndef SMI_PAR_TYPES
#ifndef WIN32
typedef enum { STRING, INTEGER, FLOAT } SHORT_PAR_TYPES;
#endif
typedef enum { SMI_STRING, SMI_INTEGER, SMI_FLOAT } PAR_TYPES;
#define SMI_PAR_TYPES
#endif

#ifdef WIN32
#ifdef SMIUILIB
#	define SmiDllExp __declspec(dllexport)
#else
#	define SmiDllExp __declspec(dllimport)
#endif
#else
#	define SmiDllExp
#endif

class SmiDllExp SmiParam : public SLLItem
{
	friend class SmiObject;
	friend class SmiAction;
	char *itsName;
	int itsDefault;
	void *itsValue;
	int itsValueSize;
	int itsType;
	int itisObj;
	void *itsEscValue;
	int itsEscValueSize;
public:
	SmiParam(char *name);
	~SmiParam();
	char *getName() { return itsName; };
	int getType() {return itsType; };
	int getValueInt() { return *(int *)itsValue; };
	double getValueFloat() { return *(double *)itsValue; };
	char *getValueString() { return (char *)itsValue; };
	char *getEscValueString() { return (char *)itsEscValue; }
	void convertToEsc();
	int hasValue() {return itsDefault; };
	int setValue(int val);
	int setValue(double val);
	int setValue(char *val);
	friend ostream& operator<<(ostream&, SmiParam&);
};

class SmiDllExp ParamHandler
{
	friend class SmiObject;
	friend class SmiAction;
	int itsNParams;
	SLList itsParamList;
	void addParam(SmiParam *ptr) { itsParamList.add(ptr);};
	SmiParam *remParam() { return (SmiParam *) itsParamList.removeHead(); };
public:
	virtual ~ParamHandler() {};
	int getNParams() { return itsNParams; };
	virtual SmiParam *getFirstParam();
	virtual SmiParam *getNextParam();
};

class SmiObject;

class SmiDllExp SmiAction : public SLLItem, public ParamHandler
{
	friend class SmiObject;
	char *itsName;
	SmiObject *itsObject;
	void getPars();
public:
	SmiAction(char *name, int npars);
	virtual ~SmiAction();
	char *getName() { return itsName; };
	int setParam(SmiParam *param);
	int setParam(char *name, int val);
	int setParam(char *name, double val);
	int setParam(char *name, char *val);
	int askParams();
	int send();
	friend ostream& operator<<(ostream&, SmiAction& );
};

class SmiDllExp SmiAttribute : public SLLItem
{
	char *itsName;
public:
	SmiAttribute(char *name);
	~SmiAttribute();
	char *getName() { return itsName; };
	friend ostream& operator<<(ostream&, SmiAttribute& );
};

class SmiDomain;

class SmiDllExp SmiObject : public SLLItem, public ParamHandler
{
	char *itsName;
	char *itsState;
	char *itsActionInP;
	char *currActionName;
	int itsId;
	int itsBusy;
	int itsNActions;
	int itsOK;
	SLList itsActionList;
	void addAction(SmiAction *ptr) { itsActionList.add(ptr);};
	SmiAction *remAction() { return (SmiAction *) itsActionList.removeHead(); };
	SLList itsAttributeList;
        int gotAttributes;
	void getAttributes();
	void addAttribute(SmiAttribute *ptr) { itsAttributeList.add(ptr);};
	SmiAttribute *remAttribute() { return (SmiAttribute *) itsAttributeList.removeHead(); };
	int itsWakeup;
	
public:
	SmiDomain *itsDomain;
	void getObjState(int id);
	void getObjPars(int id);
	void getObjActions(int id);
	SmiObject(char *name, SmiDomain *ptr);
	SmiObject(char *name);
	SmiObject(char *domain, char *name);
	virtual ~SmiObject();
	char *getName() { return itsName; };
	virtual void smiStateChangeHandler() {};
	virtual void smiExecutingHandler() {};
	char *getState();
	int getBusy();
	void setId(int id) {itsId = id; }
	int getId() { return itsId; };
	int getNActions() { return itsNActions; };
	char *getActionInProgress();
	SmiAction *getNextAction();
	SmiAction *getAction(char *name);
	SmiParam *getNextParam();
	SmiAttribute *getNextAttribute();
	int sendCommand(SmiAction *action);
	int sendCommand(char *name);
	int sendCommand();
	friend ostream& operator<<(ostream&, SmiObject& );
	void waitUp()
	{
		while(!itsWakeup)
		{
#ifdef __VMS
			sys$hiber();
#else
			dim_wait();
#endif
		}
	}
	void wakeUp()
	{
		itsWakeup = 1;
#ifdef __VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
};

class SmiDllExp SmiObjectPtr : public SLLItem
{
	char *itsName;
	SmiObject *itsObject;
	
public:
	SmiObjectPtr(SmiObject *ptr)
	{
		itsObject = ptr;
	}
	~SmiObjectPtr()
	{
	}
	SmiObject *getObject() { return itsObject; }
};

class SmiDllExp SmiObjectSet : public SLLItem
{
	char *itsName;
	int itsId;
	int itsNObjects;
	int itsOK;
	SLList itsObjectList;
	void addObject(SmiObjectPtr *ptr) { itsObjectList.add(ptr);};
	SmiObjectPtr  *remObject() { return (SmiObjectPtr *) itsObjectList.removeHead(); };
	int itsWakeup;
	
public:
	SmiDomain *itsDomain;
	void getObjects(int id);
	SmiObjectSet(char *name, SmiDomain *ptr);
	SmiObjectSet(char *name);
	SmiObjectSet(char *domain, char *name);
	virtual ~SmiObjectSet();
	char *getName() { return itsName; };
	virtual void smiObjSetChangeHandler() {};
	void setId(int id) {itsId = id; }
	int getId() { return itsId; };
	int getNObjects() { return itsNObjects; };
	SmiObject *getNextObject();
	friend ostream& operator<<(ostream&, SmiObjectSet& );
	void waitUp()
	{
		while(!itsWakeup)
		{
#ifdef __VMS
			sys$hiber();
#else
			dim_wait();
#endif
		}
	}
	void wakeUp()
	{
		itsWakeup = 1;
#ifdef __VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
};

class SmiDllExp SmiDomain
{
	char *itsName;
	int itsWakeup;
	int itsNObjects;
	SmiObject *allocObj;
	SLList itsObjList;
	void addObj(SmiObject *ptr) { itsObjList.add(ptr);};
	SmiObject *remObj() { return (SmiObject *) itsObjList.removeHead(); };
	SLList itsObjSetList;
	void addObjSet(SmiObjectSet *ptr) { itsObjSetList.add(ptr);};
	SmiObjectSet *remObjSet() { return (SmiObjectSet *) itsObjSetList.removeHead(); };
public:
	SmiDomain(char *name);
	virtual ~SmiDomain();
	SmiDomain *itsHandler;
	SmiObject *currObj;
	SmiObjectSet *currObjSet;
	void setNObjects(int nobjs) {itsNObjects = nobjs;};
	void setAllocation(int up);
	char *getName() { return itsName; };
	int getNObjects();
	void getObjects(int up);
	SmiObject *getNextObject();
	SmiObject *getObject() {return currObj; };
	virtual void smiStateChangeHandler() {};
	virtual void smiExecutingHandler() {};
	virtual void smiDomainHandler() {};
	void getObjectSets(int up);
	SmiObjectSet *getNextObjectSet();
	SmiObjectSet *getObjectSet() {return currObjSet; };
	virtual void smiObjSetChangeHandler() {};
	int sendCommand(char *obj, char *cmnd);
	int sendCommand();
	int shutdown();
	char *getAllocationState() { return allocObj->getState(); };
	int allocate();
	int release();
	friend ostream& operator<<(ostream&, SmiDomain& );
	friend ostream& operator<<(ostream&, SmiDomain* );
	void waitUp()
	{
		while(!itsWakeup)
		{
#ifdef __VMS
			sys$hiber();
#else
			dim_wait();
#endif
		}
	}
	void wakeUp()
	{
		itsWakeup = 1;
#ifdef __VMS
		sys$wake(0,0);
#endif
#ifdef WIN32
		wake_up();
#endif
	}
	int isUp() { return itsWakeup;};
};


class SmiDllExp SmiUi
{
public:
	static int sendCommand(char *obj, char *cmnd);
	static int sendProxyCommand(char *obj, char *cmnd);
	static int checkDomain(char *domain);
	static int checkProxy(char *proxy);
//	static void setError(){};
//	static int printError(){};
	static void capitalize(char *name);
};

#endif

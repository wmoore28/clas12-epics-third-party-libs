#ifndef commhandler_HH
#define commhandler_HH

#ifndef solaris
#ifdef sun
#define solaris
#endif
#endif
#include "name.hxx"
#include "namevector.hxx"
#include "twonames.hxx"
#include "queue_twonames.hxx"
#include "ptrvector.hxx"
class CommHandler;
#include "smiobject.hxx"
#include "smiobjectset.hxx"
#include "state.hxx"
#include <dic.hxx>
#include <dis.hxx>

#define MAX_NAME 132

typedef struct{
	int busy;
	char action[1];
}BUSY; 

class CommObj : public DimServer
{
protected:
	Name topName;
	Name itsName;
	Name currState;
	Name currDimState;
	Name currActions;
	BUSY   *currBusy;
	DimService *dimState;
	DimService *dimActions;
	DimService *dimBusy;
	DimCommand *dimCmd;
	void serviceHandler();
public :
	CommObj(Name &name, Name &topname) 
	{	
		itsName = name; 
		topName = topname; 
		currState = "$DEAD";
		currDimState = "$DEAD";
		currActions = "";
		dimState = 0; dimActions = 0; dimBusy = 0;
//		currBusy = new BUSY;
		currBusy = (BUSY *) new char[1+sizeof(int)];		
		currBusy->busy = 0;
		currBusy->action[0] = '\0';
		created = 0;
//		create();
	};
    int created;
	void create();
	Name &getName() {return itsName;};
	Name &getState() {return currState;};
	Name &findState(State *pstate);
	void setBusy(char *cmnd);
	void clearBusy();
	void updateState() 
		{if(dimState) /*create(); else*/ dimState->updateService();};
	void updateActions() 
		{if(dimActions) /*create(); else*/ dimActions->updateService();};
	void updateBusy() 
		{if(dimBusy) /*create(); else*/ dimBusy->updateService();};
	void commandHandler();
	void releaseDomain();
};

class Obj : public CommObj, public DimClient
{
	SMIObject *pObj;
	int itsNSubObjs;
	PtrVector itsSubObjs;
	DimInfo *itsProxy;
	DimInfo *itsProxyBusy;
	void getDimName(char *servname);
	void infoHandler();
public :
	Obj(Name &name, SMIObject *pobj, int nsubobjs);
	void create();
	int findState();
	int getNSubObjs() {return itsNSubObjs;};
	CommObj *getSubObj(int index) {return (CommObj *)itsSubObjs[index];};
	SMIObject *getpObj() {return pObj;};
	void updateState();
	void setBusy(char *cmnd);
	void clearBusy();
	void sendCommand(char *cmnd);
};

class ObjSet
{
private:
	Name itsName;
	SMIObjectSet *pObjSet;
	int itsNObjs;
	Name itsSetObjs;
	DimService *dimSetObjs;
public :
	ObjSet(Name &name, SMIObjectSet *pobjset);
	Name &getName() {return itsName;};
	int getNObjs() {return itsNObjs;};
	SMIObjectSet *getpObjSet() {return pObjSet;};
	void updateObjSet(int first_time);
};

class CommHandler : public DimServer
{
public :
	CommHandler(char* domainname, Queue_TwoNames* ipStateQ, 
		Queue_TwoNames* ipExternalActionQ);
	CommHandler(char* domainname, Queue_TwoNames* ipStateQ, 
		Queue_TwoNames* ipExternalActionQ, int propagateBusy);
	void startIt();
	void publishState(SMIObject *pObj);
	void publishBusy(SMIObject *pObj, char *command);
	void invalidAction(SMIObject *pObj);
	void sendCommand( const char* object_nm, char* command) const;
	void declareObj(SMIObject *pObj);
	void declareObjList();
	void declareObjSet(SMIObjectSet *pObjSet);
	void publishObjSet(SMIObjectSet *pObjSet);
	void declareObjSetList();
	void domainAllocated(int conn_id);
	void domainDeallocated(int conn_id);
	void clientExitHandler();
	void addObjToDynObjList(SMIObject *pObj);

	static char *domainName;
	static Queue_TwoNames* pStateQ;
	static Queue_TwoNames* pExternalActionQ;
	static PtrVector itsObjs;
	static PtrVector itsObjSets;
	static void printDateTime();
	static void setDnsNode( char *node);
	static int propagateBusyFlag;
private :
/* class vars */
//	char *itsObjList;
	Name itsDynObjList;
	Name itsDynObjSetList;
	int itsVersion;
	DimService /**objList,*/ *dynObjList, *dynObjSetList, *version;
	int nObjs;
	int nObjSets;
	Obj *findObject(SMIObject *pobj);
	Obj *findObjectByName(const char *objname) const;
	ObjSet *findObjectSet(SMIObjectSet *pobjset);
	void declareDynObjList();
	int started;
};

#endif


//-------------------------- CommHandler Class ----------------------------------
#define SMI
#include "smixx_common.hxx"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef WIN32
#ifdef __VMS /* VMS doesn't take includes relative to the source directory! */
#include "../../smixx_version.h"
#else
#include "version.h"
#endif
#else
#include "version.h"
#include <windows.h>
#endif
#include "commhandler.hxx"
/*
#define COMMS_DEBUG 1
*/
char *CommHandler::domainName;
Queue_TwoNames* CommHandler::pStateQ;
Queue_TwoNames* CommHandler::pExternalActionQ;
PtrVector CommHandler::itsObjs;
PtrVector CommHandler::itsObjSets;
int CommHandler::propagateBusyFlag;

void CommHandler::printDateTime()
{
    time_t t;
	char str[128];

    t = time((time_t *)0);
/*
#ifdef WIN32
	strcpy(s, ctime(&t));
#else
	ctime_r(&t, s);
#endif
*/
	my_ctime(&t, str, 128);
	str[strlen(str)-1] = '\0';
    cout << "SMICOMMS - " << str << " - ";
}

void CommObj::serviceHandler() 
{
	DimService *curr = getService();

	if(curr == dimState)
	{
		curr->setData(currState.getString());
	}
	else if(curr == dimActions)
	{
		curr->setData(currActions.getString());
	}
	else if(curr == dimBusy)
	{
		curr->setData(currBusy,strlen(currBusy->action)+1+sizeof(int));
	}
}

void CommObj::create()
{
	char servname[MAX_NAME];
	char tmpstr[MAX_NAME];

	if(created)
		return;
	created = 1;
	strcpy(tmpstr,"SMI/");
	strcat(tmpstr,CommHandler::domainName);
	strcat(tmpstr,"/");
	if(!(topName == itsName))
	{
		strcat(tmpstr,topName.getString());
		strcat(tmpstr,".");
	}	        
	strcat(tmpstr,itsName.getString());
	strcpy(servname,tmpstr);

	dimState = new DimService(servname,"C",this);

	strcat(tmpstr,"/ACTIONS&PARS");
	dimActions = new DimService(tmpstr,"C",this);

	strcpy(tmpstr,servname);
	strcat(tmpstr,"/BUSY");
	dimBusy = new DimService(tmpstr,"L:1;C",this);

	strcpy(tmpstr,servname);
	strcat(tmpstr,"/CMD");
	dimCmd = new DimCommand(tmpstr,"C",this);

}

Name &CommObj::findState(State *pState)
{
char *ptr;
Name tmp;

	currState = pState->stateName();
	int n_actions = pState->numActions();
	currActions = "";
	int first = 1;
	for(int i = 0; i< n_actions; i++)
	{
		tmp = pState->actionString(i);
		ptr = tmp.getString();
		if(!strstr(ptr,"&END_IN_STATE_WHEN"))
		{
			if(first)
				first = 0;
			else
				currActions += "|";
			currActions += ptr;
		}
	}
	return currState;
}

void CommObj::setBusy(char *cmnd)
{
	dim_lock();
	if(	currBusy->busy == 1)
	{
//		if(!strcmp(currBusy->action,cmnd))
		dim_unlock();
		return;
	}
	delete[] currBusy;
	currBusy = (BUSY *)new char[strlen(cmnd)+1+sizeof(int)];
	currBusy->busy = 1;
	strcpy(currBusy->action,cmnd);
	updateBusy();
	dim_unlock();
}

void CommObj::clearBusy()
{
	dim_lock();
	if(currBusy->busy)
	{
		delete[] currBusy;
		currBusy = (BUSY *)new char[1+sizeof(int)];
		currBusy->busy = 0;
		currBusy->action[0] = '\0';
	}
	updateBusy();
	dim_unlock();
}

void CommObj::releaseDomain()
{
	int conn_id;
	static char *cmndptr = 0;

#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << itsName << " executing : "<< "RELEASE" << endl;
#endif
	if(!cmndptr)
		cmndptr = new char[MAX_NAME+8];
	if( (conn_id = DimServer::getClientId()) )
	{
		sprintf(cmndptr,"RELEASE/&ID(S)=%s/&CONN_ID(I)=%d",
			DimServer::getClientName(),conn_id); 
	}
	TwoNames  actionInfo;
	if(!(topName == itsName))
		actionInfo.set(topName.getString(),cmndptr);
	else
		actionInfo.set(itsName.getString(),cmndptr);
	(CommHandler::pExternalActionQ)->add(actionInfo);
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

void CommObj::commandHandler()
{
	int conn_id;
	static char *cmndptr = 0;
	static int cmndsize = 0;
	
	DimCommand *curr = getCommand();
//	setBusy(curr->getString());
#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << itsName << " executing : "<< curr->getString() << endl;
#endif
	int size = curr->getSize();
	if(cmndsize < ( size + MAX_NAME))
	{
		cmndsize = size + MAX_NAME;
		if(cmndptr)
			delete[] cmndptr;
		cmndptr = new char[cmndsize];
	}
	strcpy(cmndptr,curr->getString());
//	if(conn_id = dis_get_client(name))
	if( (conn_id = DimServer::getClientId()) )
	{
		sprintf(&cmndptr[size-1],"/&ID(S)=%s/&CONN_ID(I)=%d",
			DimServer::getClientName(),conn_id); 
	}
	TwoNames  actionInfo;
	if(!(topName == itsName))
		actionInfo.set(topName.getString(),cmndptr);
	else
		actionInfo.set(itsName.getString(),cmndptr);
	(CommHandler::pExternalActionQ)->add(actionInfo);
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

void my_usleep(int usecs)
{

#ifndef WIN32
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = usecs;
	select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
#else
//	usleep(usecs);
	Sleep(usecs/1000);
#endif
}

Obj::Obj(Name &name, SMIObject *pobj, int nsubobjs) : CommObj(name, name)
{
	char tmpname[MAX_NAME]; 
	CommObj *subptr;
	int i;

	pObj = pobj;
	itsNSubObjs = nsubobjs;
	if(itsNSubObjs)
	{
		getDimName(tmpname);
		dim_lock();
		itsProxy = new DimInfo(tmpname,"$DEAD",this);
		dim_unlock();
		getDimName(tmpname);
		if(CommHandler::propagateBusyFlag)
		{
			strcat(tmpname,"/BUSY");
			dim_lock();
			itsProxyBusy = new DimInfo(tmpname,-1,this);
			dim_unlock();
		}
//		my_usleep(1000);
//		dim_usleep(1000);
	}
	if(itsNSubObjs > 1)
	{
		for(i = 0; i < itsNSubObjs; i++)
		{
			Name subname = pObj->subobjectName(i);
			subptr = new CommObj(subname, name);
			itsSubObjs += subptr;
		}
	}
}

int Obj::findState()
{
//	int i;
	State *pState;
	
//	if(itsNSubObjs > 1)
//	{
//		currState = "";
//		for(i = 0; i < itsNSubObjs; i++)
//		{
//			pState = pObj->pCurrentState(i);
//			currState += ((CommObj *)itsSubObjs[i])->getName();
//			currState += ".";
//			currState += ((CommObj *)itsSubObjs[i])->findState(pState);
//			if(i != itsNSubObjs - 1)
//				currState += ",";
//		}
//	}
//	else
//	{
//		pState = pObj->pCurrentState(0);
		pState = pObj->pCurrentState();
		CommObj::findState(pState);
//	}
	currState += pObj->parmString();
	return 1;
}

void Obj::updateState()
{
	int i;
	
	if(itsNSubObjs > 1)
	{
		for(i = 0; i < itsNSubObjs; i++)
		{
			((CommObj *)itsSubObjs[i])->updateState();
			((CommObj *)itsSubObjs[i])->updateActions();
			((CommObj *)itsSubObjs[i])->clearBusy();
		}
	}
	CommObj::updateState();
	CommObj::updateActions();
	CommObj::clearBusy();
}

void Obj::clearBusy()
{
	int i;
	
	if(itsNSubObjs > 1)
	{
		for(i = 0; i < itsNSubObjs; i++)
		{
			((CommObj *)itsSubObjs[i])->clearBusy();
		}
	}
	CommObj::clearBusy();
}

void Obj::setBusy(char *cmnd)
{
	int i;
	
	if(itsNSubObjs > 1)
	{
		for(i = 0; i < itsNSubObjs; i++)
		{
			((CommObj *)itsSubObjs[i])->setBusy(cmnd);
		}
	}
	CommObj::setBusy(cmnd);
}

void Obj::create()
{
	int i;
	
	if(itsNSubObjs > 1)
	{
		for(i = 0; i < itsNSubObjs; i++)
		{
			((CommObj *)itsSubObjs[i])->create();
		}
	}
	CommObj::create();
}

void Obj::sendCommand(char *cmnd)
{
	char servname[MAX_NAME];
	
	setBusy(cmnd);
#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << itsName << " executing : "<< cmnd << endl;
#endif
	getDimName(servname);
	strcat(servname,"/CMD");
	DimClient::sendCommandNB(servname,cmnd);
}

void Obj::getDimName(char *servname)
{
	char tmpstr[MAX_NAME];
	int index;

	if ((index = itsName.existsAt("::")) != -1)
	{
		strcpy(servname,"SMI/");
		strcpy(tmpstr,itsName.getString());
		tmpstr[index] = '\0';
		strcat(servname,tmpstr);
		strcat(servname,"/");
		strcpy(tmpstr,&itsName[index+2]);
		strcat(servname,tmpstr);
	}
	else
	{
		strcpy(servname,"EP/");
		strcat(servname,CommHandler::domainName);
		strcat(servname,"/");
		strcat(servname,itsName.getString());
	}
}

void Obj::infoHandler()
{
	TwoNames stateInfo;
	DimInfo *proxy, *proxyBusy;
	DimInfo *curr;
	char *data;
	BUSY *busyData;

//	proxy = getInfo();
	curr = getInfo();
//cout << "Received "<<curr->getName() << endl;
	if(curr == itsProxy)
	{
		proxy = curr;
		data = (char *)proxy->getData();
//cout << "Passing state up "<<this->itsName.getString()<<" "<<data<< endl;
		stateInfo.set(this->itsName.getString(), data);
		(CommHandler::pStateQ)->add(stateInfo);
/*
		if((this->currState == "$DEAD") || 
			(!strncmp(this->currState.getString(),"DEAD",4)))
*/
		if((this->currDimState == "$DEAD") && 
			(strcmp(data,"$DEAD")))
		{
			char servname[MAX_NAME];
			strcpy(servname,CommHandler::domainName);
			strcat(servname,"::");
			strcat(servname,this->itsName.getString());
			DimClient::setExitHandler(servname);
		} 
		this->currDimState = data;
	}
	else if(curr == itsProxyBusy)
	{
		proxyBusy = curr;
		busyData = (BUSY *)proxyBusy->getData();
		if(busyData->busy == 1)
		{
			stateInfo.set(this->itsName.getString(), "&BUSY");
			(CommHandler::pStateQ)->add(stateInfo);
		}
	}
#ifdef __VMS
	sys$wake(0,0);
#endif
#ifdef WIN32
	wake_up();
#endif

}

ObjSet::ObjSet(Name &name, SMIObjectSet *pobjset)
{
	itsName = name; 
	pObjSet = pobjset;
	dimSetObjs = 0;
	updateObjSet(1);
}

void ObjSet::updateObjSet(int first_time)
{
	Name tmpname;
	char tmpstr[MAX_NAME], *ptr;
	int i;

	itsNObjs = pObjSet->numOfObjects();
	pObjSet->reset();
	itsSetObjs = "";
	if(itsNObjs)
	{
		for(i = 0; i < itsNObjs; i++)
		{
			pObjSet->nextObject(tmpname);
			itsSetObjs += tmpname;
			itsSetObjs += "|";
		}
	}
	ptr = itsSetObjs.getString();
	if(itsNObjs)
		ptr[strlen(ptr)-1] = '\0';
	if(first_time)
	{
		strcpy(tmpstr,"SMI/");
		strcat(tmpstr,CommHandler::domainName);
		strcat(tmpstr,"/SET/");
		strcat(tmpstr,itsName.getString());
		dimSetObjs = new DimService(tmpstr,"C",ptr, strlen(ptr)+1);
	}
	else
		dimSetObjs->updateService(ptr, strlen(ptr)+1);
}

CommHandler::CommHandler(char* domainname, Queue_TwoNames* ipStateQ,
	Queue_TwoNames* ipExternalActionQ) 
{ 
				 
	dim_init();
	domainName = domainname;
	pStateQ = ipStateQ;
	pExternalActionQ = ipExternalActionQ;
	propagateBusyFlag = 0;
	nObjs = 0;
	started = 0;
}

CommHandler::CommHandler(char* domainname, Queue_TwoNames* ipStateQ,
	Queue_TwoNames* ipExternalActionQ, int propagateBusy) 
{ 
			
	dim_init();
	domainName = domainname;
	pStateQ = ipStateQ;
	pExternalActionQ = ipExternalActionQ;
	propagateBusyFlag = propagateBusy;
	nObjs = 0;
	started = 0;
}
void CommHandler::declareObj(SMIObject *pObj)
{
	Obj *objptr;
	int nsubobjs;

	Name tmpname = pObj->name();
	if(findObjectByName(tmpname.getString()))
	{
		cout << "CommHandler: Object Already Declared - " << tmpname << endl;
		return;
	}
	nsubobjs = pObj->numSubobj();
	objptr = new Obj(tmpname, pObj, nsubobjs);
	if(nsubobjs > 1)
		nObjs += nsubobjs;
	else
		nObjs++;
	itsObjs += objptr;
#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << objptr->getName() << " initial state : "<< objptr->getState() << endl;
#endif
}

void CommHandler::declareObjSet(SMIObjectSet *pObjSet)
{
	ObjSet *setptr;

	Name setname = pObjSet->name();

	if(findObjectSet(pObjSet))
	{
		cout << "CommHandler: Object Set Already Declared - " << setname << endl;
		return;
	}
	setptr = new ObjSet(setname, pObjSet);
	nObjSets++;
	itsObjSets += setptr;
#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << setptr->getName() << " declared"<< endl;
#endif
}

void CommHandler::publishObjSet(SMIObjectSet *pObjSet)
{
	ObjSet *setptr;

	if( (setptr = findObjectSet(pObjSet)) )
	{
		setptr->updateObjSet(0);
	}
#ifdef COMMS_DEBUG
	CommHandler::printDateTime();
	cout << setptr->getName() << " updated"<< endl;
#endif
}

void CommHandler::declareObjSetList()
{
	int nobjsets;
	ObjSet *objsetptr;
	char tmpstr[MAX_NAME], *ptr;

	itsDynObjSetList = "";

	nobjsets = itsObjSets.length();
	for(int i = 0; i < nobjsets; i++)
	{
		objsetptr = (ObjSet *)itsObjSets[i];
		itsDynObjSetList += objsetptr->getName();
		itsDynObjSetList += "|";
	}
	ptr = itsDynObjSetList.getString();
	if(strlen(ptr))
		ptr[strlen(ptr)-1] = '\0';
	strcpy(tmpstr,"SMI/");
	strcat(tmpstr,CommHandler::domainName);
	strcat(tmpstr,"/OBJECTSET_LIST");
	dynObjSetList = new DimService(tmpstr,"C",ptr, strlen(ptr)+1);

//	dis_add_client_exit_handler((void (*)(...))client_exit_handler);
}


Obj *CommHandler::findObject(SMIObject *pobj)
{
	int i;
	Obj *objptr;
	
	for(i = 0; i < itsObjs.length(); i++)
	{
		objptr = (Obj *)itsObjs[i];
		if(	objptr->getpObj() == pobj)
		return (Obj *)itsObjs[i];
	}
	return (Obj *)0;
}

ObjSet *CommHandler::findObjectSet(SMIObjectSet *pobjset)
{
	int i;
	ObjSet *objsetptr;
	
	for(i = 0; i < itsObjSets.length(); i++)
	{
		objsetptr = (ObjSet *)itsObjSets[i];
		if(	objsetptr->getpObjSet() == pobjset)
		return (ObjSet *)itsObjSets[i];
	}
	return (ObjSet *)0;
}

Obj *CommHandler::findObjectByName(const char *objname) const
{
	int i;
	Obj *objptr;
	
	for(i = 0; i < itsObjs.length(); i++)
	{
		objptr = (Obj *)itsObjs[i];
		if(	objptr->getName() == objname)
		return (Obj *)itsObjs[i];
	}
	return (Obj *)0;
}

void CommHandler::clientExitHandler()
{
	Obj *objptr;

		CommHandler::printDateTime();
		cout << DimServer::getClientName() << " died, releasing Domain" << endl;
		objptr = (Obj *)CommHandler::itsObjs[0];
		objptr->releaseDomain();
}

void CommHandler::declareObjList()
{
	char tmpstr[MAX_NAME];

    itsVersion = SMIXX_VERSION;
	strcpy(tmpstr,"SMI/");
	strcat(tmpstr,CommHandler::domainName);
	strcat(tmpstr,"/SMI_VERSION_NUMBER");
	version = new DimService(tmpstr,itsVersion);

	declareDynObjList();
	declareObjSetList();

	return;
}

void CommHandler::declareDynObjList()
{
	int nobjs, nsub;
	Obj *objptr;
	char tmpstr[MAX_NAME], *ptr;;

	itsDynObjList = "";

	nobjs = itsObjs.length();
	for(int i = 0; i < nobjs; i++)
	{
		objptr = (Obj *)itsObjs[i];
		if((nsub = objptr->getNSubObjs()) > 1)
		{
			for(int j=0; j<nsub; j++)
			{
				itsDynObjList += objptr->getName();
				itsDynObjList += ".";
				itsDynObjList += objptr->getSubObj(j)->getName();
				itsDynObjList += "/ASSOCIATED";
				itsDynObjList += "|";
			}
		}
		else
		{
			itsDynObjList += objptr->getName();
			if(nsub == 1)
				itsDynObjList += "/ASSOCIATED";
			itsDynObjList += "|";
		}
	}
	
	itsDynObjList.removeLastChar();
	ptr = itsDynObjList.getString();
	
//	cout << " itsDynObjList : " << itsDynObjList << endl;
//	cout << "  length : " << itsDynObjList.length() << endl;
//	cout << "           ptr : <" << ptr << ">" << endl;
//	cout << "  strlen(ptr) : " << strlen(ptr) << endl;

	strcpy(tmpstr,"SMI/");
	strcat(tmpstr,CommHandler::domainName);
	strcat(tmpstr,"/OBJECT_LIST");
	dynObjList = new DimService(tmpstr,"C",ptr, itsDynObjList.length());
	
}

void CommHandler::addObjToDynObjList(SMIObject *pObj)
{
	itsDynObjList += "|";
	// I am assuming here that whoever calls this method made sure that
	// object of this name is not already in the list
	itsDynObjList += pObj->name();
	if( pObj->associated() ) { itsDynObjList += "/ASSOCIATED";}

	char* ptr = itsDynObjList.getString();
	
	dynObjList->updateService(ptr, itsDynObjList.length());
		
	return;
}

void CommHandler::publishState(SMIObject *pObj)
{
//cout << " ---start CommHandler::publishState  object:" << pObj->name() << endl;
//cout << "       started : " << started << endl;
	Obj *objptr;

	if( (objptr = findObject(pObj)) )
	{
		objptr->findState();
		objptr->updateState();
		if(!objptr->created)
		{
			objptr->create();
		}
#ifdef COMMS_DEBUG
		CommHandler::printDateTime();
		cout << objptr->getName() << " in state : "<< objptr->getState() << endl;
#endif
	}
	else
	{
		cout << "Publish State, object not created yet" << endl;
	}
	if(!started)
		startIt();
//cout << " ---end CommHandler::publishState " << endl;
}

void CommHandler::publishBusy(SMIObject *pObj, char *cmnd)
{
	Obj *objptr;

	if( (objptr = findObject(pObj)) )
	{
		if(!strstr(cmnd,"&END_IN_STATE_WHEN"))
		{
			objptr->setBusy(cmnd);
#ifdef COMMS_DEBUG
			CommHandler::printDateTime();
			cout << objptr->getName() << " executing : "<< cmnd << endl;
#endif
		}
	}
}

void CommHandler::startIt()
{
	int i;
	Obj *objptr;
	int missing = 0;
	char tmpstr[MAX_NAME];
	
	for(i = 0; i < itsObjs.length(); i++)
	{
		objptr = (Obj *)itsObjs[i];
		if(!objptr->created)
		{
//cout << "Missing "<< objptr->getName() << endl;
			missing = 1;
			break;
		}
	}
//cout << "Start It "<<itsObjs.length()<<" missing "<<missing << endl;
	if(!missing)
	{
		strcpy(tmpstr,CommHandler::domainName);
		strcat(tmpstr,"_SMI");
		DimServer::start(tmpstr);
		DimServer::addClientExitHandler(this);
		started = 1;
	}
}

void CommHandler::sendCommand( const char *extobj, char *cmnd) const
{
	Obj *objptr;

	if( (objptr = findObjectByName(extobj)) )
	{
		objptr->sendCommand(cmnd);
	}
}	

void CommHandler::invalidAction(SMIObject *pObj)
{
	Obj *objptr;

	if( (objptr = findObject(pObj)) )
	{
		objptr->updateState();
//		objptr->updateActions();
//		objptr->clearBusy();
#ifdef COMMS_DEBUG
		CommHandler::printDateTime();
		cout << objptr->getName() << " Invalid action" << endl;
#endif
	}
}


void CommHandler::domainAllocated(int conn_id)
{
	DimServer::setClientExitHandler(conn_id);
//	dis_set_client_exit_handler(conn_id,2);
}

void CommHandler::domainDeallocated(int conn_id)
{
	DimServer::clearClientExitHandler(conn_id);
//	dis_set_client_exit_handler(conn_id,0);
}

void CommHandler::setDnsNode( char *dns_node)
{
	char *ptr;
	int dns_port = 0;

	if((ptr = strchr(dns_node,':')))
	{
		*ptr = '\0';
		ptr++;
		sscanf(ptr,"%d",&dns_port);
	}
	if(dns_port)
	{
		DimServer::setDnsNode(dns_node, dns_port);
		DimClient::setDnsNode(dns_node, dns_port);
	}
	else
	{
		DimServer::setDnsNode(dns_node);
		DimClient::setDnsNode(dns_node);
	}
}


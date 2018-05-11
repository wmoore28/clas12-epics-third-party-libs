#include "smixx_common.hxx"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define SMIUILIB
#include "smiuirtl.hxx"

extern "C" {
#include "smixx_parstring_util.h"
}

static void obj_change_rout(int *objId, SmiObject **tag)
{
	SmiObject *t = *tag;
	int id;
	
//	cout << t->getName() << " SmiObject change rout" << endl;
	id = t->getId();
	if(id == -1)
	{
//		cout << "SmiObject out of change rout (dead)" << endl;
		return;
	}
	if(!id)
		t->setId(*objId);
	t->getObjState(*objId);
	t->getObjPars(*objId);
	t->getObjActions(*objId);
	t->wakeUp();
	t->smiExecutingHandler();
	if(!t->getBusy())
		t->smiStateChangeHandler();
	if(t->itsDomain)
	{
		if(t->itsDomain->isUp())
		{
			t->itsDomain->currObj = t;
			t->itsDomain->smiExecutingHandler();
			if(!t->getBusy())
				t->itsDomain->smiStateChangeHandler();
		}
	}
//	cout << "SmiObject out of change rout\n";
//	cout.flush();
} 

static void objset_change_rout(int *objId, SmiObjectSet **tag)
{
	SmiObjectSet *t = *tag;
	int id;
	
//	cout << t->getName() << " SmiObject change rout" << endl;
	id = t->getId();
	if(id == -1)
	{
//		cout << "SmiObject out of change rout (dead)" << endl;
		return;
	}
	if(!id)
		t->setId(*objId);
	t->getObjects(*objId);
	t->wakeUp();
	t->smiObjSetChangeHandler();
	if(t->itsDomain)
	{
		if(t->itsDomain->isUp())
		{
			t->itsDomain->currObjSet = t;
			t->itsDomain->smiObjSetChangeHandler();
		}
	}
//	cout << "SmiObject out of change rout\n";
//	cout.flush();
} 

static void domain_rout(SmiDomain **tag, int *nobjs)
{
	SmiDomain *t = *tag;
	
	t->setNObjects(*nobjs);
	t->wakeUp();
	if(!*nobjs)
	{
		t->setAllocation(0);
		t->getObjects(0);
		t->getObjectSets(0);
	}
	else
	{
		t->setAllocation(1);
		t->getObjects(1);
		t->getObjectSets(1);
	}
	t->itsHandler->smiDomainHandler();
} 

SmiParam::SmiParam(char *name) 
{
	itsName = new char[strlen(name)+1];
	strcpy(itsName, name);
	itsDefault = 0;
	itisObj = 0;
	itsValueSize = 0;
	itsEscValueSize = 0;
}

SmiParam::~SmiParam() 
{
	delete itsName;
	if(itsValueSize)
		delete (char *)itsValue;
	if(itsEscValueSize)
		delete (char *)itsEscValue;
}

int SmiParam::setValue(int val) 
{ 
	if (itisObj) 
		return -1;
	int intSize = sizeof(int);
	if(itsValueSize < intSize)
	{
		if(itsValueSize)
			delete (char *)itsValue;
		itsValue = new char[sizeof(int)];
		itsValueSize = sizeof(int);
	} 
	itsType = SMI_INTEGER;
	*(int *)itsValue = val;
	return(1);
}

int SmiParam::setValue(double val) 
{ 
	if (itisObj) 
		return -1;
	int doubleSize = sizeof(double);
	if(itsValueSize < doubleSize)
	{
		if(itsValueSize)
			delete (char *)itsValue;
		itsValue = new char[sizeof(double)];
		itsValueSize = sizeof(double);
	} 
	itsType = SMI_FLOAT;
	*(double *)itsValue = val;
	return(1);
}

int SmiParam::setValue(char *val) 
{ 
	if (itisObj) 
		return -1;
	int valLen = strlen(val);
	if(itsValueSize < (valLen+1))
	{
		if(itsValueSize)
			delete (char *)itsValue;
		itsValue = new char[strlen(val)+1];
		itsValueSize = strlen(val)+1;
	} 
	itsType = SMI_STRING;
	strcpy((char *)itsValue, val);
	/* now the escaped value */
	if ( itsEscValueSize ) delete (char *) itsEscValue;
	int nout = smixx_escape(NULL,val,0);
	if (nout < 0) return -1;
	itsEscValue = new char[nout+1];
	nout = smixx_escape((char* )itsEscValue,val,nout);
	itsEscValueSize = nout + 1;
	
	return(1);
}

void SmiParam::convertToEsc()
{
	if ( itsType != SMI_STRING ) return;
	
	if ( itsEscValueSize ) delete (char *) itsEscValue;
	int nout = smixx_escape(NULL,(char *)itsValue,0);
	if (nout < 0) return ;
	itsEscValue = new char[nout+1];
	nout = smixx_escape((char* )itsEscValue,(char *)itsValue,nout);
	itsEscValueSize = nout + 1;
	
	return;
}

SmiParam *ParamHandler::getFirstParam() 
{
	return (SmiParam *)itsParamList.getHead(); 
}

SmiParam *ParamHandler::getNextParam() 
{
	return (SmiParam *)itsParamList.getNext(); 
}

SmiAction::SmiAction(char *name, int npars) 
{
	itsName = new char[strlen(name)+1];
	strcpy(itsName, name);
	itsNParams = npars;
	itsObject = 0;
}

SmiAction::~SmiAction() 
{
SmiParam *paramptr;

	delete itsName;
	while ( (paramptr = remParam()) )
	{
		delete paramptr;
	}
	itsNParams = 0;
}

void SmiAction::getPars()
{
char tmp[MAX_NAME];
int type, size;
SmiParam *paramptr;

	itsNParams = 0;
	while(smiui_get_next_param(itsObject->getId(), tmp, &type, &size))
	{
		itsNParams++;
		paramptr = new SmiParam(tmp);
		paramptr->itsType = type;
		if(type == SMI_STRING)
			size +=1;
		paramptr->itsValue = new char[size];
		paramptr->itsValueSize = size;
		int ret = smiui_get_param_default_value(itsObject->getId(), 
			paramptr->itsValue); 
		if(ret)
		{
			paramptr->itsDefault = 1;
			paramptr->convertToEsc();
		}
		addParam(paramptr);
	}
}

int SmiAction::setParam(SmiParam *param)
{
	addParam(param);
	return 1;
}

int SmiAction::setParam(char *name, int val)
{
	SmiParam *paramptr;

	while ( (paramptr = getNextParam()) )
	{
		if(!strcmp(paramptr->getName(),name))
		{
			if(paramptr->getType() == SMI_INTEGER)
			{
				paramptr->setValue(val);
            	return 1;
			}
			else
				return 0;
		}
	}
	return(-1);
}

int SmiAction::setParam(char *name, double val)
{
	SmiParam *paramptr;

	while ( (paramptr = getNextParam()) )
	{
		if(!strcmp(paramptr->getName(),name))
		{
			if(paramptr->getType() == SMI_FLOAT)
			{
				paramptr->setValue(val);
            	return 1;
			}
			else
				return 0;
		}
	}
	return(-1);
}

int SmiAction::setParam(char *name, char *val)
{
	SmiParam *paramptr;

	while ( (paramptr = getNextParam()) )
	{
		if(!strcmp(paramptr->getName(),name))
		{
			if(paramptr->getType() == SMI_STRING)
			{
				paramptr->setValue(val);
            	return 1;
			}
			else
				return 0;
		}
	}
	return(-1);
}

int SmiAction::send()
{
	char cmnd[MAX_NAME*2];
	char tmp[MAX_NAME];
	int ret = 1;

	strcpy(cmnd,itsName); 
	if(getNParams())
	{
		SmiParam *param;
		param = getFirstParam();
		while (param)
		{
			if(!param->itsValueSize)
			{
//				cout << "ERROR: Action " << *this << " requires parameters\n";
//			    cout.flush();
				ret = 0;
			}
			else
			{
				switch(param->getType())
				{
					case SMI_INTEGER:
						sprintf(tmp,"/%s(I)=%d",param->itsName, *(int *)param->itsValue);
						break;
					case SMI_FLOAT:
						sprintf(tmp,"/%s(F)=%e",param->itsName, *(double *)param->itsValue);
						break;
					case SMI_STRING:
						sprintf(tmp,"/%s=\"%s\"",param->itsName, (char *)param->itsEscValue);
						break;
				}
			}
			strcat(cmnd, tmp);
			param = getNextParam();
		}
	}
	smiui_send_command(itsObject->getName(),cmnd);
	return ret;
}

int SmiAction::askParams()
{
/*	char cmnd[MAX_NAME*2]; */
	char tmp[MAX_NAME];
	int ival = 0;
	double fval = 0;

	if(getNParams())
	{
		SmiParam *param;
		while ( (param = getNextParam()) )
		{
			switch(param->getType())
			{
				case SMI_INTEGER:
					if(!param->itsValueSize)
						sprintf(tmp,"/%s (I) > ",param->itsName);
					else
					{
						int* pintVal = static_cast<int*>(param->itsValue);
						sprintf(tmp,"/%s (I) (default %d) > ",param->itsName, 
							*pintVal);
					}
					cout << tmp;
					cout.flush();
					cin >> ival;
					if(ival)
						param->setValue(ival);
					break;
				case SMI_FLOAT:
					if(!param->itsValueSize)
						sprintf(tmp,"/%s (F) > ",param->itsName);
					else
					{
						double* pdoubleVal = static_cast<double*>(param->itsValue);
						sprintf(tmp,"/%s (F) (default %e) > ",param->itsName, 
							*pdoubleVal);
					}
					cout << tmp;
					cout.flush();
					cin >> fval;
					if(fval)
						param->setValue(fval);
					break;
				case SMI_STRING:
					if(!param->itsValueSize)
						sprintf(tmp,"/%s (S) > ",param->itsName);
					else
					{
						char* pcharVal = static_cast<char*>(param->itsValue);
						sprintf(tmp,"/%s (S) (default \"%s\") > ",
							param->itsName, pcharVal);
					}
					cout << tmp;
					cout.flush();
					cin >> tmp;
					if(tmp[0])
						param->setValue(tmp);
					break;
			}
		}
	}
	return 1;
}

SmiAttribute::SmiAttribute(char *name) 
{
	itsName = new char[strlen(name)+1];
	strcpy(itsName, name);
}

SmiAttribute::~SmiAttribute() 
{

	delete itsName;
}

void SmiObject::getAttributes()
{
	char tmp[MAX_NAME];
	SmiAttribute *attributeptr;

	while ( ( attributeptr = remAttribute()) )
		delete attributeptr;
	while(smiui_get_next_attribute(itsName, tmp))
	{
		attributeptr = new SmiAttribute(tmp);
		addAttribute(attributeptr);
	}
}

SmiObject::SmiObject(char *name, SmiDomain *domptr) 
{ 
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsState = 0;
	itsDomain = domptr;
	itsActionInP = 0;
	currActionName = 0;
	gotAttributes = 0;
	if(strstr(name,"::"))
	{
//		getAttributes(); 
		itsId = smiui_book_statechange(itsName, 
			(void (*) (...))obj_change_rout, (dim_long)this);
	}
	else
	{
		itsId = smiui_book_alloc_statechange(itsName, 
			(void (*) (...))obj_change_rout, (dim_long)this);
	}
}
	
SmiObject::SmiObject(char *name) 
{ 
//	cout << "Constructing SmiObject: " << name << endl;
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsState = 0;
	itsDomain = 0;
	itsActionInP = 0;
	currActionName = 0;
//	getAttributes();
	gotAttributes = 0;
	itsId = smiui_book_statechange(itsName, (void (*) (...))obj_change_rout, (dim_long)this);
//	cout << "SmiObject: out of " << name << "\n";
//	cout.flush();
}
	
SmiObject::SmiObject(char *domain, char *name) 
{ 
	itsName = new char[strlen(domain)+strlen(name)+3];
	strcpy(itsName,domain);
	strcat(itsName,"::");
	strcat(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsState = 0;
	itsDomain = 0;
	itsActionInP = 0;
	currActionName = 0;
//	getAttributes(); 
	gotAttributes = 0;
	itsId = smiui_book_statechange(itsName, (void (*) (...))obj_change_rout, (dim_long)this);
}
	
SmiObject::~SmiObject() 
{
	if(strstr(itsName,"::"))
	{
		smiui_cancel_statechange(itsId);
	}
	else
	{
		smiui_cancel_alloc_statechange(itsId);
	}
	itsId = -1;
	delete itsName;
	if(itsState)
		delete itsState;
	if(itsActionInP)
		delete itsActionInP;
	SmiParam *paramptr;
	while ( (paramptr = remParam()) )
		delete paramptr;
	SmiAction *actionptr;
	while ( (actionptr = remAction()) )
		delete actionptr;
}

void SmiObject::getObjState(int id)
{
	if(!itsState)
		itsState = new char[MAX_NAME];
	itsOK = smiui_get_state(id, &itsBusy, itsState, &itsNActions);
}

void SmiObject::getObjPars(int id)
{
char tmp[MAX_NAME];
int type, size;
SmiParam *paramptr;

	while ( (paramptr = remParam()) )
		delete paramptr;
	itsNParams = 0;
	while(smiui_get_next_obj_param(id, tmp, &type, &size))
	{
		itsNParams++;
		paramptr = new SmiParam(tmp);
		paramptr->itisObj = 1;
		paramptr->itsType = type;
		if(type == SMI_STRING)
			size +=1;
		paramptr->itsValue = new char[size];
		paramptr->itsValueSize = size;
		int ret = smiui_get_obj_param_value(id, paramptr->itsValue); 
		if(ret)
		{
			paramptr->itsDefault = 1;
			paramptr->convertToEsc();
		}	
		addParam(paramptr);
	}
}

void SmiObject::getObjActions(int id) 
{
char tmp[MAX_NAME];
SmiAction *actionptr;
int npars;

	while ( (actionptr = remAction()) )
		delete actionptr;
	while(smiui_get_next_action(id, tmp, &npars))
	{
		SmiAction *actionptr;
		actionptr = new SmiAction(tmp, npars);
		actionptr->itsObject = this;
		addAction(actionptr);
		actionptr->getPars();
	}
}

char *SmiObject::getState() 
{
	waitUp();
	if(itsOK)
		return itsState;
	else
		return (char *) 0;
}

int SmiObject::getBusy() 
{
	waitUp();
	if(itsOK)
		return itsBusy;
	else
		return 0;
}

char *SmiObject::getActionInProgress() 
{
	int size;

	waitUp();
//	if(!itsActionInP)
//		itsActionInP = new char[MAX_NAME];
	if(itsActionInP)
	{
		delete itsActionInP;
		itsActionInP = 0;
	}
	if(itsOK)
	{
		smiui_get_action_in_progress_size(itsId, &size);
		itsActionInP = new char[size+1];		
		smiui_get_action_in_progress(itsId, itsActionInP); 
		return itsActionInP;
	}
	else
		return (char *)-1;
}

SmiAction *SmiObject::getNextAction() 
{
	waitUp();
	if(itsOK)
	{
		return (SmiAction *)itsActionList.getNext(); 
	}
	else
		return (SmiAction *)-1;
}

SmiParam *SmiObject::getNextParam() 
{
	waitUp();
	if(itsOK)
	{
		return ParamHandler::getNextParam(); 
	}
	else
		return (SmiParam *)-1;
}

SmiAttribute *SmiObject::getNextAttribute() 
{
	if(!gotAttributes)
	{
		getAttributes();
		gotAttributes = 1;
	}
	return (SmiAttribute *)itsAttributeList.getNext(); 
}

SmiAction *SmiObject::getAction(char *name)
{
int found = 0;
SmiAction *ret = 0;
char *namep; /* *ptr; */

	namep = new char[strlen(name)+1];
	strcpy(namep, name);
	SmiUi::capitalize(namep);
	waitUp();
	if(itsOK)
	{
		if(getNActions())
		{
			SmiAction *action;
			while ( (action = getNextAction()) )
			{
				if(!strcmp(action->getName(),namep))
				{
					ret = action;
					found = 1;
		   		}
			}
		}
	}
//	if(!found)
//	{
//		cout << "ERROR: Action " << name << " does not exist in this state\n";
//  	cout.flush();
//	}
	delete namep;
	return ret;
}

int SmiObject::sendCommand(SmiAction *actionptr)
{
int ret = 0, found = 0;

	waitUp();
	if(itsOK)
	{
		if(getNActions())
		{
			SmiAction *action;
			while ( (action = getNextAction()) )
			{
				if(!strcmp(action->getName(),actionptr->getName()))
				{
					actionptr->itsObject = this;
					ret = actionptr->send();
					found = 1;
		   		}
			}
		}
	}
//	if(!found)
//	{
//		cout << "ERROR: Action " << actionptr.getName() << " does not exist in this state\n";
//  	cout.flush();
//	}
	return ret;
}

int SmiObject::sendCommand(char *name)
{
int found = 0;
int ret = 0;
char *namep, *ptr;

	namep = new char[strlen(name)+1];
	strcpy(namep, name);
	SmiUi::capitalize(namep);
	if((ptr = strchr(namep,'/')))
		*ptr = 0;
	waitUp();
	if(itsOK)
	{
		if(getNActions())
		{
			SmiAction *action;
			while ( (action = getNextAction()) )
			{
				if(!strcmp(action->getName(),namep))
				{
					*ptr = '/';
					ret = smiui_send_command(getName(),namep);
//					ret = action->send();
					found = 1;
		   		}
			}
		}
	}
//	if(!found)
//	{
//		cout << "ERROR: Action " << name << " does not exist in this state\n";
//  	cout.flush();
//	}
	delete namep;
	return ret;
}

int SmiObject::sendCommand()
{
int found = 0;
int ret = 0;
char name[MAX_NAME];

	cout << "Command > ";
	cout.flush();
	cin >> name;
	SmiUi::capitalize(name);

	waitUp();
	if(itsOK)
	{
		if(getNActions())
		{
			SmiAction *action;
			while ( (action = getNextAction()) )
			{
				if(!strcmp(action->getName(),name))
				{
					action->askParams();
					ret = action->send();
					found = 1;
		   		}
			}
		}
	}
	if(!found)
	{
		cout << "ERROR: Action " << name << " does not exist\n";
    	cout.flush();
	}
	return ret;
}

SmiObjectSet::SmiObjectSet(char *name, SmiDomain *domptr) 
{ 
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsDomain = domptr;
	itsId = smiui_book_objectsetchange(itsName, (void (*) (...))objset_change_rout, (dim_long)this);
}
	
SmiObjectSet::SmiObjectSet(char *name) 
{ 
//	cout << "Constructing SmiObjectSet: " << name << endl;
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsDomain = 0;
	itsId = smiui_book_objectsetchange(itsName, (void (*) (...))objset_change_rout, (dim_long)this);
}
	
SmiObjectSet::SmiObjectSet(char *domain, char *name) 
{ 
	itsName = new char[strlen(domain)+strlen(name)+3];
	strcpy(itsName,domain);
	strcat(itsName,"::");
	strcat(itsName,name);
	itsId = 0;
	itsWakeup = 0;
	itsDomain = 0;
	itsId = smiui_book_objectsetchange(itsName, (void (*) (...))objset_change_rout, (dim_long)this);
}
	
SmiObjectSet::~SmiObjectSet() 
{
	smiui_cancel_objectsetchange(itsId);
	itsId = -1;
	delete itsName;
	SmiObjectPtr *objectptr;
	/*while ( (objectptr = remObject()) );  */
	while ( (objectptr = remObject()) )
		delete objectptr;
}

void SmiObjectSet::getObjects(int id) 
{
char tmp[MAX_NAME];
SmiObjectPtr *objptrp;
SmiObject *objptr;

	while ( (objptrp = remObject()) )
	{
		delete objptrp;
	}
	while(smiui_get_next_object_in_set(id, tmp))
	{
		while ( (objptr = itsDomain->getNextObject()) )
		{
			if(!strcmp(objptr->getName(), tmp))
			{
				objptrp = new SmiObjectPtr(objptr); 
				addObject(objptrp);
			}
		}
	}
}

SmiObject *SmiObjectSet::getNextObject() 
{
	waitUp();
	if(itsOK)
	{
		SmiObjectPtr *objptrp;
		objptrp = (SmiObjectPtr *)itsObjectList.getNext();
		if(objptrp)
			return objptrp->getObject();
		else
			return (SmiObject *)0; 
	}
	else
		return (SmiObject *)-1;
}

SmiDomain::SmiDomain(char *name) 
{ 

	itsWakeup = 0;
	itsNObjects = 0;
	itsName = new char[strlen(name)+1];
	itsHandler = this;
	strcpy(itsName,name);
	SmiUi::capitalize(itsName);
	currObj = 0;
	allocObj = 0;
	smiui_book_connect_domain(itsName, (void(*)(...))domain_rout, (dim_long)this);
}

SmiDomain::~SmiDomain() 
{ 
	SmiObject *objptr;
	SmiObjectSet *objsetptr;
	while ( (objptr = remObj()) )
		delete objptr;
	while ( (objsetptr = remObjSet()) )
		delete objsetptr;
	delete allocObj;
	smiui_cancel_connect_domain(itsName);
	delete itsName;
}

void SmiDomain::setAllocation(int up)
{

	if(allocObj)
	{
		delete allocObj;
		allocObj = 0;
	}
	if(up)
		allocObj = new SmiObject(itsName, itsHandler);
}

int SmiDomain::shutdown()
{
	waitUp();
	return smiui_shutdown_domain(itsName);
}

int SmiDomain::allocate()
{
	waitUp();
	return smiui_allocate(itsName);
}

int SmiDomain::release()
{
	waitUp();
	return smiui_release(itsName);
}

int SmiDomain::getNObjects()
{

	waitUp();
	return itsNObjects;
}

SmiObject *SmiDomain::getNextObject()
{

	SmiObject *objptr;
/*	char tmp[MAX_NAME]; */

	waitUp();
	objptr = (SmiObject *) itsObjList.getNext();
	return objptr;
/*
	if(smiui_get_next_object(itsName, tmp))
	{
		if(objptr = (SmiObject *) itsObjList.getHead())
		{
	        do
			{
				if(!strcmp(objptr->getName(),tmp))
				return objptr; 
			}while(objptr = (SmiObject *) itsObjList.getNext());
		}
		objptr = new SmiObject(tmp, itsHandler);
		addObj(objptr);
		return objptr; 
	}
	else
		return (SmiObject *)0;
*/
}

SmiObjectSet *SmiDomain::getNextObjectSet()
{

/*	SmiObjectSet *objsetptr;
	char tmp[MAX_NAME];  */

	waitUp();
	return (SmiObjectSet *) itsObjSetList.getNext();
/*
	if(smiui_get_next_objectset(itsName, tmp))
	{
		if(objsetptr = (SmiObjectSet *) itsObjSetList.getHead())
		{
	        do
			{
				if(!strcmp(objsetptr->getName(),tmp))
				return objsetptr; 
			}while(objsetptr = (SmiObjectSet *) itsObjSetList.getNext());
		}
		objsetptr = new SmiObjectSet(tmp, itsHandler);
		addObjSet(objsetptr);
		return objsetptr; 
	}
	else
		return (SmiObjectSet *)0;
*/
}

void SmiDomain::getObjects(int up)
{

	SmiObject *objptr;
	char tmp[MAX_NAME];

	while ( (objptr = remObj()) )
		delete objptr;
	if(up)
	{
		while(smiui_get_next_object(itsName, tmp))
		{
			objptr = new SmiObject(tmp, itsHandler);
			addObj(objptr); 
		}
	}
}

void SmiDomain::getObjectSets(int up)
{

	SmiObjectSet *objsetptr;
	char tmp[MAX_NAME];

	while ( (objsetptr = remObjSet()) )
		delete objsetptr;
	if(up)
	{
		while(smiui_get_next_objectset(itsName, tmp))
		{
			objsetptr = new SmiObjectSet(tmp, itsHandler);
			addObjSet(objsetptr);
		}
	}
}

int SmiDomain::sendCommand(char *name, char *cmnd)
{
int found = 0;
int ret = 0;
char tmp[MAX_NAME];

	waitUp();
	if(!strstr(name,"::"))
	{
		strcpy(tmp,itsName);
		strcat(tmp,"::");
		strcat(tmp,name);
	}
	else
		strcpy(tmp,name);
	if(getNObjects())
	{
		SmiObject *obj;
		while ( (obj = getNextObject()) )
		{
			if(!strcmp(obj->getName(),tmp))
			{
				ret = obj->sendCommand(cmnd);
				found = 1;
			}
		}
	}
	if(!found)
	{
		cout << "ERROR: Object " << tmp << " does not exist\n";
    	cout.flush();
	}
	return ret;
}

int SmiDomain::sendCommand()
{
int found = 0;
int ret = 0;
char tmp[MAX_NAME];
char name[MAX_NAME];

	waitUp();
	cout << "To Object > ";
	cout.flush();
	cin >> name;
	SmiUi::capitalize(name);
	if(!strstr(name,"::"))
	{
		strcpy(tmp,itsName);
		strcat(tmp,"::");
		strcat(tmp,name);
	}
	else
		strcpy(tmp,name);
	if(getNObjects())
	{
		SmiObject *obj;
		while ( (obj = getNextObject()) )
		{
			if(!strcmp(obj->getName(),tmp))
			{
				ret = obj->sendCommand();
				found = 1;
			}
		}
	}
	if(!found)
	{
		cout << "ERROR: Object " << tmp << " does not exist\n";
    	cout.flush();
	}
	return ret;
}

ostream& operator<<(ostream& os, SmiParam& par)
{
	os << par.getName();
	switch(par.getType())
	{
		case SMI_INTEGER:
			os <<"(I)";
			break;
		case SMI_FLOAT:
			os <<"(F)";
			break;
		case SMI_STRING:
			os <<"(S)";
			break;
	}
	if(par.hasValue())
	{
		switch(par.getType())
		{
			case SMI_INTEGER:
				os <<"=" << par.getValueInt();
 				break;
			case SMI_FLOAT:
				os <<"=" << par.getValueFloat();
				break;
			case SMI_STRING:
				os <<"=" << par.getEscValueString();
				break;
		}
	}
	os.flush();
	return os;
}

ostream& operator<<(ostream& os, SmiAction& action)
{
	os << action.getName();
	if(action.getNParams())
	{
		SmiParam *param;
		while ( (param = action.getNextParam()) )
			os << "/" <<*param;
	}
	os.flush();
	return os;
}

ostream& operator<<(ostream& os, SmiAttribute& attribute)
{
	os << attribute.getName();
	os.flush();
	return os;
}

ostream& operator<<(ostream& os, SmiObject& obj)
{
char *name, *state;
int alloc = 0;
SmiAttribute *attr;

	name = obj.getName();
	if(!strstr(name,"::"))
	{
		os << "Domain ";
		alloc = 1;
	}
	os << name;
	while ( (attr = obj.getNextAttribute()) )
		os << "/" << *attr;
	if(obj.getBusy())
	{
		os << " executing " << obj.getActionInProgress() << "\n";
		os.flush();
		return os;
	}
	else
	{
		state = obj.getState();
		if(!state)
		{
			os << " in state DEAD" << "\n";
			os.flush();
			return os;
		}
		else
			os << " in state " << state;
	}
	os.flush();
	if(obj.getNParams())
	{
		SmiParam *param;
		while ( (param = obj.getNextParam()) )
			os << "/" << *param;
	}
	os << "\n";  
	os.flush();
	if(alloc)
		return os;
	if(obj.getNActions())
	{
		os << "    Available Actions:\n";
		SmiAction *action;
		while ( (action = obj.getNextAction()) )
			os << "        " << *action << "\n";
	}
	os.flush();
	return os;
}

ostream& operator<<(ostream& os, SmiDomain& domain)
{
	int nobjs; 

	domain.waitUp();
	os << "Domain " << domain.getName();
	if ( (nobjs = domain.getNObjects()) )
	{
		os << " has " << nobjs << " objects\n";
		os << *(domain.allocObj) << "\n";
		SmiObject *obj;
		while ( (obj = domain.getNextObject()) )
			os << *obj << "\n";
	}
	else
	{
		os << " does not exist\n";
	}
	os.flush();
	return os;
}

ostream& operator<<(ostream& os, SmiDomain *domain)
{
	int nobjs; 

	domain->waitUp();
	os << "Domain " << domain->getName();
	if ( (nobjs = domain->getNObjects()) )
	{
		os << " is Up (" << nobjs << " objects)\n";
	}
	else
	{
		os << " is Down\n";
	}
	os.flush();
	return os;
}

void SmiUi::capitalize(char *name)
{
	while(*name)
	{
		*name = toupper(*name);
		name++;
	}
}

int SmiUi::sendCommand(char *name, char *cmnd)
{
	smiui_send_command(name,cmnd);
	return 1;
}

int SmiUi::sendProxyCommand(char *name, char *cmnd)
{
	smiui_ep_send_command(name,cmnd);
	return 1;
}

int SmiUi::checkDomain(char *domain)
{
	return smiui_connect_domain(domain);
}

int SmiUi::checkProxy(char *proxy)
{
	return smiui_check_proxy(proxy);
}


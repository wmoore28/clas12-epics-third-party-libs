#include <smiuirtl.hxx>

class Object: public SmiObject
{
	void smiStateChangeHandler();
	void smiExecutingHandler();
public:
	Object(char *name): SmiObject(name) {};
};

class Domain: public SmiDomain
{
	void smiStateChangeHandler();
	void smiObjSetChangeHandler();
	void smiExecutingHandler();
	void smiDomainHandler();
public:
	Domain(char *name): SmiDomain(name) {};
};

void Domain::smiStateChangeHandler()
{
	SmiObject *objp = getObject();
	char *name, *state;
		
	name = objp->getName();
	state = objp->getState();
	if(name)
	{
		if(state)
			cout << name << " in state " << objp->getState() << endl;
		else
			cout << name << " is dead" << endl;
	}
}

void Domain::smiObjSetChangeHandler()
{
	SmiObjectSet *objsetp = getObjectSet();
	SmiObject *objp;
	char *name;
		
	name = objsetp->getName();
	cout << "OBJECT_SET: " << name << " contains: "<< endl;
	while(objp = objsetp->getNextObject())
	{
		cout << "\t" << objp->getName() << endl;
	}
}

void Domain::smiExecutingHandler()
{
	SmiObject *objp = getObject();
	char *name;

	name = objp->getName();
	if(name)
	{
		if(objp->getBusy())
		{
			cout << name << " executing " <<  objp->getActionInProgress() << endl;
		}
	}
}

void Domain::smiDomainHandler()
{
	int nobjs;
	cout << "Domain " << getName();
	if(nobjs = getNObjects())
	{
		cout << " is Up (" << nobjs << " objects)" << endl;
//		while(getNextObject());
//		while(getNextObject());
//		while(getNextObjectSet());
	}
	else
	{
		cout << " is Down" << endl;
	}
}

void Object::smiStateChangeHandler()
{
	char *name, *state;
	SmiParam *param;

	name = getName();
	state = getState();
	if(name)
	{
		if(state)
			cout << name << " in state " << getState() << endl;
		else
			cout << name << " is dead" << endl;
	}
	while(param = getNextParam())
	{
		if(param == (SmiParam *)-1)
			break;
		cout << param->getName() << endl;
	}
}

void Object::smiExecutingHandler()
{
	char *name;

	name = getName();
	if(name)
	{
		if(getBusy())
		{
			cout << name << " executing " <<  getActionInProgress() << endl;
		}
	}
}

int main(int argc, char *argv[])
{


//	Domain *domainp;

//	domainp = new Domain(argv[1]);

	Object *objp;
	char cmd[32];
//	SmiAction action("START", 1);
//	SmiParam par("TYPE");
	SmiAction *action;

	objp = new Object(argv[1]);
			
//	cout << " in state " << objp->getState() << endl;
	sleep(10);
//	strcpy(cmd,"START/TYPE=PHYSICS");
//	cout << "Sending command " << cmd << endl;
//	objp->sendCommand(cmd);
/*
	par.setValue("PHYS|ICS/123");
	action.setParam(&par);
	objp->sendCommand(&action);
*/
	if(action = objp->getAction("START"))
	{
		action->setParam("TYPE", "PHYS|ICS/123");
		objp->sendCommand(action);
	}

	while(1)
		pause();
	return 0;
}

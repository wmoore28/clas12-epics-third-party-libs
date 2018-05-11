#include "smixx_common.hxx"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define SMILIB
#include "smirtl.hxx"
#include "version.h"

extern "C" {
#include "smixx_parstring_util.h"
}

static int Version_number = SMIXX_VERSION;

ParInfo::ParInfo(char *name, int type)
{
	itsName = new char[strlen(name)+1];
	strcpy(itsName, name);
	itsType = type;
	switch(type)
	{
		case SMI_INTEGER:
			strcpy(itsTypeStr,"(I)");
			itsData = new char[32];
			itsEscData =0;
			break;
		case SMI_FLOAT:
			strcpy(itsTypeStr,"(F)");
			itsData = new char[32];
			itsEscData =0;
			break;
		case SMI_STRING:
			itsData = new char[32];
			itsEscData = new char[32];
			strcpy(itsTypeStr,"(S)");
			break;
	}
}
	
ParInfo::~ParInfo()
{
	delete itsName;
	delete itsData;
	if ( !itsEscData ) delete itsEscData;
}

int ParInfo::setInfo(void *data, int type)
{
	if(itsType != type)
		return -1;
	switch(type)
	{
		case SMI_INTEGER:
			sprintf(itsData,"%d",*(int *)data);
			break;
		case SMI_FLOAT:
			sprintf(itsData,"%e",*(double *)data);
			break;
		case SMI_STRING:
			delete itsData;
			/* the user is supplying un-escaped data*/
			itsData = new char[ strlen((char *)data) +1];
			strcpy(itsData,(char *)data);
			
			/* now we have to create the escaped string*/
			delete itsEscData;
			int nout = smixx_escape(NULL,itsData,0);
			if ( nout < 0 ) return -1;
			itsEscData = new char[ nout +1];
			smixx_escape(itsEscData, itsData,nout);
			break;
	}
	return 1;
}

int ParInfo::setStrInfo(char *data, int type)
{
	if(itsType != type)
		return -1;
		
	if ( type == SMI_STRING )
	{
		/* escaped string comming from command handler*/
		delete itsEscData;
		itsEscData = new char[ strlen(data) +1];
		strcpy(itsEscData,(char *)data);
		
		/* have to unescape it*/
		delete itsData;
		int nout = smixx_unescape(NULL,itsEscData,0);
		if (nout < 0) return -1;
		itsData = new char[nout + 1];
		nout = smixx_unescape(itsData,itsEscData,nout);
		return 1;
	}
	else
	{
		delete itsData;
		itsData = new char[ strlen(data) +1];
		strcpy(itsData,(char *)data);
		return 1;
	}
}

SmiProxy::SmiProxy(char *name)
{
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	itsState = 0;
	itsAction = 0;
	itsShortState = 0;
	itsCommand = 0;
	itsParValue = 0;
	cmndTokens = 0;
	infoPrint = 1;
}

SmiProxy::SmiProxy()
{
	itsName = 0;
	itsState = 0;
	itsAction = 0;
	itsShortState = 0;
	itsCommand = 0;
	itsParValue = 0;
	cmndTokens = 0;
	infoPrint = 1;
}

SmiProxy::~SmiProxy() 
{
	ParInfo *parptr;
	delete itsName;
	delete itsFullName;
	if(itsState)
		delete itsState;
	if(itsShortState)
		delete itsShortState;
	if(itsAction)
		delete itsAction;
	if(itsCommand)
		delete itsCommand;
	if(itsParValue)
		delete itsParValue;
	if(cmndTokens)
		delete cmndTokens;
	while((parptr = (ParInfo *)itsParList.removeHead()) != 0)
		delete parptr;
	while((parptr = (ParInfo *)itsCmndParList.removeHead()) != 0)
		delete parptr;
	delete itsDimState;
	delete itsDimCmnd;
	delete itsDimVersion;
}
	
void SmiProxy::printDateTime()
{
	time_t t;
	char str[128];

	t = time((time_t *)0);
/*
#ifdef WIN32
	strcpy(str, ctime(&t));
#else
	ctime_r(&t, str);
#endif
*/
	my_ctime(&t, str, 128);
	str[strlen(str)-1] = '\0';
	cout << "SMIRTL - " << str << " -";
}
	
void SmiProxy::smiDomainExitHandler() 
{
	if(infoPrint)
	{
		printDateTime();
		cout << " Smi Domain " << itsDomain << 
		" Exited - Proxy " << itsName << " Exiting..." << endl;
	}
	exit(0);
}

void SmiProxy::commandHandler()
{
	DimCommand *cmnd;

	cmnd = DimCommandHandler::getCommand();
	if(itsCommand)
		delete itsCommand;
	itsCommand = new char[cmnd->getSize()];
	strcpy(itsCommand, cmnd->getString());
	if(cmndTokens)
		delete cmndTokens;
	cmndTokens = new TokenString(itsCommand);
	storeCmndPar();
	if(infoPrint)
	{
		printDateTime();
		cout << " " << itsFullName << " executing " << itsCommand << endl;
	}
	smiCommandHandler();
}

void SmiProxy::publish_it(int later)
{
	char *aux;

	capitalize(itsDomain);
	capitalize(itsName);

	itsFullName = new char[strlen(itsDomain)+strlen(itsName)+3];
	strcpy(itsFullName,itsDomain);
	strcat(itsFullName,"::");
	strcat(itsFullName,itsName);
	aux = new char[strlen(itsDomain)+strlen(itsName)+5+32];
	strcpy(aux,"EP/");
	strcat(aux,itsDomain);
	strcat(aux,"/");
	strcat(aux,itsName);

	if(!itsState)
		itsState=new char[8];
	if(!itsShortState)
		itsShortState=new char[128];
	strcpy(itsState,"$DEAD");
	strcpy(itsShortState,"$DEAD");
	itsDimState = new DimService(aux,itsState);
	strcat(aux,"/CMD");
	itsDimCmnd = new DimCommand(aux,"C",this);
	strcpy(aux,"EP/");
	strcat(aux,itsDomain);
	strcat(aux,"/");
	strcat(aux,itsName);
	strcat(aux,"/SMI_VERSION_NUMBER");
	itsDimVersion = new DimService(aux,Version_number);
	if(!later)
		DimServer::start(itsFullName);
	else
		DimServer::start();
}

void SmiProxy::attach(char *domain)
{
	itsDomain = new char[strlen(domain)+1];
	strcpy(itsDomain,domain);
	publish_it(0);
//	smi_attach(itsFullName, (void (*) (...))handle_smi_command);
}

void SmiProxy::attach(char *domain, char *name)
{
	itsDomain = new char[strlen(domain)+1];
	strcpy(itsDomain,domain);
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	publish_it(0);
//	smi_attach(itsFullName, (void (*) (...))handle_smi_command);
}

void SmiProxy::attach(char *domain, char *name, int )
{
	itsDomain = new char[strlen(domain)+1];
	strcpy(itsDomain,domain);
	itsName = new char[strlen(name)+1];
	strcpy(itsName,name);
	publish_it(1);
//	smi_attach(itsFullName, (void (*) (...))handle_smi_command);
}

void SmiProxy::setVolatile()
{
//	smi_volatile();
	DimServer::addClientExitHandler(this);
}

int SmiProxy::setState(char *state)
{
	
//	return smi_set_state(state);
	DISABLE_AST
	formState(state);
	if(infoPrint)
	{
		printDateTime();
		cout << " " << itsFullName << " in state " << itsState << endl;
	}
	itsDimState->updateService(itsState);
	ENABLE_AST
	return(1);
}
	
char *SmiProxy::getState()
{
//	if(!itsState)
//		itsState=new char[256];
//	smi_get_state(itsState,256);
	return itsShortState;
}

int SmiProxy::setParameter(char *parName, int parValue)
{
//	return smi_set_par(parName, &parValue, SMI_INTEGER);
	return addPar(parName, &parValue, SMI_INTEGER);

}
	
int SmiProxy::setParameter(char *parName, double parValue)
{
//	return smi_set_par(parName, &parValue, SMI_FLOAT);
	return addPar(parName, &parValue, SMI_FLOAT);
}
	
int SmiProxy::setParameter(char *parName, char *parValue)
{
//	return smi_set_par(parName, parValue, SMI_STRING);
	return addPar(parName, parValue, SMI_STRING);
}

int SmiProxy::getParameterInt(char *parName)
{
	int value;
	char *data;

	data = getCmndPar(parName, SMI_INTEGER);
	if(data)
	{
		if(strchr(data,'x'))
			sscanf(data,"%x",&value);
		else
			sscanf(data,"%d",&value);
		return value;
	}
//	if(smi_get_par_value(parName, &value))
//		return value;
//	else
		return -1;
}

double SmiProxy::getParameterFloat(char *parName)
{
	double value;
	char *data;

	data = getCmndPar(parName, SMI_FLOAT);
	if(data)
	{
		sscanf(data,"%lf",&value);
		return value;
	}
//	if(smi_get_par_value(parName, &value))
//		return value;
//	else
		return -1;
}
	
char *SmiProxy::getParameterString(char *parName)
{
//	if(!itsParValue)
//		itsParValue=new char[256];
//	itsParValue[0] = '\0';
//	smi_get_par_value(parName, itsParValue);
//	return itsParValue;
	char *data;

	data = getCmndPar(parName, SMI_STRING);
	if(data)
	{
		if(data[0] == '"')
		{
			if(data[strlen(data)-1] == '"')
				data[strlen(data)-1] = '\0';
			return data+1;
		}
		return data;
	}
	return 0;
}

int SmiProxy::testAction(char *action)
{
//	return smi_test_action(action);

	return (!strcmp(itsAction,action));
}

char *SmiProxy::getAction()
{
//	int npars;

//	if(!itsAction)
//		itsAction=new char[256];
//	itsAction[0] = '\0';
//	smi_get_action(itsAction,&npars);
	return itsAction;
}

void SmiProxy::capitalize(char *str)
{
	while(*str)
	{
		*str = toupper(*str);
		str++;
	}
}
/*
int SmiProxy::findPar(char *name)
{
	ParInfo *parptr;
	char *parname;

	parptr = (ParInfo *)itsParList.getHead();
	while(parptr)
	{
		parname = parptr->getName();
		if(!strcmp(parname, name))
		{
			return parptr;
			break;
		}
		parptr = (ParInfo *)itsParList.getNext();
	}
	return 0;
}
*/
int SmiProxy::addPar(char *name, void *value, int type)
{
	ParInfo *parptr;
	int found = 0;
	char *parname, *auxName;
	int ret = 1;

	auxName = new char[strlen(name)+1];
	strcpy(auxName,name);
	capitalize(auxName);
	parptr = (ParInfo *)itsParList.getHead();
	while(parptr)
	{
		parname = parptr->getName();
		if(!strcmp(parname, auxName))
		{
			found = 1;
			break;
		}
		parptr = (ParInfo *)itsParList.getNext();
	}
	if(!found)
	{
		parptr = new ParInfo(auxName, type);
		itsParList.add(parptr);
	}
	delete auxName;
	ret = parptr->setInfo(value, type);
	return(ret);
}

int SmiProxy::getStateLen(char *state)
{
	ParInfo *parptr;
	char *parname;
	int type;
	int len = 0;

	len = strlen(state);
	parptr = (ParInfo *)itsParList.getHead();
	while(parptr)
	{
		parname = parptr->getName();
		len += 1 + strlen(parname) +3+1;  /* /par_name(X)= */
		type = parptr->getType();
		if ( type == SMI_STRING )
		{
			len += parptr->getEscDataSize();
		}
		else
		{
			len += parptr->getDataSize();
		}
		parptr = (ParInfo *)itsParList.getNext();
	}
	len += 1;
	return len;	
}

void SmiProxy::formState(char *state)
{
	int len;
	ParInfo *parptr;
//	char *parname;
	int type;

	len = getStateLen(state);
	delete itsState;
	itsState = new char[len];

	strcpy(itsShortState, state);	
	strcpy(itsState,state);
	parptr = (ParInfo *)itsParList.getHead();
	while(parptr)
	{
		strcat(itsState,"/");
		strcat(itsState, parptr->getName());
		strcat(itsState, parptr->getTypeStr());
		strcat(itsState,"=");
		type = parptr->getType();
		if ( type == SMI_STRING )
		{
			strcat(itsState, parptr->getEscData());
		}
		else
		{
			strcat(itsState, parptr->getData());
		}
		parptr = (ParInfo *)itsParList.getNext();
	}
}

int SmiProxy::getType(char *str)
{
    int type;

    switch(*str)
    {
        case 'S':
            type = SMI_STRING;
            break;
        case 'I':
            type = SMI_INTEGER;
            break;
        case 'F':
            type = SMI_FLOAT;
            break;
    }
    return(type);
}

int SmiProxy::storeCmndPar()
{
	char *token, *parname, *token_data;
	ParInfo *parptr;
	int type, size = 0;
	int ret = 1;

	while ( (parptr = (ParInfo *)itsCmndParList.removeHead()) )
		delete parptr;
	cmndTokens->getToken(token);
	if(itsAction)
		delete itsAction;
	itsAction = new char [strlen(token)+1];
	strcpy(itsAction,token);
	while(cmndTokens->getToken(token))
	{
		if(cmndTokens->cmpToken("/"))
		{
			cmndTokens->getToken(parname);
			capitalize(parname);
			cmndTokens->pushToken();
			cmndTokens->getToken(token);
			if(cmndTokens->cmpToken("("))
			{
				cmndTokens->getToken(token);
				type = getType(token);
				cmndTokens->getToken(token);
			}
			else
			{
				cmndTokens->popToken();
				type = SMI_STRING;
            }
			parptr = new ParInfo(parname, type);
			itsCmndParList.add(parptr);

			cmndTokens->getToken(token);
			if(cmndTokens->cmpToken("="))
			{
				cmndTokens->pushToken();
				while(cmndTokens->getToken(token))
				{
					if(cmndTokens->cmpToken("/"))
					{
						break;
					}
					size += strlen(token);
				}
				cmndTokens->popToken();
				token_data = new char[size+1];
				token_data[0] = '\0';
				
				while(1)
				{
					cmndTokens->pushToken();
					if(!cmndTokens->getToken(token))
					{
						cmndTokens->popToken();
						break;
					}
					if(cmndTokens->cmpToken("/"))
					{
						cmndTokens->popToken();
						break;
					}
					else
						strcat(token_data, token);
				}
				ret = parptr->setStrInfo(token_data, type);
			}
		}
	}

	currCmndPar = (ParInfo *)0;
	return(ret);
}

char *SmiProxy::getCmndPar(char *name, int type)
{
	ParInfo *parptr;
	char *parname, *auxName, *ret;
//	int found = 0;

	ret = (char *)0;
	auxName = new char[strlen(name)+1];
	strcpy(auxName,name);
	capitalize(auxName);
	if(currCmndPar)
	{
		parname = currCmndPar->getName();
		if(!strcmp(parname, auxName))
		{
			if(currCmndPar->getType() == type)
//				return currCmndPar->getData();
				ret = currCmndPar->getData();
		}
//		return 0;
//		ret = (char *)0;
	}
	else
	{
		parptr = (ParInfo *)itsCmndParList.getHead();
		while(parptr)
		{
			parname = parptr->getName();
			if(!strcmp(parname, auxName))
			{
				if(parptr->getType() == type)
				{
//					return parptr->getData();
					ret = parptr->getData();
					break;
				}
			}
			parptr = (ParInfo *)itsCmndParList.getNext();
		}
	}
	delete auxName;
	return ret;
}

int SmiProxy::getParameterType(char *name)
{
	ParInfo *parptr;
	char *parname, *auxName;
	int ret;

	ret = -1;
	auxName = new char[strlen(name)+1];
	strcpy(auxName,name);
	capitalize(auxName);
	if(currCmndPar)
	{
		parname = currCmndPar->getName();
		if(!strcmp(parname, auxName))
		{
//			return currCmndPar->getType();
			ret = currCmndPar->getType();
		}
//		return -1;
	}
	else
	{
		parptr = (ParInfo *)itsCmndParList.getHead();
		while(parptr)
		{
			parname = parptr->getName();
			if(!strcmp(parname, auxName))
			{
//				return parptr->getType();
				ret = parptr->getType();
			}
			parptr = (ParInfo *)itsCmndParList.getNext();
		}
	}
	delete auxName;
	return ret;
}

char *SmiProxy::getNextParameter()
{

	if(!currCmndPar)
		currCmndPar = (ParInfo *)itsCmndParList.getHead();
	else
		currCmndPar = (ParInfo *)itsCmndParList.getNext();
	if(currCmndPar)
	{
		return currCmndPar->getName();
	}
	return 0;
}


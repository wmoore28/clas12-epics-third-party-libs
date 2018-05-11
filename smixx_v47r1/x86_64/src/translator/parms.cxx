
//----------------------  Class  Parms  -----------------------------------
// This class is going eventually to replace all the other parameter clases
// this was a dream (BF March 2014)
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 1998
//----------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <string.h>
#include <assert.h>
#include "name.hxx"
#include "smlline.hxx"
#include "smllinevector.hxx"
#include "parms.hxx"
#include "utilities.hxx"
#include "errorwarning.hxx"

  const Name noval = "&noval";
  const char undef[] = "&undef";
  const Name undefNm = "&undef";
  const Name typeint = "INT", typefloat = "FLOAT", typestring = "STRING";
//
//
//-----------------------   Constructors -------------------------------------
  Parms::Parms() {
    _num=0;
   }

//------------------------  numOfEntries --------------------------------------
  int Parms::numOfEntries() const {
     return _num;
  }

//------------------------  add  --------------------------------------------
  int Parms::add( const Name& name, const Name& value, const char* type) {
    _num++;


// checks on correctness of declared type    
    if ( value == noval || !strcmp(type,undef) ){ }
    else {
       if ( !strcmp(type,"STRING") ) {} // string can be anything
       else if ( !strcmp(type,"INT") ) {
            if ( !check_int(value) ) {
//               cout << " Default value of " << name << " is not INT"<< endl;
//               cout.flush();
//               abort();
		return 0;
            }
       }
       else if ( !strcmp(type,"FLOAT") ) {
            if ( !check_float(value) ) {
//               cout << " Default value of " << name << " is not FLOAT"<< endl;
//               cout.flush();
//               abort();
		return 0;
            }
       }
       else {
//          cout << " Unknown type : " << type << endl;
//          cout.flush();
//          abort();
		return 0;
       }
    }

    _types.add(type);
    _names.add(name);
    _values.add(value);
    
    return 1;
  }


//------------------------  add  --------------------------------------------
  int Parms::add( const Name& name, const Name& value) {

    int flg = add(name,value,undef);
    
    return flg;
  }

//------------------------  add  --------------------------------------------
  int Parms::add( const Name& name, const char* type) {


    int flg = add(name,noval,type);

    return flg;
  }

//--------------------------- out -------------------------------------------

  void Parms::out(const Name offset = "\0") const {
	char* ptn=offset.getString(); cout << ptn << endl;
     if ( _num > 0 ) {
        for ( int ii=0; ii<_num; ii++) {
            cout << ptn << _types[ii] << _names[ii] << _values[ii] << "\n";
            cout.flush();
        }
     }
  }


  int Parms::get( const Name& name, Name& value, char* type) const {

     if ( _num <= 0 ) {return 0;}

     for ( int i=0; i<_num; i++) {
         if ( name == _names[i] ) {
            value = _values[i];
            _types[i].whatAreYou(type,7);
            return 1;
         }
     }
     return 0;

  }


  void Parms::get( const int inx, Name& name, Name& value, char* type) const {
	
     if ( inx < 0 || inx >= _num ) {
        cout << " Illegal index : " << inx << endl;
        cout.flush();
        abort();
     }

     name = _names[inx];
     value = _values[inx];
     _types[inx].whatAreYou(type,7);
  }

//------------------------------------------- BF 28-March-2000 ------------------
void Parms::initFromSMLcode(const int declaration,SMLlineVector* pSMLcode,
				const int ist, const int jst,  
				int& inext, int& jnext)
 {
	int inbl,jnbl,iprev,jprev; 
	char item = firstNonBlank(pSMLcode,ist,jst,inbl,jnbl,inext,jnext,iprev,jprev);
	if (item == '\0' || item == '#' || item == '!' ) {
		ErrorWarning::printHead("ERROR",(*pSMLcode)[ist],
		                        "No parameters found");
		exit(2);
	}

	char firstChar = item;


	int istt,jstt,idel,jdel; char del;
	if (firstChar == '(' ) {istt = inext; jstt = jnext;}
	else				{istt=inbl; jstt=jnbl;}

//cout << "First char |" << firstChar << "|" << endl;
//pSMLcode->out();

    for (;;) {
		getNextParameter(declaration, pSMLcode, istt,jstt,
			idel,jdel,del,inext,jnext);

//cout << "delimiter |" <<del<<"|"<< " inext jnext " << inext << " " << jnext << endl;
		if (del==')') {
			if (firstChar == '(') {return;}
			else
			{
				ErrorWarning::printHead("ERROR",(*pSMLcode)[istt],
		                "Brackets do not balance while parsing parameters");
				exit(2);
			}	
		}
		if (inext<0) {
			if (firstChar == '(')
			{
				ErrorWarning::printHead("ERROR",(*pSMLcode)[istt],
				"Brackets do not balance while parsing parameters");
				exit(2);
			}
			else { return; }
		}
		istt = inext; jstt = jnext;
	}
	return;
 }

//------------------------------------------- BF 28-March-2000 ------------------
void Parms::getNextParameter(const int declaration, SMLlineVector* pSMLcode,
							const int ist, const int jst,
					   int& idel, int& jdel, char& del, int& inext, int& jnext)
{

	SMLline line; char* pLine;
	
	SMLline lineToReport;  // in case of errors

//cout << " ist jst " << ist << " " << jst << endl;

	int istt,jstt; int ierr;
	Name token,type,name,value,typev; char typevch[8];
	
	lineToReport = (*pSMLcode)[ist];
	del = getNextToken(pSMLcode,ist,jst," ,=)",token,idel,jdel,inext,jnext);
	if ( token == "\0" ) {
		ErrorWarning::printHead("ERROR",lineToReport,
		                        "parameters parsing");
		exit(2);
	}
//cout << " Getting the next parm: token :" 
//<< token << " delimiter after |" << del << "|" <<endl;

	if (del == ' ')
	{      // this token can only be 'type', the next token must be the 'name'
		token.upCase();
		if ( token == typestring || token == typeint || token == typefloat) {
			type = token;
//cout << " Type declaration " << type << endl;
		}
		else
		{
			ErrorWarning::printHead("ERROR",lineToReport);
			cout << "Unknown parameter type " << type << endl;
			exit(2);
		}
		
		// now get the 'name'
		istt = inext; jstt = jnext;
		lineToReport = (*pSMLcode)[istt];
		del = getNextToken(pSMLcode,istt,jstt,",=)",name,idel,jdel,inext,jnext);
		if ( name == "\0" ) // name not found
		{
			ErrorWarning::printHead("ERROR",lineToReport);	
			cout << " parameter name after a valid type not found" << endl;
			exit(2);
		}
//cout << " Parameter name " << name << "  delimiter after |" << del << "|" << endl;
	}  // should now have 'type' and 'name'
	
	else
	{  // the delimiter is either , or = or ) or /0 and therefore the token must be 'name'
	   // as type we choose STRING. This is correct in case of declaration.
		type = typestring;
		name = token;
	}

	name.upCase(); name.trim();
	if(!check_name(name))
	{
		ErrorWarning::printHead("ERROR",lineToReport);
		cout << " String " << name << " is not a name " << endl;
		exit(2);
	}
	
	if (del == '=' )
	{    //  the next token in this case must be 'default value' of the parameter
		line = (*pSMLcode)[inext];	pLine = line.getString();

		lineToReport = (*pSMLcode)[inext];
		del = getValue(pSMLcode,inext,jnext,",)",value,typevch,ierr,
				idel,jdel,inext,jnext);
		typev = typevch;
//cout << " returned from getValue " << endl;
//cout << " type |" << typevch << " value " << value << endl;
//cout << " inext jnext" << inext << " " << jnext << endl;

//cout << ierr << typev << value << endl;
		if (ierr != 0 )
		{
			ErrorWarning::printHead("ERROR",lineToReport);
			cout << " Error parsing parameter value" << endl;
			exit(2);
		}
		if (typev == "UNKNOWN")
		{
			ErrorWarning::printHead("ERROR",lineToReport);
			cout << " Error parsing parameter value" << endl;
			exit(2);
		}
		if (declaration)
		{      // if this is a declaration, we can check the types
			if ( !(type == typev) )
			{
				ErrorWarning::printHead("ERROR",lineToReport);
				cout << "Parameter value inconsistent with type" << endl;
				exit(2);
			}
			char typech[9];
			type.whatAreYou(typech,8);
			add(name,value,typech); // no need to check the return, we made sure it is OK
		}
		else {
			add(name,value);  // in case of non-declaration, the type will go in as undefined
		}
	}
	
	else
	{  // as the delimiter was either , or ) we finished with the parameter
		if(declaration) {
			char typech[9];
			type.whatAreYou(typech,8);
			add(name,typech);
		}
		else
		{   // if it was not a declaration, it is 'DO' instruction and every parameter
		    // has to have value
			ErrorWarning::printHead("ERROR",lineToReport);
			cout << " Value of parameter must be present" << endl;
			exit(2);
		}

	}
//cout << type << name << value << endl;
	return;
}


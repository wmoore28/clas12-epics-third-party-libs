//--------------------------  ObjectRegistrar Class  -----------------------------------
#include <assert.h>
#include <string.h>
#include "parameters.hxx"
#include "name.hxx"
#include "smiobject.hxx"
#include "objectregistrar.hxx"
//                                                         21-August-1996
//                                                          B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//---------------------------------------------------------------------------
//
//--------------------------- Registrar --------------------------------------
ObjectRegistrar::ObjectRegistrar(){

   _num = 0;  _size = 0;

}

//
//--------------------------- Registrar --------------------------------------
ObjectRegistrar::~ObjectRegistrar(){
//
   if ( _size > 0 ) {
      delete [] _pbase;
   }
}

//--------------------------- registerObject ----------------------------------
int ObjectRegistrar::registerObject( SMIObject * const pSmiObj) {
   SMIObject** pbasenew;


    if( _num >= _size ) {
       pbasenew = new SMIObject* [_size+10];
       assert (!pbasenew == 0);
       if ( _size > 0 ) {
          for ( int i=0; i<_size; i++) {
              pbasenew[i] = _pbase[i];
          }
          delete [] _pbase;
       }
       _pbase = pbasenew;
       _size = _size + 10;
    }
    _pbase[_num] = pSmiObj;
    _num++;

    return 1;
}

//---------------------------- gimePointer ------------------------------------
SMIObject *ObjectRegistrar::gimePointer(const Name& objname) const {

   SMIObject* pObj;
//   cout << " Looking for : " << objname << endl;
//   cout.flush();

   if ( _num == 0 ) { 
      cout << " no objects registered " << endl;
      cout.flush();
      abort();
   }

   for (int io = 0; io < _num; io++) {
      pObj = _pbase[io];
//      cout << " io " << io << " _pbase[io] " << _pbase[io] << endl;
//      cout.flush();
//      cout << pObj->name() << endl;
//      cout.flush();
      if ( objname == pObj->name() ) {return _pbase[io];}
   }   

   return 0;
}

SMIObject *ObjectRegistrar::gimePointer(const char* objname) const {
   Name temp_name;

   temp_name = objname;

   SMIObject* crap=gimePointer(temp_name);
   return crap;
}


SMIObject *ObjectRegistrar::gimePointer(const int inx) const {

   if ( inx >= _num || inx < 0) { 
      cout << " illegal index : " << inx << endl;
      cout.flush();
      abort();
   }


   return _pbase[inx];
}
//--------------------------- numOfObjects ------------------------------------
int ObjectRegistrar::numOfObjects() const {
   return _num;
}

//--------------------------- gimeName ----------------------------------------
Name& ObjectRegistrar::gimeName(const int inx) const {

   if ( inx >= _num || inx < 0) { 
      cout << " illegal index : " << inx << endl;
      cout.flush();
      abort();
   }

   return _pbase[inx]->name();
}
//---------------------------  out  -------------------------------------------
void ObjectRegistrar::out(Name offset) const {
    int num = numOfObjects();
    
    for (int i=0; i<num; i++) {
        SMIObject* ptn = gimePointer(i);
	cout << offset.getString() << ptn->name() << "   " << ptn << endl;
    }
    return;
}

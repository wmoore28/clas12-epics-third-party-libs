//---------------------- ObjectRegistrar   Class ---------------------------
#ifndef OBJECTREGISTRAR_HH
#define OBJECTREGISTRAR_HH

//
//   This class  holds a list of pointers
//   to SMI Objects .
//                                                          28-October-1996
//                                                            B. Franek
// Copyright Information:
//      Copyright (C) 1996-2001 CCLRC. All Rights Reserved.
//
//                                                    
//--------------------------------------------------------------------------
//
class Name;
class SMIObject;


class ObjectRegistrar {
public :
  ObjectRegistrar();

  ~ObjectRegistrar();

  SMIObject *gimePointer(const char* objname) const;

  SMIObject *gimePointer(const Name& objname) const;

  SMIObject *gimePointer(const int inx) const;

  int registerObject( SMIObject * const pSMIObj); 

  int numOfObjects() const;

  Name& gimeName(const int inx) const;
  
  void out(Name offset) const;


private :

   int _num,  _size;

   SMIObject ** _pbase;    // array of pointers to SMI Objects
};


#endif

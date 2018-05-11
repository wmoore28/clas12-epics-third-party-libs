//----------------------  Class   Block_Name  ---------------------------------
//
//                                                      Author: Boda Franek
//                                                      Date : 4 April 1997
//----------------------------------------------------------------------------
#include <stdlib.h>
#include "smixx_common.hxx"
#include <assert.h>
#include "string.h"
#include "block_name.hxx"


//-----------------------------  Block_Name  -----------------------------------
Block_Name::Block_Name() { 
   _num = 0; _size = 0; 
}


//-----------------------------  ~Block_Name  ---------------------------------
Block_Name::~Block_Name() {
//
   if ( _num > 0 ) {
       for (int i=0; i<_num; i++) {
           delete _pName[i];
       }
   }
//
   if ( _size > 0 ) {
      delete [] _pName;      
   }
}


//-----------------------------  add ---------------------------------------

void Block_Name::add( const Name &val) {
   Name* ptemp;
   Name** pNamenew;


    if( _num >= _size ) {
       pNamenew = new Name* [_size+10];
       assert (!pNamenew == 0);
       if ( _size > 0 ) {
          for ( int i=0; i<_size; i++) {
              pNamenew[i] = _pName[i];
          }
          delete [] _pName;
       }
       _pName = pNamenew;
       _size = _size + 10;
    }
    ptemp = new Name();
    assert (!ptemp == 0 );
    *ptemp = val;
    _pName[_num] = ptemp;
    _num++;
}


//------------------------------------  add  --------------------------------
void Block_Name::add(const char* str) {
   Name temp;
   temp = str;
   add(temp);
}

void Block_Name::append(Block_Name& blockname) {

   Name tempnm;
   
   for ( int i=0; i<blockname._num; i++) {
       tempnm = blockname[i];
       add(tempnm);
   }
}

//------------------------------------  []  -----------------------------------
Name& Block_Name::operator[] (int index) const {
    if ( index >= _num || index < 0 ) { 
          cout << " illegal index : " << index << "  _num : " << _num << "\n";
		  cout.flush();
          abort();
    }
    return *_pName[index];
}


//------------------------------------  clear  -------------------------------
void Block_Name::clear() {
//
   for (int i=0; i<_num; i++) {
       delete _pName[i];
   }
//
   delete [] _pName;      

   _size = 0;
   _num = 0;

}



//----------------------------------  out  -----------------------------------
void Block_Name::out() const {

   for (int i=0; i<_num; i++) {
       _pName[i]->out(); cout << "\n";
   }
   cout.flush();

   return;
}


ostream& operator<<(ostream& os, Block_Name& block) {

   cout << " Block of names    Num of entries : "<< block._num << "\n";
   cout.flush();

   if ( block._num > 0 ) {
       for (int i=0; i<block._num; i++) {
            os << i << " " << *block._pName[i] << "\n";
       }
   }   

   return os;
}

//----------------------------------  numOfEntries  --------------------------
int Block_Name::numOfEntries() const {

    return _num;
}


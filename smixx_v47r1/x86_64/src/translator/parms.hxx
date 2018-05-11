//----------------------  Class   Parms  -----------------------------------
//
//
//                                                      Author: Boda Franek
//                                                      Date : 10 June 1998
//                                                Revised: BF 26 March 1999
//                                                Revised: BF March 2014
//
//----------------------------------------------------------------------------
#ifndef PARMS_HH
#define PARMS_HH
#include "typedefs.hxx"
#include "block_name.hxx"
class Name;
class SMLlineVector;
//----------------------------------------------------------------------------

class Parms {
   public :
      Parms(); 

      int numOfEntries() const;

      void out(const Name offset) const;

      int get( const Name& name, Name& value, char* type) const;//0 not found

      void get( const int inx, Name& name, Name& value, char* type) const ;

// when something goes wrong, Translator is terminated inside the method
// that means it can be void now.

	void initFromSMLcode(const int declaration, SMLlineVector* pSMLcode,
				const int ist, const int jst,  
				int& inext, int& jnext);

   private :
   
      int add( const Name& name, const Name& value, const char* type);
// type="STRING","INT" or "FLOAT"

      int add( const Name& name,  const char* type);

      int add( const Name& name, const Name& value);

	void getNextParameter(const int declaration, SMLlineVector* pSMLcode, const int ist, const int jst,
					   int& idel, int& jdel, char& del, int& inext, int& jnext);

	int _num;
	Block_Name _types,_names,_values;
};

#endif

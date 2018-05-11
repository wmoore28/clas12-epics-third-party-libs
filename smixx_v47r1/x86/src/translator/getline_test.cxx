/*
//----------------------   M a i n   P r o g r a m  --------------------------
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <fstream.h>
#include "utilities.hxx"
#include "name.hxx"
//-----------------------------------------------------------------------------

int main(int argc, char* argv[]) {

  char inputFile[80];

  strcpy(inputFile,argv[1]);


  cout << " Input file : |" << inputFile << "| \n"; 
       

       ifstream input_file(inputFile);

       if (input_file.bad()) {
          cout << " CodeBlock Could not open file :" << inputFile << endl;
          exit(0);
       }

       Name line;

       for (;;) {
          int flag = readLine(input_file,line);
          if ( flag == 1 ) { cout  << line <<  endl; continue; }

          if ( flag == 0 ) {
              cout << "Error reading file\n";
              cout.flush();
              exit(0);
          }

          else if ( flag == 2 ) {
              input_file.close();
              exit(1);
          }
 
          else { 
              cout << " unknown flag " << flag << endl; cout.flush(); exit(0);
          }
       }

   exit(1);
}*/

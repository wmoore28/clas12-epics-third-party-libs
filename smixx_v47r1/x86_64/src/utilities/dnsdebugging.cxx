#include <stdlib.h>
#include "smixx_common.hxx"
#include <stdio.h>
#include <dic.hxx>

int main(int argc, char* argv[])
{
  //  cout << argc << "  argv[1]: |" << argv[1] << "|" << endl; exit(2);

  if ( argc != 2 ) {
    cout << "  usage :  dnsDebugging 1 or 0" << endl;
    exit(2);   
  }

  int debug;
  sscanf(argv[1],"%d",&debug);
  if (debug != 0 && debug != 1) {
    cout << "  usage :  dnsDebugging 1 or 0" << endl;
    exit(2);
  }


  if (debug == 1 ) {DimClient::sendCommand("DIS_DNS/DEBUG_ON"," ");}

  if (debug == 0 ) {DimClient::sendCommand("DIS_DNS/DEBUG_OFF"," ");}
}



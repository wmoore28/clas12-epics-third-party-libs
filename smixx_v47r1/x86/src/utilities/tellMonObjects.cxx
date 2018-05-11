
//---------------------- Client sends commands ---------------
#ifdef WIN32
#include <windows.h>
#endif
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <dic.hxx> 
using namespace std;
//===========================================================================
int getNode(string& node) {
	char hostname[256];
	int flg;
#ifdef WIN32
	int init_sock();
	init_sock();
#endif	
	flg = gethostname(hostname,255);
	if ( flg != 0 ) {
		return 1;
	}
//	cout << flg << hostname << endl;
	node = hostname;
	
	return 0;
}
//==========================================================================
int main( int argc, char* argv[] ) 
{ 
	string serviceName; int iarg;
	
//    printf("%d\n",argc);
	if ( argc < 2 ) {
		printf(" ***error  no arguments \n");
		exit (1);
	}
	
	if ( getNode(serviceName) != 0 ) {
		cerr << "Error  Can not determine pid/host " << endl;
		exit(1);
	}
	
	iarg = 1;
	if (argc == 3) {
		serviceName = serviceName + "/" + argv[1];
		iarg = 2;
	} 
	serviceName = serviceName + "/SMIGUI/CMNDS";


//cout << "|" << serviceName.c_str() << "|" << endl;
//cout << "|" << argv[iarg] << "|" << endl;
	
	DimClient::sendCommand(serviceName.c_str(),argv[iarg]); 
	exit (0);
} 

//------------------------------------------------------------


#ifdef WIN32
int init_sock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	static int sock_init_done = 0;

	if(sock_init_done) return(1);
 	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) 
	{
    	return(0);
	}

	/* Confirm that the WinSock DLL supports 2.0.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.0 in addition to 2.0, it will still return */
	/* 2.0 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 ) 
	{
	    WSACleanup( );
    	return(0); 
	}
	sock_init_done = 1;
	return(1);
}
#endif


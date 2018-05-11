#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
using namespace std;
int main() {

	int pid;
	char cpid[10];
	char hostname[256];
	int flg;
	char serviceName[512];
	
	pid =getpid();
	flg = gethostname(hostname,255);
	if ( flg != 0 ) {
		return 1;
	}
	
	strcpy(serviceName,"SMIGUI/SERVER/CMNDS/");
	
	strcat(serviceName,hostname);
	strcat(serviceName,"/");
	
	sprintf(cpid,"%d",pid);
	strcat(serviceName,cpid);
	
//	printf("%s",serviceName);
	cout << serviceName;
    return 0;
}

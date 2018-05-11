#include <time.h>
char* getTimeString() {
	time_t t;
        char* z;
        
        t = time((time_t *)0);
        z = ctime(&t);
        *(char *)strchr(z,'\n') = '\0';
        return z;
}

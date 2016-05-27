#ifndef __COMMON__
#define __COMMON__
#include<errno.h>
#include<stdlib.h>

# define err_exit(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

#endif //__COMMON__


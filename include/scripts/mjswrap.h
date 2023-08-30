#ifndef MJSWRAP
#define MJSWRAP

#include <devlib/cdev.h>

#define MJSEXEC_SUCCESS 0
#define MJSEXEC_FAIL 1

int mjsexec(int argc, char **argv, DEV* console);

#endif
#ifndef LOG
#define LOG

#include <devlib/cdev.h>

unsigned int setlogdev(DEV* device);
unsigned int write_log(char* message);
void printchartohex(uint8_t input);

#endif
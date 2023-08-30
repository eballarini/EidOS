#include <devlib/cdev.h>
#include <sys/sysutils.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LOG_SIZE 100

DEV* log_dev;

unsigned int setlogdev(DEV* device)
{
    log_dev=device;
}

unsigned int write_log(char* message)
{
    writearray(log_dev, message, MAX_LOG_SIZE, '\0');
}

void printchartohex(uint8_t input)
{
    char literal[3];
    
    ctoliteral(input,literal);
    write_log (literal);
}

void log_string_int(char* message, uint32_t data)
{
   char literal[10]="\0";
   
   writearray(log_dev, message, MAX_LOG_SIZE, '\0'); 
   
   writearray(log_dev, literal, MAX_LOG_SIZE, '\0'); 
}
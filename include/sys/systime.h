#ifndef SYS_TIME
#define SYS_TIME

#include <FreeRTOS.h>

typedef uint32_t time_t;

typedef struct
{
    uint32_t year;
    uint32_t mon;
    uint32_t mday;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
    uint32_t tm_wday;	
    uint32_t tm_yday;	
    uint32_t tm_isdst;
}tm;

void time_unix_to_tm(tm* tm_time, time_t unix_time);

time_t time(time_t offset);

void vTask_systime_update();

#endif
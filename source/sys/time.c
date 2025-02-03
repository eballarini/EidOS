/*
EidOS - a small RTOS for PIC microntrollers
Copyright (C) 2022  Emanuele Ballarini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <sys/systime.h>
#include <task.h>

uint32_t systime;

void time_unix_to_tm(tm* tm_time, time_t unix_time)
{
    uint32_t era_day,cicle_day,cicles_complete,year_in_cicle, january_days, february_days, march_days, april_days, may_days, june_days, july_days, august_days, september_days, october_days, november_days, dicember_days;
    
    tm_time->second=unix_time%60;
    tm_time->minute=(unix_time/60)%60;
    tm_time->hour=(unix_time/3600)%24;
    
    //day since 1900 era_day
    era_day=unix_time/86400;
    
    tm_time->tm_wday=era_day%7;
    
    //day in a four year cicle
    cicle_day=era_day%1461;
    
    //4 year cicles completed
    cicles_complete=era_day/1461;
    
    //years completed
    if (era_day!=0)
        year_in_cicle=(era_day-1)/365;
    else
        year_in_cicle=0;
    
    tm_time->year= cicles_complete*4+year_in_cicle;
    
    //day in the year
    
    
    if (era_day<366)
        tm_time->tm_yday=era_day;
    else
        tm_time->tm_yday=(era_day-1)%365;
    
    if(year_in_cicle==0)
        february_days=29;
    else
        february_days=28;
    
    january_days=31;
    february_days+=january_days;
    march_days=february_days+31;
    april_days=march_days+30;
    may_days=april_days+31;
    june_days=may_days+30;
    july_days=june_days+31;
    august_days=july_days+31;
    september_days=august_days+30;
    october_days=september_days+31;
    november_days=october_days+30;
    dicember_days=november_days+31;
    
    if (tm_time->tm_yday>0 && tm_time->tm_yday<january_days)
        {
        tm_time->mon=0;
        tm_time->mday=tm_time->tm_yday;
        }
    else if (tm_time->tm_yday>january_days && tm_time->tm_yday<february_days)
        {
        tm_time->mon=1;
        tm_time->mday=tm_time->tm_yday-january_days;
        }
    else if (tm_time->tm_yday>february_days && tm_time->tm_yday<march_days)
        {
        tm_time->mon=2;
        tm_time->mday=tm_time->tm_yday-february_days;
        }
    else if (tm_time->tm_yday>march_days && tm_time->tm_yday<april_days)
        {        
        tm_time->mon=3;
        tm_time->mday=tm_time->tm_yday-march_days;
        }    
    else if (tm_time->tm_yday>april_days && tm_time->tm_yday<may_days)
        {        
        tm_time->mon=4;
        tm_time->mday=tm_time->tm_yday-april_days;
        }    
    else if (tm_time->tm_yday>may_days && tm_time->tm_yday<june_days)
        {        
        tm_time->mon=5;
        tm_time->mday=tm_time->tm_yday-may_days;
        }    
    else if (tm_time->tm_yday>june_days && tm_time->tm_yday<july_days)
        {        
        tm_time->mon=6;
        tm_time->mday=tm_time->tm_yday-june_days;
        }    
    else if (tm_time->tm_yday>july_days && tm_time->tm_yday<august_days)
        {        
        tm_time->mon=7;
        tm_time->mday=tm_time->tm_yday-july_days;
        }    
    else if (tm_time->tm_yday>august_days && tm_time->tm_yday<september_days)
        {        
        tm_time->mon=8;
        tm_time->mday=tm_time->tm_yday-august_days;
        }    
    else if (tm_time->tm_yday>september_days && tm_time->tm_yday<october_days)
        {        
        tm_time->mon=9;
        tm_time->mday=tm_time->tm_yday-september_days;
        }    
    else if (tm_time->tm_yday>october_days && tm_time->tm_yday<november_days)
        {        
        tm_time->mon=10;
        tm_time->mday=tm_time->tm_yday-october_days;
        }    
    else if (tm_time->tm_yday>november_days && tm_time->tm_yday<dicember_days)
        {        
        tm_time->mon=11;
        tm_time->mday=tm_time->tm_yday-november_days;
        }    
    
    tm_time->tm_isdst=0;
}

time_t time(time_t offset)
{
    return systime+offset;
}


void vTask_systime_update()
{
const TickType_t delay = 1000 / portTICK_PERIOD_MS;    

for (;;)
{
    systime+=1;
    vTaskDelay(delay);
}

}

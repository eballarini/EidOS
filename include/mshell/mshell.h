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

#ifndef MSHELL
#define MSHELL

#define CMD_MAX_NUM 200
#define CMD_DESCR_LEN 100


typedef struct msh_cmd{
    
    char* command;
    int (*function)(int, char **, DEV*);
    const char description[CMD_DESCR_LEN];
    
}MSH_CMD;

typedef struct msh_cmd_list{
    
    MSH_CMD cmd_list[CMD_MAX_NUM];
    int index;
    
}MSH_CMD_LIST;

void MshInitialize();

void vTaskmShell(unsigned int device_num);

void vTaskCon( void* pvParameters);

void MshCommandRegister(char* cmd, int (*function)(int argc, char **argv, DEV* console));

unsigned int MshCommandParser(char* cmd, unsigned int len, char *argv[]);

void MshCommandExecuter(int argc, char **argv, DEV* console);

#endif

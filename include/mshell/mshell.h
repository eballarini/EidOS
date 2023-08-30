
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
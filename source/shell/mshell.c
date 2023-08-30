
#include <devlib/cdev.h>
#include <mshell/mshell.h>
#include <string.h>
#include <sys/hardwaredefs.h>
#include <system_config.h>
#include <sys/log.h>
#include <sdcard/sdcard.h>
#include <source/ff.h>
#include <driver/pic32/eth_macp32_phy8710a.h>
#include <task.h>
#include <mjswrap.h>

/*max input size from cli*/
#define MSH_MAX_ARRAY_SIZE 100

/*max argument size and number*/
#define MSH_MAX_CMD_SIZE 100
#define MSH_MAX_ARG_NUM 20

extern SD_dev sd[];
FATFS root_fs;

char test_command[]="read";
char test_mount_command[]="mount";
char ls_command[]="ls";
char pfile_command[]="pfile";
char pcsd_command[]="pcsd";
char write_command[]="write";
char pcid_command[]="pcid";
char ethstatus_command[]="ethstatus";
char ethinit_command[]="ethinit";
char ethwrite_command[]="ethwrite";
char ethstats_command[]="ethstats";
char mjs_command[]="mjs";

MSH_CMD_LIST msh_commands;

int test_cmd_func(int argc, char **argv, DEV* console);

int test_fs(int argc, char **argv, DEV* console);
int test_mount_fs(int argc, char **argv, DEV* console);

int list_dir(int argc, char **argv, DEV* console);
int print_file(int argc, char **argv, DEV* console);

int print_csd(int argc, char **argv, DEV* console);
int print_cid(int argc, char **argv, DEV* console);

int write_file(int argc, char **argv, DEV* console);

int print_ethphy(int argc, char **argv, DEV* console);

int init_ethphy(int argc, char **argv, DEV* console);

int write_ethphy(int argc, char **argv, DEV* console);

int print_ethstats(int argc, char **argv, DEV* console);

void MshInitCommands(void)
{   
    MshCommandRegister(test_command, test_fs);
    MshCommandRegister(test_mount_command, test_mount_fs);
    MshCommandRegister(ls_command, list_dir);
    MshCommandRegister(pfile_command, print_file);
    MshCommandRegister(pcsd_command, print_csd);
    MshCommandRegister(pcid_command, print_cid);
    MshCommandRegister(write_command, write_file);
    MshCommandRegister(ethstatus_command, print_ethphy);
    MshCommandRegister(ethinit_command, init_ethphy);
    MshCommandRegister(ethwrite_command, write_ethphy);
    MshCommandRegister(ethstats_command, print_ethstats);
    MshCommandRegister(mjs_command, mjsexec);
}

void vTaskmShell(unsigned int device_num)
{
    
 TickType_t xLastWakeTime;
 char command[100];
 char banner[]="*** Welcome to EidOS ! ***\r\n";
 char prompt='>';
 DEV* console;
 DEV_PARAM device_conf;
 int i;
 
 unsigned int argc=0;
 char **argv;
 char *test;
 
 
 argv=pvPortMalloc(sizeof(char*)*MSH_MAX_ARG_NUM);
 
 for(i=0; i< MSH_MAX_ARG_NUM; ++i)
 {
 argv[i]=pvPortMalloc(MSH_MAX_CMD_SIZE);
 }
 
 device_conf.dev_num=device_num;
    
 device_conf.config=cdev_get_conf(device_conf.dev_num);
 
 console=opendev(device_conf.config, device_conf.dev_num);
 
 //initialize console as initial log device
 setlogdev(console);
 
 MshInitialize();
 
 /*initialize command list for test purposes*/
 MshInitCommands();
 
 writearray(console, banner, MSH_MAX_ARRAY_SIZE, '\n');
 
 while(1)
 { 
       
 writechar(console, prompt);
 
 readechoarray( console, command, MSH_MAX_ARRAY_SIZE, '\r');
 
 argc=MshCommandParser( command, MSH_MAX_ARRAY_SIZE, argv);
 
 MshCommandExecuter(argc, argv, console);
 
 xLastWakeTime = xTaskGetTickCount();
 
 vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 1 ) );
 
 }
 
}

void vTaskCon(void* pvParameters)
{
    vTaskmShell(CONSOLE_UART);
}

void MshInitialize()
{
    msh_commands.index=0;
}

void MshCommandRegister(char* cmd, int (*function)(int, char **, DEV*))
{
    if (msh_commands.index<CMD_MAX_NUM)
   {
   msh_commands.cmd_list[msh_commands.index].command=cmd;
   msh_commands.cmd_list[msh_commands.index].function=function;
   msh_commands.index++;
   }
}

unsigned int MshCommandParser(char* cmd, unsigned int len, char *argv[])
{
    unsigned int i=0, arg_count=0, count=0;
    char endian='\r';
    
    for (i=0; i<len; i++)
    {
            if (cmd[i]!=' '&&cmd[i]!=endian)
            {
                argv[arg_count][count]=cmd[i];
                count++;
            }
            else if (cmd[i]==' ')
            {
                argv[arg_count][count]='\0';
                arg_count++;
                count=0;
            }
            else 
            {
            argv[arg_count][count]='\0';
            return arg_count+1;
            }
            
            if (count>=MSH_MAX_CMD_SIZE)
                {
                argv[arg_count][MSH_MAX_CMD_SIZE]='\0';
                arg_count++;
                while(cmd[i]!=' '&&cmd[i]!=endian)
                {
                    i++;
                }
                count=0;
                }
            if (arg_count>=MSH_MAX_ARG_NUM)
                {
                //debugging
                //USER_LED2=~USER_LED2;
                return MSH_MAX_ARG_NUM;

                }
                
    } 
    
    argv[arg_count][count]='\0';
    return arg_count+1;
}

void MshCommandExecuter(int argc, char **argv, DEV* console)
{
 int cmp, i;
 
 for(i=0;i<msh_commands.index; i++)
 {
 cmp = strcmp(argv[0], msh_commands.cmd_list[i].command);
 
 if (cmp==0)
 {  
    writearray(console, "\r\n", 2, '\n');
    msh_commands.cmd_list[i].function(argc,argv,console); 
 }
 }
 
 writearray(console, "\r\n", 2, '\n');
}



int test_mount_fs(int argc, char **argv, DEV* console)
{
    FRESULT res;  
    
    setlogdev(console);
    
    res=f_mount(&root_fs,"",1);
    
    if(res!=FR_OK)
      {
        write_log ("\r\nfs: mount error\0");
        return 1;
      }
    else
        {
       write_log ("\r\nfs: fyle system mounted\0"); 
        }
    
    return 0;
}

int test_fs(int argc, char **argv, DEV* console)
{

    
    setlogdev(console);

    sd_test_read();
     
}

int list_dir(int argc, char **argv, DEV* console)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, argv[1]);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                writearray(console, "\r\ndirectory: \0" , MSH_MAX_ARRAY_SIZE, '\0');
            } else {                                       /* It is a file. */
                writearray(console, "\r\nfile: \0" , MSH_MAX_ARRAY_SIZE, '\0');
            }
            writearray(console, fno.fname , strlen(fno.fname), '\0');
        }
        f_closedir(&dir);
    }

    return res;
}

int print_file(int argc, char **argv, DEV* console)
{
    FIL file;
    FRESULT res, read_res; 
    uint8_t * buffer;
    UINT bytes_read;
    
    res = f_open(&file, argv[1], FA_READ);
    
    buffer=pvPortMalloc(10);
    
    if(buffer==0)
    {
       writearray(console, "\r\nmalloc failed\r\n\0" , MSH_MAX_ARRAY_SIZE, '\0');   
       return res;
    }
    
    if (res == FR_OK) 
    {
        
        do
        {
            if((f_read(&file, (void*)buffer, 10, &bytes_read))==FR_OK)
                swritearray(console,buffer,bytes_read); 
            else
            {
                writearray(console, "\r\nfile read failed \r\n\0" , MSH_MAX_ARRAY_SIZE, '\0');
                bytes_read=0;
            }   
        }while(bytes_read>0);
        
        f_close(&file);
    }
    else
      writearray(console, "\r\nfile open failed \r\n\0" , MSH_MAX_ARRAY_SIZE, '\0');  
    
    vPortFree(buffer);
    
    return res;
}

int write_file(int argc, char **argv, DEV* console)
{
    FIL file;
    FRESULT res; 
    char *len;
    UINT bytes_written;
    uint8_t * buffer;
    UINT bytes_read;
    
    buffer=pvPortMalloc(3);
    
    res = f_open(&file, argv[1], FA_CREATE_NEW | FA_WRITE);
    
    if (res == FR_OK) {
        
        res=f_write(&file, argv[2], strlen(argv[2]), &bytes_written);
        
        if(res==FR_OK)
            {
              write_log ("\r\n file written \r\n\0");
            }
        else
            write_log ("\r\n file write failed \r\n\0");
        
        
        f_close(&file);
        }
    else
        write_log ("\r\n file open failed \r\n\0");
      
    
}

int print_csd(int argc, char **argv, DEV* console)
{
    
    write_log ("\r\n csd register: \r\n\0");
    sd_print_csd(&sd[0]);
}

int print_cid(int argc, char **argv, DEV* console)
{
    
    write_log ("\r\n cid register: \r\n\0");
    sd_print_cid(&sd[0]);
}

void print_ethphy_reg(uint8_t reg_addr)
{
    uint16_t data;
    
    p32_mac_lan8710a_readreg(reg_addr,&data);   
    printchartohex((uint8_t)(data>>8));
    printchartohex((uint8_t)data);
    write_log ("\r\n\0");
}

int print_ethphy(int argc, char **argv, DEV* console)
{
    uint16_t data;
    
    write_log ("\r\n ethphy registers: \r\n\0");
    
    
    //print_ethphy_reg(LAN8710A_BASE_CTRL_REG);
   /* write_log ("\r\n base ctrl register: \r\n\0");
    print_ethphy_reg(LAN8710A_BASE_CTRL_REG);
    write_log ("\r\n base status register: \r\n\0");
    print_ethphy_reg(LAN8710A_BASE_STATUS_REG);
    write_log ("\r\n phy id1 register: \r\n\0");
    print_ethphy_reg(LAN8710A_PHY_ID1_REG);
    write_log ("\r\n phy id 2 register: \r\n\0");
    print_ethphy_reg(LAN8710A_PHY_ID2_REG);
    write_log ("\r\n autoneg adv register: \r\n\0");
    print_ethphy_reg(LAN8710A_AUTONEG_ADV_REG);
    write_log ("\r\n autoneg lpa register: \r\n\0");
    print_ethphy_reg(LAN8710A_AUTONEG_LPA_REG);
    write_log ("\r\n autoneg exp register: \r\n\0");
    print_ethphy_reg(LAN8710A_AUTONEG_EXP_REG);
    write_log ("\r\n mode status register: \r\n\0");
    print_ethphy_reg(LAN8710A_MODE_STATUS_REG);*/
    write_log ("\r\n special mode register: \r\n\0");
    print_ethphy_reg(LAN8710A_SPECIAL_MODE_REG);/*
    write_log ("\r\n symbol error register: \r\n\0");
    print_ethphy_reg(LAN8710A_SYMB_ERR_REG);
    write_log ("\r\n ctrl status register: \r\n\0");
    print_ethphy_reg(LAN8710A_CTRL_STATUS_IND_REG);
    write_log ("\r\n ISR register: \r\n\0");
    print_ethphy_reg(LAN8710A_ISR_REG);
    write_log ("\r\n int mask register: \r\n\0");
    print_ethphy_reg(LAN8710A_INT_MASK_REG);*/
    write_log ("\r\n phy special ctrl status register: \r\n\0");
    print_ethphy_reg(LAN8710A_PHY_SPECIAL_CTRL_STATUS_REG);
}

int init_ethphy(int argc, char **argv, DEV* console)
{
    init_eth();
}

int write_ethphy(int argc, char **argv, DEV* console)
{
    p32_mac_lan8710a_writereg(LAN8710A_BASE_CTRL_REG,0x1000);
    p32_mac_lan8710a_writereg(LAN8710A_AUTONEG_ADV_REG,0x01E1);
    p32_mac_lan8710a_writereg(LAN8710A_SPECIAL_MODE_REG,0x40E0);
    p32_mac_lan8710a_writereg(LAN8710A_PHY_SPECIAL_CTRL_STATUS_REG,0x0000);
}

int print_ethstats(int argc, char **argv, DEV* console)
{
    CheckRXframes();
}
# EidOS
a small RTOS for PIC microntrollers based on an idea (as the name suggests), FreeRTOS, Elm Chan FAT FS libraries and mJS Restricted JavaScript engine.

Current version is working on OLIMEX PIC32-EMZ144 (xc32 compiler), features available are:

- simple shell available trough serial RS232 (commands can be added if needed);
- Javascript restricted (mJS) engine integrated into shell to build custome scripts (calls to system C functions can be added);
- file read and write on SD card (FAT file system).

compile instructions:

- get EidOS, FreeRTOS (https://github.com/FreeRTOS/FreeRTOS-Kernel), Elm Chan FAT FS library and mJS library (https://github.com/cesanta/mjs);

- set proper hardware configs for clock and pin configuration in sys/hardwaredefs.h or in main file;

- write a main function and call init_main_tasks() in it (do remember to start task scheduler);

- use the following calls to function in proper functions inside diskio.c (FAT FS library): 

  initialize_sd_card(&sd[0]);
  
  sd_read_sector(&sd[0], buff, sector, 512, count);
  
  sd_write_sector(&sd[0], buff, sector, 512, count);
  
  sd_read_busy_check(&sd[0]); (in response to CTRL_SYNC command).
  
- set wrapping in loader for io functions adding the following options: --wrap=fopen --wrap=fclose --wrap=fread --wrap=fwrite --wrap=fseek --wrap=fputc --wrap=fflush --wrap=ftell --wrap=vfprintf --wrap=fprintf --wrap=vprintf --wrap=vsnprintf --wrap=snprintf --wrap=printf --wrap=fputc --wrap=fputs --wrap=malloc --wrap=calloc --wrap=free  --wrap=realloc
  
 mJS instructions:
 
- inside shell type command mjs "script file name" 

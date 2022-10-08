# EidOS
a small RTOS for PIC microntrollers based on an idea (as the name suggests), FreeRTOS and Elm Chan FAT FS libraries.

Current version is working on OLIMEX PIC32-EMZ144 (xc32 compiler), features available are:

- simple shell available trough serial RS232 (commands can be added if needed);

- file read and write on SD card (FAT file system).

compile instructions:

- get EidOS, FreeRTOS and Elm Chan FAT FS library;

- set proper hardware configs for clock and pin configuration in sys/hardwaredefs.h or in main file;

- write a main function and call init_main_tasks() in it (do remember to start task scheduler);

- use the following calls to function in proper functions inside diskio.c (FAT FS library): 

  initialize_sd_card(&sd[0]);
  
  sd_read_sector(&sd[0], buff, sector, 512, count);
  
  sd_write_sector(&sd[0], buff, sector, 512, count);
  
  sd_read_busy_check(&sd[0]); (in response to CTRL_SYNC command).

#ifndef FS_H
#define FS_H

#include <source/ff.h>

void vTaskDriveHandler(void* pvParameters);

DWORD get_fattime (void);

#endif
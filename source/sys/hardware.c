#include <sys/hardwaredefs.h>
#include <stdlib.h>
#include <stddef.h> 
#include <p32xxxx.h>
#include <devlib/cdev.h>

void prvSetupHardware()
{
    
    INTCONSET = _INTCON_MVEC_MASK;
       
    void initialize_dev_list();

}
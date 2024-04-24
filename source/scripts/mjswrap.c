# define MJS_TAG_UNDEFINED 0

#include <mjs.h>
#include <mjswrap.h>
#include <devlib/cdev.h>
#include <task.h>
#include <devlib/cdev.h>
#include <string.h>
#include <scripts/mjsresolv.h>
#include <sys/eid_io.h>
#include <source/ff.h>

typedef struct
{
  DEV* stdev;
  const char * path;
  TaskHandle_t * TaskHandle;
}mjs_env_vars;

DEV* mjsdev;

void *mjs_resolver(void *handle, const char *name) {
  
  if (strcmp(name, "foo") == 0) return foo;
  else if (strcmp(name, "filetext") == 0) return filetext;
  else if (strcmp(name, "mjs_res_prints") == 0) return mjs_res_prints;
  else if (strcmp(name, "mjs_flip_gpio") == 0) return mjs_flip_gpio;
  else if (strcmp(name, "mjs_wait") == 0) return mjs_wait;
  else if (strcmp(name, "filetext") == 0) return filetext;
  else if (strcmp(name, "mjs_res_prints") == 0) return mjs_res_prints;
  else if (strcmp(name, "mjs_pwm_set_dc") == 0) return mjs_pwm_set_dc;
  else if (strcmp(name, "mjs_pwm_set_period") == 0) return mjs_pwm_set_period;  
  else if (strcmp(name, "mjs_pwm_on") == 0) return mjs_pwm_on;  
  else if (strcmp(name, "mjs_pwm_off") == 0) return mjs_pwm_off;  
  else if (strcmp(name, "mjs_gpio_set") == 0) return mjs_gpio_set;  
  else if (strcmp(name, "mjs_gpio_unset") == 0) return mjs_gpio_unset;  
  else if (strcmp(name, "mjs_gpio_flip") == 0) return mjs_gpio_flip; 
  else if (strcmp(name, "mjs_delay") == 0) return mjs_delay; 
  else if (strcmp(name, "mjs_temp_less_than") == 0) return mjs_temp_less_than; 
  else if (strcmp(name, "mjs_temp_greater_than") == 0) return mjs_temp_greater_than; 
  else if (strcmp(name, "mjs_humidity_less_than") == 0) return mjs_humidity_less_than; 
  else if (strcmp(name, "mjs_humidity_greater_than") == 0) return mjs_humidity_greater_than; 
  else if (strcmp(name, "mjs_light_less_than") == 0) return mjs_light_less_than; 
  else if (strcmp(name, "mjs_light_greater_than") == 0) return mjs_light_greater_than; 
  else if (strcmp(name, "mjs_pressure_less_than") == 0) return mjs_pressure_less_than; 
  else if (strcmp(name, "mjs_pressure_greater_than") == 0) return mjs_pressure_greater_than; 
  
  return NULL;
}

//file_path not needed anymore
void mjswrap(void * pvParameters/*, const char *file_path*/)
{   
    mjs_env_vars * env;
    mjs_val_t res = MJS_UNDEFINED;
    mjs_err_t ret;
    
    size_t obj_read;
    
    FIL file;
    FRESULT fres, read_res; 
    uint8_t * buffer;
    UINT bytes_read;
    
    //setting environment
    env=(mjs_env_vars *) pvParameters;
    
    mjsdev=env->stdev;
    
    //declare mjs struct
    struct mjs *mjs = mjs_create();

    //set embedded function resolver
    mjs_set_ffi_resolver(mjs, mjs_resolver);
    
    fres = f_open(&file, env->path, FA_READ);
    
    if (fres == FR_OK) 
    {
        buffer=pvPortMalloc(f_size(&file)+1);
        
        if(buffer==0)
        {
            __wrap_printf("\r\nmalloc failed\r\n");   
        }
        
        obj_read=__wrap_fread(buffer, f_size(&file), 1, (FILE *)&file);
        
        buffer[f_size(&file)]='\0';
        
        f_close(&file);
        
        //execute script: warn if failed
        ret=mjs_exec(mjs, buffer, NULL);
    
        if(ret!=MJS_OK)
            __wrap_printf("script execution failed with errors");
        
        vPortFree(buffer);
    }
    else
      __wrap_printf("\r\nfile open failed \r\n");  
    
    mjs_destroy(mjs);
    
    //delete process
    vTaskDelete( *env->TaskHandle );
    
    vPortFree(env->TaskHandle);
    
    vPortFree(env);
    
}

int mjsexec(int argc, char **argv, DEV* console)
{
    BaseType_t task_create_result;
    //variable environment
    mjs_env_vars * env;
    char* file_path;
    
    //allocating environment variables
    env=pvPortMalloc(sizeof(mjs_env_vars));
    
    file_path=pvPortMalloc(strlen(argv[1])+1);
    
    strcpy(file_path,argv[1]);
    
    
    if(env==0)
        return MJSEXEC_FAIL;
    
    //setting console as standard device
    
    env->stdev=console;
    env->path=file_path;
    env->TaskHandle=pvPortMalloc(sizeof(TaskHandle_t));
    
    //start process
    
    //mjswrap(env);
    task_create_result=xTaskCreate(mjswrap, "mjswrap", 2000, env, 1, env->TaskHandle);
    
    //return code on startup
    if(task_create_result==pdPASS)
        return MJSEXEC_SUCCESS;
    else
        return MJSEXEC_FAIL;
}


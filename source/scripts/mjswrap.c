# define MJS_TAG_UNDEFINED 0

#include <mjs.h>
#include "mjswrap.h"
#include "devlib/cdev.h"
#include "task.h"
#include <devlib/cdev.h>
#include <string.h>
#include <scripts/mjsresolv.h>

typedef struct
{
  DEV* stdev;
}shell_env;

DEV* mjsdev;

void *mjs_resolver(void *handle, const char *name) {
  
  if (strcmp(name, "foo") == 0) return foo;
  
  return NULL;
}

void mjswrap(void * pvParameters, const char *file_path)
{   
    shell_env * env;
    mjs_val_t res = MJS_UNDEFINED;
    mjs_err_t ret;
    
    //setting environment
    env=(shell_env *) pvParameters;
    
    mjsdev=env->stdev;
    
    //declare mjs struct
    struct mjs *mjs = mjs_create();
    
    //set embedded function resolver
    mjs_set_ffi_resolver(mjs, mjs_resolver);
    
    //execute script: warn if failed
    mjs_exec(mjs, "let f = ffi('void foo(int)'); f(1234)", NULL);
    
    mjs_destroy(mjs);
    
    mjs = mjs_create();
    
    ret = mjs_exec_file(mjs, file_path, &res);
    
    mjs_destroy(mjs);
    
    //delete process
    //vTaskDelete( NULL );
    
}

int mjsexec(int argc, char **argv, DEV* console)
{
    BaseType_t task_create_result;
    TaskHandle_t * handle;
    //variable environment
    shell_env * env;
    
    //allocating environment variables
    env=pvPortMalloc(sizeof(shell_env));
    
    if(env==0)
        return MJSEXEC_FAIL;
    
    //setting console as standard device
    
    env->stdev=console;
    
    //start process
    
    mjswrap(env, argv[1]);
    //task_create_result=xTaskCreate(mjswrap, "mjswrap", 100, env, 1, handle);
    
    //return code on startup
    if(task_create_result==pdPASS)
        return MJSEXEC_SUCCESS;
    else
        return MJSEXEC_FAIL;
}


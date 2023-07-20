#include <stdio.h>
#include <stdarg.h>
#include <source/ff.h>
#include <devlib/cdev.h>
#include <string.h>

#define STRING_MAX_SIZE 100

extern DEV cdev[];

int __wrap_vsnprintf (char * str, size_t n, const char * format, va_list arg )
{
  uint32_t i=0;

  //TODO: placeholder implementation
  for(i=0; format[i]!='\0'&&i<n&&str!=NULL; i++)
  {
      //to be implemented
      str[i]=format[i];

  }
  
  if(i==n&&str!=NULL)
  { if (n=0)
      str[0]='\0';
    else
      str[i-1]='\0';
  }
  
  return i;
}

int __wrap_snprintf (char * str, size_t n, const char * format, ...)
{
   va_list vlist; 
   int ret;
   char string[200];
	
   va_start(vlist, format);
   
   ret=__wrap_vsnprintf(str, n, format, vlist);
  
   va_end(vlist);
   
   return ret;
}

int __wrap_vsprintf(char *str, const char *format, va_list arg)
{
    return __wrap_vsnprintf(str, STRING_MAX_SIZE, format, arg);
}


int __wrap_vprintf( const char* format, va_list vlist)
{
    char string[200];
    
    __wrap_vsprintf(string, format, vlist);

    //find a better way to write to stdout - define stdout before
    writearray(&cdev[0],string, 100, '\0');
    
    return strlen(string);
    
}

int __wrap_printf( const char * format, ...)
{
   va_list vlist; 
   int ret;
   char string[200];
	
   va_start(vlist, format);
   
   ret=__wrap_vprintf(format, vlist);
  
   va_end(vlist);
   
   return ret;
}

int __wrap_vfprintf(FILE* stream, const char* format, va_list vlist)
{
    char string[200];
    
    vsprintf(string, format, vlist);

    return f_puts((const TCHAR*)string, (FIL*)stream);
    
}

int __wrap_fprintf(FILE* stream, const char* format, ...)
{
   va_list vlist; 
   int ret;
	
   va_start(vlist, format);
   ret=__wrap_vfprintf(stream, format, vlist);
   va_end(vlist);
   
   return ret;
}

FILE* __wrap_fopen (const char* filename, const char* mode)
{
  FRESULT result;
  FIL* file;
  BYTE fil_mode;
  
  file=pvPortMalloc(sizeof(FIL));
  
  //file mode conversion
  
  if (strcmp (mode,"r") == 0)	
      fil_mode=FA_READ;
  else if(strcmp (mode,"r+") == 0)	
      fil_mode=FA_READ | FA_WRITE;
  else if(strcmp (mode,"w") == 0)	
      fil_mode=FA_CREATE_ALWAYS | FA_WRITE;
  else if(strcmp (mode,"w+") == 0)	
      fil_mode=FA_CREATE_ALWAYS | FA_WRITE | FA_READ;
  else if(strcmp (mode,"a") == 0)	
      fil_mode=FA_OPEN_APPEND | FA_WRITE;
  else if(strcmp (mode,"a+") == 0)	
      fil_mode=FA_OPEN_APPEND | FA_WRITE | FA_READ;
  else if(strcmp (mode,"wx") == 0)	
      fil_mode=FA_CREATE_NEW | FA_WRITE;
  else if(strcmp (mode,"w+x") == 0)	
      fil_mode=FA_CREATE_NEW | FA_WRITE | FA_READ;
  else
      return NULL;
  
  result=f_open( file, (const TCHAR*)  filename, fil_mode);
  
  if (result == FR_OK) {
      return (FILE*) file;
  }
  else
  {
      return NULL;
  }
}

size_t __wrap_fread(void * buffer, size_t size, size_t count, FILE * stream)
{
    uint32_t obj_num;
    UINT read_num;
    FRESULT result;
    
    for(obj_num=0;obj_num<size;obj_num++)
    {
          result=f_read( (FIL*) stream, buffer, (UINT) size, &read_num );
  
          if (result == FR_OK&&read_num>=size) {
              buffer+=size;
              continue;
            }         
          else
            {
              return obj_num;
            }
    }
    
    return count;
}


size_t __wrap_fwrite(const void * buffer, size_t size, size_t count, FILE * stream)
{
    uint32_t obj_num;
    UINT wrote_num;
    FRESULT result;
    
    for(obj_num=0;obj_num<size;obj_num++)
    {
          result=f_write( (FIL*) stream, buffer, (UINT) size, &wrote_num );
  
          if (result == FR_OK&&wrote_num>=size) {
              buffer+=size;
              continue;
            }         
          else
            {
              return obj_num;
            }
    }
    
    return count;
}

int __wrap_fclose(FILE* stream)
{
  FRESULT result;
  
  result=f_close( (FIL*) stream );
  
  vPortFree(stream);
  
  if (result == FR_OK) {
      return 0;
  }
  else
  {
      return EOF;
  }  
}

int __wrap_fseek(FILE* stream, long offset, int origin)
{
  FRESULT result;
  long calc_offset;
  
  
  if (origin==SEEK_SET)
      calc_offset=origin;
  else if (origin==SEEK_CUR)
      calc_offset=origin+f_tell((FIL*)stream);
  else if (origin==SEEK_END)
      calc_offset=origin+f_size((FIL*)stream);
  
  result=f_lseek( (FIL*) stream , calc_offset );
  
  if (result == FR_OK) {
      return 0;
  }
  else
  {
      return -1;
  }   
}

int __wrap_fputc(int ch, FILE* stream)
{
   int result;
    
   result=f_putc(ch, (FIL*) stream);
   
   if (result<0)
       return EOF;
   else
       return ch;
}

int __wrap_fputs(const char *str, FILE *stream)
{
    
}

int __wrap_fflush(FILE* stream)
{
  FRESULT result;
  
  result=f_sync( (FIL*) stream );
  
  if (result == FR_OK) {
      return 0;
  }
  else
  {
      return EOF;
  }
}

long __wrap_ftell(FILE* stream)
{
  return (long)f_tell((FIL*) stream);  
}
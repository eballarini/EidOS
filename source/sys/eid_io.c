#include <stdio.h>
#include <stdarg.h>
#include <source/ff.h>
#include <devlib/cdev.h>
#include <string.h>


void *__wrap_malloc (size_t size)
{
    return pvPortMalloc(size);
}

void *__wrap_calloc (size_t num, size_t size)
{
    uint32_t i,j;
    uint8_t *pointer;
    
    pointer=pvPortMalloc(num*size);
    
    if(pointer!=NULL)
    {
    for(i=0;i<num;i++)
        {
        for(j=0;j<size;j++)
            {
            pointer[i*size+j]=0;
            }
        }
    }
    
    return (void*) pointer;
}

//void *__wrap_realloc (void *, size_t);
void __wrap_free (void * pointer)
{
    vPortFree(pointer);
}

extern DEV cdev[];

int __wrap_vprintf( const char* format, va_list vlist)
{
    char string[200];
    
    vsprintf(string, format, vlist);

    return writearray(&cdev[0],string, 100, '\0');
    return strlen(string);
    /*return f_puts((const TCHAR*)string, (FIL*)stream);*/
    
}

int __wrap_printf( const char* format, ...)
{
   va_list vlist; 
   int ret;
	
   va_start(vlist, format);
   ret=vprintf(format, vlist);
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
   ret=vfprintf(stream, format, vlist);
   va_end(vlist);
   
   return ret;
}

FILE* __wrap_fopen (const char* filename, const char* mode)
{
  FRESULT result;
  FIL* file;
  
  result=f_open( file, (const TCHAR*)  filename, (BYTE) *mode);
  
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

//to be checked causing exception
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
  
  //to be done calculation of origin from SEEK_END SEEK_SET, SEEK CUR
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
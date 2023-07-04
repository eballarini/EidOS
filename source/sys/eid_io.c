#include <stdio.h>
#include <stdarg.h>
#include <source/ff.h>
#include <devlib/cdev.h>
#include <string.h>

#define STRING_MAX_SIZE 100

typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} BlockLink_t;

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

#ifndef heapBITS_PER_BYTE
#define heapBITS_PER_BYTE		( ( size_t ) 8 )
#endif

static size_t xBlockAllocatedBit=( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );

extern DEV cdev[];

void *pvPortCalloc (size_t num, size_t size)
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

void *pvPortRealloc( void *pv, size_t xWantedSize )
{/*
    BlockLink_t *pxLink;
    void *pvReturn = NULL;
	size_t block_size;
    size_t move_size;
    
    
	if (xWantedSize > 0)
	{ 
            // The memory being freed will have an BlockLink_t structure immediately before it.
			pxLink = (BlockLink_t *)pv-xHeapStructSize;
            block_size=pxLink->xBlockSize & xBlockAllocatedBit;
            
            pvReturn=pvPortMalloc(xWantedSize);
            
            if(pvReturn!=0)
            {
            if(xWantedSize>block_size)
                move_size=block_size;
            else
                move_size=xWantedSize;
            
            if(block_size>0 && pv != NULL )
            {
                memcpy(pvReturn, pv, move_size);
            }
            
            vPortFree(pv);
            }
    }
*/
	// Exit with memory block
	//return pvReturn;
    
    
	// Local variable	
	size_t move_size;
	size_t block_size;
	BlockLink_t *pxLink;
	void *pvReturn = NULL;
	uint8_t *puc = ( uint8_t * ) pv;

	// Se NULL, exit
	if (xWantedSize > 0)
	{
		// Controllo se buffer valido
		if (pv != NULL)
		{
			// The memory being freed will have an BlockLink_t structure immediately before it.
			puc -= xHeapStructSize;

			// This casting is to keep the compiler from issuing warnings.
			pxLink = ( void * ) puc;

			// Check allocate block
			if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0)
			{
				// The block is being returned to the heap - it is no longer allocated.
				block_size = (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;

				// Alloco nuovo spazio di memoria
				pvReturn = pvPortCalloc(1, xWantedSize);

				// Check creation
				if (pvReturn != NULL)
				{
					// Sposta soltanto fino al limite
					if (block_size < xWantedSize)
					{
						// Il nuovo posto disponibile è inferiore
						move_size = block_size;
					}
					else
					{
						// Il nuovo posto disponibile è maggiore
						move_size = xWantedSize;
					}

					// Copio dati nel nuovo spazio di memoria
					memcpy(pvReturn, pv, move_size);

					// Libero vecchio blocco di memoria
					vPortFree(pv);
				}
			}
			else
			{
				// Puntatore nullo, alloca memoria come fosse nuova
				pvReturn = pvPortCalloc(1, xWantedSize);
			}
		}
		else
		{
			// Puntatore nullo, alloca memoria come fosse nuova
			pvReturn = pvPortCalloc(1, xWantedSize);
		}
	}
	else
	{
		// Exit without memory block
		pvReturn = NULL;
	}

	// Exit with memory block
	return pvReturn;
}

	
void *__wrap_malloc (size_t size)
{
    return pvPortMalloc(size);
}

void *__wrap_calloc (size_t num, size_t size)
{
    return pvPortCalloc(num,size);
}

//void *__wrap_realloc (void *, size_t);
void __wrap_free (void * pointer)
{
    vPortFree(pointer);
}

void *__wrap_realloc( void *ptr, size_t new_size )
{
    return pvPortRealloc(ptr, new_size);
}


int __wrap_vsnprintf (char * str, size_t n, const char * format, va_list arg )
{
  uint32_t i=0;

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

    writearray(&cdev[0],string, 100, '\0');
    
    return strlen(string);
    /*return f_puts((const TCHAR*)string, (FIL*)stream);*/
    
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
  
  file=pvPortMalloc(sizeof(FIL));
  
  //select file mode
  
  result=f_open( file, (const TCHAR*)  filename, FA_READ);
  
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
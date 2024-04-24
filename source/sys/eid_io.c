#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <source/ff.h>
#include <devlib/cdev.h>
#include <string.h>

extern DEV cdev[];

typedef enum {PRINT_N_CHAR, PRINT_W_CHAR, PRINT_INT, PRINT_UINT, PRINT_UOCT, PRINT_N_HEX, PRINT_W_HEX, PRINT_FLOAT, PRINT_PTR, PRINT_COUNTER, PRINT_N_STR, PRINT_W_STR}print_type;

typedef enum {PRINT_NORMAL, PRINT_LONG, PRINT_LLONG}print_size;

typedef enum {PRINT_NARROW, PRINT_WIDE}print_case;

typedef enum{PRINT_DECIMAL, PRINT_OCT, PRINT_HEX} print_base;

typedef enum{PRINT_UNSIGNED, PRINT_SIGNED} print_sign;

typedef struct{
    print_type type;
    print_size size;
} print_format;

char print_charmap[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
char print_charmap_wide[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

#define STRING_MAX_SIZE 200

#define MAX_DIGIT 10
#define MAX_DIGIT_LONG 14
#define MAX_DIGIT_LLONG 24

#define SIGN_MASK 0x80000000
#define SIGN_MASK_LONG 0x80000000
#define SIGN_MASK_LLONG 0x8000000000000000

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define PRINT_LEN(type) \
unsigned int print_len_##type ( type num, unsigned int base) \
{ unsigned int count=0; \
do {  num=num/base;count++;}\
while(num!=0);\
return count;\
}

#define PRINT_LEN_LONG(type) \
unsigned int print_len_long_##type ( long type num, unsigned int base) \
{ unsigned int count=0; \
do {  num=num/base;count++;}\
while(num!=0);\
return count;\
}

#define PRINT_LEN_LLONG(type) \
unsigned int print_len_llong_##type ( long long type num, unsigned int base) \
{ unsigned int count=0; \
do {  num=num/base;count++;}\
while(num!=0);\
return count;\
}

#define PRINT_NUM(type)\
void print_num_##type(type num, char * str, unsigned int base, unsigned int length, print_case pcase)\
{\
  type digit;\
  do\
  {\
  digit=num%base;\
  num=num/base;\
  if(pcase==PRINT_NARROW)\
  str[length]=print_charmap[digit];\
  else if (pcase==PRINT_WIDE)\
  str[length]=print_charmap_wide[digit];\
  length--;\
  }while(num!=0);\
}

#define PRINT_NUM_LONG(type)\
void print_num_long_##type(type num, char * str, unsigned int base, unsigned int length, print_case pcase)\
{\
  type digit;\
  do\
  {\
  digit=num%base;\
  num=num/base;\
  if(pcase==PRINT_NARROW)\
  str[length]=print_charmap[digit];\
  else if (pcase==PRINT_WIDE)\
  str[length]=print_charmap_wide[digit];\
  length--;\
  }while(num!=0);\
}

#define PRINT_NUM_LLONG(type)\
void print_num_llong_##type(type num, char * str, unsigned int base, unsigned int length, print_case pcase)\
{\
  type digit;\
  do\
  {\
  digit=num%base;\
  num=num/base;\
  if(pcase==PRINT_NARROW)\
  str[length]=print_charmap[digit];\
  else if (pcase==PRINT_WIDE)\
  str[length]=print_charmap_wide[digit];\
  length--;\
  }while(num!=0);\
}

#define PRINT_NUM_SIGN(type)\
void print_num_sign_##type(type* num, char * str, size_t offset, print_base base)\
{\
  char sign='+';\
   if((*num&SIGN_MASK)==SIGN_MASK)\
   {\
    sign='-';\
    *num=~(*num-1);\
   }\
   else\
    sign='+';\
   str[offset]=sign;\
}

#define PRINT_NUM_SIGN_LONG(type)\
void print_num_sign_long_##type(long type* num, char * str, size_t offset, print_base base)\
{\
  char sign='+';\
   if((*num&SIGN_MASK_LONG)==SIGN_MASK_LONG)\
   {\
    sign='-';\
    *num=~(*num-1);\
   }\
   else\
    sign='+';\
   str[offset]=sign;\
}

#define PRINT_NUM_SIGN_LLONG(type)\
void print_num_sign_llong_##type(long long type* num, char * str, size_t offset, print_base base)\
{\
  char sign='+';\
   if((*num&SIGN_MASK_LLONG)==SIGN_MASK_LLONG)\
   {\
    sign='-';\
    *num=~(*num-1);\
   }\
   else\
    sign='+';\
   str[offset]=sign;\
}

PRINT_NUM_SIGN(int);
PRINT_NUM_SIGN_LONG(int);
PRINT_NUM_SIGN_LLONG(int);

PRINT_NUM_SIGN(unsigned);
PRINT_NUM_SIGN_LONG(unsigned);
PRINT_NUM_SIGN_LLONG(unsigned);

#define print_num_sign(X,Y,Z,W) _Generic((X), \
    int *: print_num_sign_int , \
    long int *: print_num_sign_long_int, \
    long long int *: print_num_sign_llong_int, \
    unsigned *: print_num_sign_unsigned, \
    long unsigned *: print_num_sign_long_unsigned, \
    long long unsigned *: print_num_sign_llong_unsigned, \
    default: print_num_sign_int \
    )(X,Y,Z,W)

PRINT_LEN(int);
PRINT_LEN_LONG(int);
PRINT_LEN_LLONG(int);

PRINT_LEN(unsigned);
PRINT_LEN_LONG(unsigned);
PRINT_LEN_LLONG(unsigned);

PRINT_NUM(int);
PRINT_NUM_LONG(int);
PRINT_NUM_LLONG(int);

PRINT_NUM(unsigned);
PRINT_NUM_LONG(unsigned);
PRINT_NUM_LLONG(unsigned);



#define print_len(X,Y) _Generic((X), \
    int: print_len_int , \
    long int: print_len_long_int, \
    long long int: print_len_llong_int, \
    unsigned: print_len_unsigned, \
    long unsigned: print_len_long_unsigned, \
    long long unsigned: print_len_llong_unsigned, \
    default: print_len_int \
    )(X,Y)

#define print_num(X,Y,Z,W,K) _Generic((X), \
    int: print_num_int , \
    long int: print_num_long_int, \
    long long int: print_num_llong_int, \
    unsigned: print_num_unsigned, \
    long unsigned: print_num_long_unsigned, \
    long long unsigned: print_num_llong_unsigned, \
    default: print_num_int \
    )(X,Y,Z,W,K)
    
    
    
#define PRINT_NUM_TO_STR(type)\
unsigned int print_num_to_str_##type(type num, char * str, size_t offset, size_t max_length, print_case pcase, print_base base, print_sign sign)\
{\
  unsigned int counter=0, ret,p_base;\
  char string[MAX_DIGIT];\
  if (base==PRINT_DECIMAL&&sign==PRINT_SIGNED)\
  {\
   print_num_sign(&num, str, offset, base);\
   offset++;\
  }\
   switch(base)\
   {\
   case PRINT_OCT: p_base=8; break;\
   case PRINT_HEX: p_base=16; break;\
   default: p_base=10; break;\
   }\
   counter=print_len(num, p_base);\
   counter--;\
   ret=counter+offset;\
   print_num(num, string, p_base, counter, pcase);\
   strncpy(str+offset, string, MIN(ret, max_length-offset));\
   return ret;\
}

#define PRINT_NUM_TO_STR_LONG(type)\
unsigned int print_num_to_str_long_##type(long type num, char * str, size_t offset, size_t max_length, print_case pcase, print_base base, print_sign sign)\
{\
  unsigned int counter=0, ret,p_base;\
  char string[MAX_DIGIT_LONG];\
  if (base==PRINT_DECIMAL&&sign==PRINT_SIGNED)\
  {\
   print_num_sign(&num, str, offset, base);\
   offset++;\
  }\
   switch(base)\
   {\
   case PRINT_OCT: p_base=8; break;\
   case PRINT_HEX: p_base=16; break;\
   default: p_base=10; break;\
   }\
   counter=print_len(num, p_base);\
   counter--;\
   ret=counter+offset;\
   print_num(num, string, p_base, counter, pcase);\
   strncpy(str+offset, string, MIN(ret, max_length-offset));\
   return ret;\
}

#define PRINT_NUM_TO_STR_LLONG(type)\
unsigned int print_num_to_str_llong_##type(long long type num, char * str, size_t offset, size_t max_length, print_case pcase, print_base base, print_sign sign)\
{\
  unsigned int counter=0, ret,p_base;\
  char string[MAX_DIGIT_LLONG];\
  if (base==PRINT_DECIMAL&&sign==PRINT_SIGNED)\
  {\
   print_num_sign(&num, str, offset, base);\
   offset++;\
  }\
   switch(base)\
   {\
   case PRINT_OCT: p_base=8; break;\
   case PRINT_HEX: p_base=16; break;\
   default: p_base=10; break;\
   }\
   counter=print_len(num, p_base);\
   counter--;\
   ret=counter+offset;\
   print_num(num, string, p_base, counter, pcase);\
   strncpy(str+offset, string, MIN(ret, max_length-offset));\
   return ret;\
}

PRINT_NUM_TO_STR(int);
PRINT_NUM_TO_STR_LONG(int);
PRINT_NUM_TO_STR_LLONG(int);

PRINT_NUM_TO_STR(unsigned);
PRINT_NUM_TO_STR_LONG(unsigned);
PRINT_NUM_TO_STR_LLONG(unsigned);

#define print_num_to_str(X,Y,Z,W,K,I,J) _Generic((X), \
    int: print_num_to_str_int , \
    long int: print_num_to_str_long_int, \
    long long int: print_num_to_str_llong_int, \
    unsigned: print_num_to_str_unsigned, \
    long unsigned: print_num_to_str_long_unsigned, \
    long long unsigned: print_num_to_str_llong_unsigned, \
    default: print_num_to_str_int \
    )(X,Y,Z,W,K,I,J)
    
print_size print_parse_size(const char * format, size_t offset, size_t counter)
{
  if(counter>offset)
    {
     if(format[counter-1]=='l'&&counter>offset+1)
     {
     	if (format[counter-2]=='l')
     		return PRINT_LLONG;
     	else
     		return PRINT_LONG;
     }
     else if (format[counter-1]=='l')
     	return PRINT_LONG;
     else
     	return PRINT_NORMAL;
    }
  else
    return PRINT_NORMAL;
}

unsigned int parse_format(const char * format, print_format * parsed_format, size_t offset, size_t size)
{
  unsigned int i;
  
  for(i=offset; format[i]!='\0'&&i<size; i++)
  {
      
      switch(format[i])
      {
          case 'c': parsed_format->type=PRINT_N_CHAR; return i; break;
          case 'C': parsed_format->type=PRINT_W_CHAR; return i; break;
          case 'd': parsed_format->type=PRINT_INT; parsed_format->size=print_parse_size(format, offset, i); return i; break;
          case 'i': parsed_format->type=PRINT_INT; return i; break;
          case 'o': parsed_format->type=PRINT_UOCT; return i; break;
          case 'u': parsed_format->type=PRINT_UINT; return i; break;
          case 'x': parsed_format->type=PRINT_N_HEX; return i; break;
          case 'X': parsed_format->type=PRINT_W_HEX; return i; break;
          case 'e': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'E': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'f': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'F': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'g': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'G': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'a': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'A': parsed_format->type=PRINT_FLOAT; return i; break;
          case 'n': parsed_format->type=PRINT_COUNTER; return i; break;
          case 'p': parsed_format->type=PRINT_PTR; return i; break;
          case 's': parsed_format->type=PRINT_N_STR; return i; break;
          case 'S': parsed_format->type=PRINT_W_STR; return i; break;
          default:
            break;
      }
  }
  return offset+size;
}

char char_to_str(char c, char * str, size_t offset, size_t max_length, print_case pcase)
{

 if((offset)<max_length)
   {
    str[offset]=c;
    return offset;
   }
 else
   return offset-1;
}

unsigned int double_to_str(double num, char * str, size_t offset, size_t max_length)
{
  
  char buffer[100];
  
  gcvt(num, 10, buffer);

  strncpy(str+offset, buffer, MIN(strlen(buffer), max_length-offset));
  
  return offset+MIN(strlen(buffer), max_length-offset)-1;
}


unsigned int print_count(void * ptr, char * str, size_t offset, size_t max_length)
{   
  return print_num_to_str((unsigned int) offset,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_UNSIGNED); 
}

unsigned int str_to_str(char * str_in, char * str, size_t offset, size_t max_length, print_case pcase)
{
  
  strncpy(str+offset, str_in, MIN(strlen(str_in), max_length-offset));

  return offset+MIN(strlen(str_in), max_length-offset)-1;
}

/*not taking care of parameters so far, to do*/

unsigned int print_value(char * str, size_t offset, size_t max_length, print_format * parsed_format, va_list arg)
{
	int i_buff=0;
	unsigned u_buff=0;
	long int li_buff=0;
	long unsigned lu_buff=0;
	long long int lli_buff=0;
	long long unsigned llu_buff=0;
	char c, *s;
	double f;
	void * ptr=0;
	unsigned long int ptr_buff;
	
	switch(parsed_format->type)
      	{
      	case PRINT_N_CHAR: c=va_arg(arg,int); return char_to_str(c,str,offset, max_length, PRINT_NARROW); break;
	case PRINT_W_CHAR: c=va_arg(arg,int); return char_to_str(c,str,offset, max_length, PRINT_WIDE); break;
	case PRINT_INT: 
	if(parsed_format->size==PRINT_NORMAL) 
		{
		i_buff=va_arg(arg,int); 
		return print_num_to_str(i_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_SIGNED);
		} 
	else if(parsed_format->size==PRINT_LONG) 
		{
		li_buff=va_arg(arg,long int);
		return print_num_to_str(li_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_SIGNED);
		} 
	else if(parsed_format->size==PRINT_LLONG) 
		{
		lli_buff=va_arg(arg,long long int);
		return print_num_to_str(lli_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_SIGNED);
		} 
	break;
	case PRINT_UOCT: 
	if(parsed_format->size==PRINT_NORMAL) 
		{
		u_buff=va_arg(arg,int); 
		return print_num_to_str(u_buff,str,offset, max_length, PRINT_NARROW, PRINT_UOCT, PRINT_UNSIGNED); 
		}
	else if(parsed_format->size==PRINT_LONG) 
		{
		lu_buff=va_arg(arg,long int); 
		return print_num_to_str(lu_buff,str,offset, max_length, PRINT_NARROW, PRINT_UOCT, PRINT_UNSIGNED); 
		}
	else if(parsed_format->size==PRINT_LLONG) 
		{
		llu_buff=va_arg(arg,long long int); 
		return print_num_to_str(llu_buff,str,offset, max_length, PRINT_NARROW, PRINT_UOCT, PRINT_UNSIGNED); 
		} 
	break;
	case PRINT_UINT: 
	if(parsed_format->size==PRINT_NORMAL) 
		{
		u_buff=va_arg(arg,int); 
		return print_num_to_str(u_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_UNSIGNED); 
		} 
	else if(parsed_format->size==PRINT_LONG) 
		{
		lu_buff=va_arg(arg,long int); 
		return print_num_to_str(lu_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_UNSIGNED); 
		} 
	else if(parsed_format->size==PRINT_LLONG) 
		{
		llu_buff=va_arg(arg,long long int); 
		return print_num_to_str(llu_buff,str,offset, max_length, PRINT_NARROW, PRINT_DECIMAL, PRINT_UNSIGNED); 
		} 
	break;
	case PRINT_N_HEX: 
	if(parsed_format->size==PRINT_NORMAL) 
		{
		u_buff=va_arg(arg,int); 
		return print_num_to_str(u_buff,str,offset, max_length, PRINT_NARROW, PRINT_HEX, PRINT_UNSIGNED); 
		} 
	else if(parsed_format->size==PRINT_LONG) 
		{
		lu_buff=va_arg(arg,long int); 
		return print_num_to_str(lu_buff,str,offset, max_length, PRINT_NARROW, PRINT_HEX, PRINT_UNSIGNED); 
		} 
	else if(parsed_format->size==PRINT_LLONG) 
		{
		llu_buff=va_arg(arg,long long int); 
		return print_num_to_str(llu_buff,str,offset, max_length, PRINT_NARROW, PRINT_HEX, PRINT_UNSIGNED); 
		} 
	break;
	case PRINT_W_HEX: 
	if(parsed_format->size==PRINT_NORMAL) 
		{
		u_buff=va_arg(arg,int); 
		return print_num_to_str(u_buff,str,offset, max_length, PRINT_WIDE, PRINT_HEX, PRINT_UNSIGNED); 
		} 
	else if(parsed_format->size==PRINT_LONG) 
		{
		lu_buff=va_arg(arg,long int);
		return print_num_to_str(lu_buff,str,offset, max_length, PRINT_WIDE, PRINT_HEX, PRINT_UNSIGNED);
		} 
	else if(parsed_format->size==PRINT_LLONG) 
		{
		llu_buff=va_arg(arg,long long int); 
		return print_num_to_str(llu_buff,str,offset, max_length, PRINT_WIDE, PRINT_HEX, PRINT_UNSIGNED); 
		} 
	break;
	case PRINT_FLOAT: f=va_arg(arg,double); return double_to_str(f,str,offset, max_length); break;
	case PRINT_PTR: ptr=va_arg(arg,void*); ptr_buff=(unsigned long int)ptr; return print_num_to_str(ptr_buff,str,offset, max_length, PRINT_NARROW, PRINT_HEX, PRINT_UNSIGNED); break;
	case PRINT_COUNTER: return print_count(ptr,str,offset, max_length); break;
	case PRINT_N_STR: s=va_arg(arg,char*); return str_to_str(s,str,offset, max_length, PRINT_WIDE); break;
	case PRINT_W_STR: s=va_arg(arg,char*); return str_to_str(s,str,offset, max_length, PRINT_WIDE); break;
      	default: break;
      	}

         
        return offset;
}

int __wrap_vsnprintf (char * str, size_t n, const char * format, va_list arg )
{
  unsigned int i=0, j=0, offset;
  print_format parsed_format;
  

  for(i=0; format[i]!='\0'&&i<n&&str!=NULL; i++, j++)
  {

      
      if(format[i]=='%')
      {
         
         offset=i;
      
         i=parse_format(format, &parsed_format, offset+1, n-(i+1));
         
         
         j=print_value(str,j, n, &parsed_format, arg);   
         
      }
          
      else
      {
      str[j]=format[i];
      }
  }
  
  if (n==0)
      str[0]='\0';
  else
      str[j]='\0';
  
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
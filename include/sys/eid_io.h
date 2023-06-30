#ifndef EID_IO
#define EID_IO

#include <stdio.h>

int __wrap_vprintf( const char* format, va_list vlist);

int __wrap_printf( const char *__restrict, ...);

int __wrap_vfprintf(FILE* stream, const char* format, va_list vlist);

int __wrap_fprintf(FILE* stream, const char* format, ...);

FILE* __wrap_fopen (const char* filename, const char* mode);

size_t __wrap_fread(void * buffer, size_t size, size_t count, FILE * stream);

size_t __wrap_fwrite(const void * buffer, size_t size, size_t count, FILE * stream);

int __wrap_fclose(FILE* stream);

int __wrap_fseek(FILE* stream, long offset, int origin);

int __wrap_fputc(int ch, FILE* stream);

//TODO
int __wrap_fputs(const char *str, FILE *stream);

int __wrap_fprintf(FILE* stream, const char* format, ...);

int __wrap_vfprintf( FILE* stream, const char* format, va_list vlist );

int __wrap_fflush(FILE* stream);

long __wrap_ftell(FILE* stream);

#endif
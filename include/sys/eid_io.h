/*
EidOS - a small RTOS for PIC microntrollers
Copyright (C) 2022  Emanuele Ballarini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef EID_IO
#define EID_IO

#include <stdio.h>

int __wrap_vsnprintf (char * str, size_t n, const char * format, va_list arg );

int __wrap_vsprintf(char *str, const char *format, va_list arg);

int __wrap_vprintf( const char* format, va_list vlist);

int __wrap_snprintf (char * str, size_t n, const char * format, ...);

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

//to be incremented
#define printf(X, ...) __wrap_printf((X), ##__VA_ARGS__)


#endif

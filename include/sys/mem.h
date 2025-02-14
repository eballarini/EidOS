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

#ifndef SYS_MEM
#define SYS_MEM

void *pvPortCalloc (size_t num, size_t size);

void *pvPortRealloc( void *pv, size_t xWantedSize );

void *__wrap_malloc (size_t size);

void *__wrap_calloc (size_t num, size_t size);

void __wrap_free (void * pointer);

void *__wrap_realloc( void *ptr, size_t new_size );

#endif


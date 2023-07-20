#ifndef SYS_MEM
#define SYS_MEM

void *pvPortCalloc (size_t num, size_t size);

void *pvPortRealloc( void *pv, size_t xWantedSize );

void *__wrap_malloc (size_t size);

void *__wrap_calloc (size_t num, size_t size);

void __wrap_free (void * pointer);

void *__wrap_realloc( void *ptr, size_t new_size );

#endif



#include <sys/eid_io.h>
//int printf( const char *format, ...);

#define printf(X, ...) __wrap_printf(X,__VA_ARGS__)

void foo(int x) {
  printf("Hello %d!\n", x);
    
  //writearray(mjsdev, "\r\n mjs script executed! \r\n\0" , 100, '\0');
}
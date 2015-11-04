#include <stddef.h>
#include <cstdio>
#include <cstring>

#include "helpers.h"

using namespace std;

void print_hex(char * c, size_t ct){
    for(size_t i=0; i<ct; i++) {
        printf("%02x", (int) c[i]);
        printf("%c", c[i]);
    }
}

#if IS_BIG_ENDIAN
void fix_endianness(void * location, size_t bytes, size_t step) {},
#else
void fix_endianness(void * location, size_t bytes, size_t step){
    char * l = (char *) location;
    for (size_t b=0; b<bytes; b+=step) {
        // switch the endianness on step
        for(size_t o=0; o<step/2; o++) {
            char tmp = l[b+o];
            l[b+o] = l[b+step-1-o];
            l[b+step-1-o] = tmp;
        }
    }
}
#endif




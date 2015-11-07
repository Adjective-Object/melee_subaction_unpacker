#include <stddef.h>
#include <cstdio>
#include <cstring>

#include "helpers.h"
#include "config.h"
#include <iostream>
#include <iomanip>

using namespace std;


void cout_hex(int indent, unsigned char*c, size_t rowsize, size_t rows) {
    string ind = string(indent * INDENT_SIZE, ' ');
    for (unsigned int i=0; i<rows; i++) {
        cout << ind;
        for (unsigned int j=0; j<rowsize; j++) {
            cout << hex << setfill('0') << setw(2) 
                << +c[i*rowsize + j] << " ";
        }
    cout << endl;
    }
}

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




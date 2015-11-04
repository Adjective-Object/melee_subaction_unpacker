#ifndef HELPERS_MREADER
#define HELPERS_MREADER

void print_hex(char * c, size_t ct);
void fix_endianness(void * location, size_t bytes, size_t step);
#endif

#ifndef HELPERS_MREADER
#define HELPERS_MREADER

#include <string>

extern void * MMAP_ORIGIN;

void print_hex(char *c, size_t ct);
void cout_hex(int indent, unsigned char *c, size_t rowsize, size_t rows);
void fix_endianness(void *location, size_t bytes, size_t step = 0);
size_t get_available_size(void *location, size_t cap);

bool hasEnding (std::string const &fullString, std::string const &ending);
std::string to_string_hex(int n);

#define RESET "\033[0m"
#define REVERSE "\033[7m"
#define RESETREVERSE "\033[27m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#endif

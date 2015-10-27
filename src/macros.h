#ifndef MREADER_MACROS
#define MREADER_MACROS
#include <endian.h>

// define the byte order swap macro endiswp s.t. 
// endiswap(bigarg, littlearg) will evaluate the right expression
// based on host endianness (big/little endian)
#if __BYTE_ORDER == __BIG_ENDIAN
    #define endiswp(bigarg, littlearg) (bigarg)
    #define IS_BIG_ENDIAN 1
#else
    #define endiswp(bigarg, littlearg) (littlearg)
    #define IS_BIG_ENDIAN 0
#endif

#endif

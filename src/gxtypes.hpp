#ifndef MREADER_GX
#define MREADER_GX

#include "stdint.h"

enum GXCompType : uint32_t
{
    GX_U8  = 0, // 0 - 255
    GX_S8  = 1, // -127 - +127
    GX_U16 = 2, // 0 - 65535
    GX_S16 = 3, // -32767 - +32767
    GX_F32 = 4, // floating point
};

const char * gx_name(GXCompType type);

#endif

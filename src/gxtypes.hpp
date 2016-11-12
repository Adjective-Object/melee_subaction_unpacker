#ifndef MREADER_GX
#define MREADER_GX

#include "stdint.h"

enum GXCompType : uint32_t
{
    GX_U8  = 0b100, // 0 - 255
    GX_S8  = 0b011, // -127 - +127
    GX_U16 = 0b010, // 0 - 65535
    GX_S16 = 0x001, // -32767 - +32767
    GX_F32 = 0x000, // floating point
};

const char * gx_name(GXCompType type);

#endif

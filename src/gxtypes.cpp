#include "gxtypes.hpp"

const char * gx_name(GXCompType type) {
    switch(type) {
        case GX_U8:     return "uint8_t";
        case GX_S8:     return "int8_t";
        case GX_U16:    return "uint16_t";
        case GX_S16:    return "int16_t";
        case GX_F32:    return "float";
        default:        return "?";
    }
}

#ifndef MREADER_ANIM_TRACK
#define MREADER_ANIM_TRACK

#include <stdint.h>
#include "dolfs/dolfs.hpp"
#include "gxtypes.hpp"
#include <assimp/anim.h>

typedef enum animation_track_type {
    X_ROTATION = 1,
    Y_ROTATION = 2,
    Z_ROTATION = 3,
    X_TRANSLATION = 5,
    Y_TRANSLATION = 6,
    Z_TRANSLATION = 7,
    X_SCALE = 8,
    Y_SCALE = 9,
    Z_SCALE = 10
} animation_track_type;

typedef struct track_header {
    uint16_t length;
    uint16_t unknown_padding;
    uint8_t  track_type;
    uint8_t  value_format;
    uint8_t  tan_format;
    uint8_t  unknown_padding_2;
    uint32_t animdataOffset;
} track_header;

typedef enum tangent_format {

} tangent_format;


typedef enum interpolation_format {
    UNMANAGED = 0,
    // this is just a theory but I think
    // 0 means that this bone is managed by the physics system.
    // G&W's idle anim doesn't use it at all.

    STEP = 1,
    LINEAR = 2,
    HERMITE_VALUE = 3,
    HERMITE_VALUE_AND_TANGENT = 4,
    HERMITE_TANGENT = 5,
    CONSTANT= 6
} animation_track_value_format;


typedef struct track_command {
   int command : 3;   
} track_command;

class TrackHeader : public DataProxy {
    const DatFile * datfile;
    
    DatInspector * headerInspector;
    DatInspector * bodyInspector;

public:
    track_header * animhead;
    TrackHeader(const DatFile * datfile, track_header * animhead);
    void informNextOffset(size_t nextOffset);
    void print(int indent = 0);
    void serialize();
    void writeTrack(aiNodeAnim * newAnim, size_t maxFrames);

    uint8_t* print_anim_data(int indent, uint8_t * data, uint8_t val, uint8_t tan);
};



void * print_gx_frames(int indent, void* data, void * origin,
        uint8_t interpolation_type,
        size_t numFrames, 
        GXCompType data_type, int data_scale,
        GXCompType tan_type, int tan_scale );
uint8_t * print_gx(uint8_t * data, GXCompType type, int scale);
uint8_t * read_varlen_int(uint8_t * head, unsigned int * out);

typedef struct GXConfig {
    GXCompType val_type;
    uint8_t val_scale;

    GXCompType tan_type;
    uint8_t tan_scale;
} GXConfig;

typedef struct keyframe_head {
    uint8_t interpolation_type;
    long int numFrames;
} keyframe_head;

#endif


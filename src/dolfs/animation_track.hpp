#ifndef MREADER_ANIM_TRACK
#define MREADER_ANIM_TRACK

#include <stdint.h>
#include "dolfs/dolfs.hpp"
#include "gxtypes.hpp"

typedef struct track_header {
    uint16_t length;
    uint16_t unknown_padding;
    uint8_t  track_type;
    uint8_t  value_format;
    uint8_t  tan_format;
    uint8_t  unknown_padding_2;
    uint32_t animdataOffset;
} track_header;

typedef struct track_command {
   int command : 3;
   
} track_command;

class TrackHeader : public DataProxy {
    const DatFile * datfile;
    
    DatInspector * targetInspector;
    uint8_t* print_anim_data(int indent, uint8_t * data, uint8_t val, uint8_t tan);

public:
    track_header * animhead;
    TrackHeader(const DatFile * datfile, track_header * animhead);
    void informNextOffset(size_t nextOffset);
    void print(int indent = 0);
    void serialize();
};



void * print_gx_frames(int indent, void* data, 
        uint8_t interpolation_type,
        size_t numFrames, 
        GXCompType data_type, int data_scale,
        GXCompType tan_type, int tan_scale );
char * print_gx(char * data, GXCompType type, int scale);
char * read_varlen_int(char * head, int * out);


#endif


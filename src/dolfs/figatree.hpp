#ifndef MREADER_FIGATREE_HEADER
#define MREADER_FIGATREE_HEADER

#include "dolfs/dolfs.hpp"
#include "dolfs/animation_track.hpp"
#include "gxtypes.hpp"


class DataProxy;
class TrackCtTable;
class TrackHeader;


typedef struct figatree_header {
    uint32_t unknown0x0; // == 1 usually ?
    uint32_t unknown0x4; // == 0 usually ?
    float num_frames; // +1
    uint32_t boneTableOffset;
    uint32_t animDataOffset;
} figatree_header;

class FigaTree : public DataProxy {
    figatree_header * fig;
    const DatFile * datfile;

    TrackCtTable * trackCtTable;
    TrackHeader ** animDatas;
public:
    FigaTree(const DatFile * datfile, figatree_header * fig);
    void print(int indent = 0);
    void serialize();
};





class TrackCtTable : public DataProxy {
    const DatFile * datfile;
public:
    unsigned char * head;
    size_t length;
    size_t numTracks;
    TrackCtTable(const DatFile * datfile, unsigned char* head);
    void print(int indent = 0);
    void serialize();
};


#endif


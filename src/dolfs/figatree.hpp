#ifndef MREADER_FIGATREE_HEADER
#define MREADER_FIGATREE_HEADER

#include "dolfs/dolfs.hpp"


class DataProxy;
class BoneIndexTable;
class AnimDataHeader;


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

    BoneIndexTable * boneIndexTable;
    AnimDataHeader ** animDatas;
public:
    FigaTree(const DatFile * datfile, figatree_header * fig);
    void print(int indent = 0);
    void serialize();
};





class BoneIndexTable : public DataProxy {
    const DatFile * datfile;
public:
    unsigned char * head;
    size_t length;
    BoneIndexTable(const DatFile * datfile, unsigned char* head);
    void print(int indent = 0);
    void serialize();
};




typedef struct animdata_header {
    uint16_t length;
    uint16_t unknown_padding;
    uint32_t unknown_flags;
    uint32_t animdataOffset;
} animdata_header;

class AnimDataHeader : public DataProxy {
    const DatFile * datfile;
    
    DatInspector * targetInspector;
public:
    animdata_header * animhead;
    unsigned char boneflag;


    AnimDataHeader(const DatFile * datfile, animdata_header * animhead, unsigned char boneflag);
    void informNextOffset(size_t nextOffset);
    void print(int indent = 0);
    void serialize();
};

#endif


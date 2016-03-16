#ifndef MREADER_FIGATREE_HEADER
#define MREADER_FIGATREE_HEADER

#include "dolfs/dolfs.hpp"

typedef struct figatree_header {
    uint32_t unknown0x0; // == 1 usually ?
    uint32_t unknown0x4; // == 0 usually ?
    float num_frames; // +1
    uint32_t a[31]; // 31
} figatree_header;

class FigaTree : public DataProxy {
    figatree_header * fig;
    const DatFile * datfile;
public:
    FigaTree(const DatFile * datfile, figatree_header * fig);
    void print(int indent = 0);
    void serialize();
};

#endif


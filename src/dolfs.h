#ifndef DOLFS_MREADER
#define DOLFS_MREADER
#include <cstdint>

typedef struct dol_header {
    uint32_t fileSize; // size of main data block
    uint32_t dataBlockSize;
    uint32_t relocationTableCount;
    uint32_t rootCountA;
    // 0x10
    uint32_t rootCountB;
    uint32_t unknown0x14; // '001B' in main Pl*.dat files
    uint32_t unknown0x18;
    uint32_t unknown0x1C;
    // 0x20
} __attribute__((packed)) dol_header;

typedef struct root_node {
    uint32_t dataSectionOffset;
    uint32_t stringTableOffset;
} __attribute__((packed)) root_node;

// fix the endianness on a dolfile to match the host endianness
void dolfile_init(dol_header * datfile);
void print_dolfile_index(dol_header * datfile);

#endif

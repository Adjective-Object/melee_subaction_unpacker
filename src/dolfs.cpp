#include "dolfs.h"
#include "macros.h"
#include <stddef.h>
#include <iostream>

using namespace std;

void print_hex(char * c, size_t ct){
    for(size_t i=0; i<ct; i++) {
        printf("%02x", (int) c[i]);
        printf("%c", c[i]);
    }
}

#if IS_BIG_ENDIAN
void fix_endianness(void * location, size_t bytes, size_t step) {},
#else
void fix_endianness(void * location, size_t bytes, size_t step){
    char * l = (char *) location;
    for (size_t b=0; b<bytes; b+=step) {
        // switch the endianness on step
        for(size_t o=0; o<step/2; o++) {
            char tmp = l[b+o];
            l[b+o] = l[b+step-1-o];
            l[b+step-1-o] = tmp;
        }
    }
}
#endif


static char * dataBlock(dol_header * datafile) {
    return (char*) (datafile + 1);
}

static uint32_t * relocationTable(dol_header * datfile) {
    return (uint32_t *) (
            dataBlock(datfile) + datfile->dataBlockSize);
}

static root_node * rootList(dol_header *datfile) {
    return (root_node*) (
            relocationTable(datfile) + datfile->relocationTableCount);
}

static char * stringTable(dol_header * datfile) {
    return (char*) (rootList(datfile) + 
                datfile->rootCountA + 
                datfile->rootCountB);
}




void dolfile_init(dol_header * datfile) {
    // fix the endianness on the header
    fix_endianness(datfile, sizeof(dol_header), sizeof(uint32_t));
    cout << "size of data blocK:" << datfile->dataBlockSize << endl;
    cout << "root count a: " << datfile -> rootCountA << endl;
    cout << "root_count b: " << datfile -> rootCountB << endl;

    // fix endianness on root nodes and relocation table
    uint32_t * rt = relocationTable(datfile);
    root_node * rl = rootList(datfile);
    fix_endianness(rt, sizeof(uint32_t) * datfile->relocationTableCount, 4);
    fix_endianness(rl, 
        sizeof(root_node) * (datfile->rootCountA + datfile->rootCountB), 4);

    // apply the relocation table
    // uint32_t * datfile_addr = (uint32_t *) datfile;
    // for (unsigned int i=0; i<datfile->relocationTableCount; i++) {
    //     datfile_addr[rt[i] / sizeof(uint32_t)] = 
    //         datfile_addr[rt[i] / sizeof(uint32_t)] + (intptr_t)datfile_addr;
    // }
}

void print_dolfile_index(dol_header * datfile) {
    root_node * root_list = rootList(datfile);
    char * stringtable = stringTable(datfile);

    cout << "addr of root list: " << root_list << endl;

    unsigned int root_ct = datfile->rootCountA + datfile->rootCountB;
    cout << "total root count: " << root_ct << endl;
    unsigned int i;
    for (i=0; i<root_ct; i++) {
        unsigned int name_offset = root_list[i].stringTableOffset;
        cout << i << ": " << stringtable + name_offset << endl;
    }
}

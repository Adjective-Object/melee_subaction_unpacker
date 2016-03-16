#ifndef DOLFS_MREADER_DOBJ
#define DOLFS_MREADER_DOBJ

#include <cstdint>
#include <vector>
#include <map>

class DObj;

/**
 * Game objects (DObjs)
 **/
typedef struct gameobject_header {
    uint32_t unknown0x0;
    uint32_t nextOffset;
    uint32_t materialOffset;
    uint32_t modelOffset;
} gameobject_header;

class DObj : public DataProxy {
    const DatFile * datfile;
    gameobject_header * gameobject;
    AnonymousData * material;
    AnonymousData * model;
public:
    vector<DObj *> peers;

    DObj(DatFile const * datheader, gameobject_header * gameobj);
    void print(int indent = 0);
    virtual void serialize();
};


#endif


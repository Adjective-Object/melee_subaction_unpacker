#ifndef DOLFS_MREADER_JOBJ
#define DOLFS_MREADER_JOBJ

#include <cstdint>
#include <vector>
#include <map>

#include "dolfs/dobj.hpp"

using namespace std;

class JObj;

/**
 * Joint Object
 **/
typedef struct jointdata_header {
    uint32_t unknown0x0;
    uint32_t flags;
    uint32_t childOffset;
    uint32_t nextPeerOffset;
    uint32_t dobjOffset;

    float rotationZ;
    float rotationY;
    float rotationX;
    float scaleZ;
    float scaleY;
    float scaleX;
    float translationZ;
    float translationY;
    float translationX;
    uint32_t inverseTransOff;

    uint32_t unknown0x3C;
} __attribute__((packed)) jointdata_header;


class JObj : public DataProxy {
    const DatFile * datfile;
    jointdata_header * jobj;

    intptr_t offset;

    vector<JObj *> peers;
    vector<JObj *> children;
    vector<DObj *> associatedObjects;

    void _serialize_bvh_structure(ofstream & fout, int indent);
    void _serialize_bvh_parameters(ofstream & fout);

public:
    JObj(DatFile const *datheader, jointdata_header * jobj);
    // Destructors not handled yet
    //~JObj();
    void print(int indent = 0);

    /**
     * Serializes the joint object to a BDF
     **/
    virtual void serialize();
};


#endif


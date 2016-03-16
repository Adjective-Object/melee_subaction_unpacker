#include "macros.hpp"
#include "event_mapper.hpp"
#include <iostream>
#include <iomanip>
#include <map>
#include <cstring>

#include "helpers.hpp"
#include "config.hpp"

#include "dolfs/dolfs.hpp"
#include "dolfs/dobj.hpp"

using namespace std;

DObj::DObj(DatFile const * datheader, gameobject_header * gameobj) {
    this->datfile = datheader;
    this->gameobject = gameobj;
    this->peers = vector<DObj *>();

    fix_endianness(gameobj, sizeof(gameobj), sizeof(uint32_t));
    this->model = new AnonymousData(
        datfile->dataSection + gameobj->modelOffset);
    this->material = new AnonymousData(
        datfile->dataSection + gameobj->materialOffset);

    if (gameobj->nextOffset != 0) {
        DObj * nextPeer = new DObj(datfile, (gameobject_header *)
                (datfile->dataSection + gameobj->nextOffset));
        this->peers.push_back(nextPeer);
        this->peers.insert(
                peers.end(),
                nextPeer->peers.begin(),
                nextPeer->peers.end());
    }
}

void DObj::print(int indent) {
    // record formatting
    ios::fmtflags f(cout.flags());
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << "material: "
         << setw(10) << this->material
         << "      "
         << "model"
         << setw(10) << this->model 
         << endl;

    // reset formatting
    cout.flags(f);
}

void DObj::serialize() {
}



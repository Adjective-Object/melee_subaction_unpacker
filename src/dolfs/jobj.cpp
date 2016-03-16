#include "macros.hpp"
#include "event_mapper.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <cstring>

#include "helpers.hpp"
#include "config.hpp"

#include "dolfs/dolfs.hpp"
#include "dolfs/ftdata.hpp"
#include "dolfs/jobj.hpp"
#include "dolfs/dobj.hpp"

using namespace std;

JObj::JObj(DatFile const * datfile, jointdata_header * jobj) {
    this->datfile = datfile;
    this->jobj = jobj;

    this->peers = vector<JObj *>();
    this->children = vector<JObj *>();
    this->associatedObjects = vector<DObj *>();

    this->offset = (intptr_t)(((char *)this->jobj) -
                     ((char *)this->datfile->dataSection));

    // size of float and uint32_t are 4, so this is safe
    fix_endianness(jobj, sizeof(jointdata_header), 4);

    // fix_endianness(&(jobj->flags), sizeof(uint32_t) * 4, 4);
    // fix_endianness(&(jobj->inverseTransOff),
    //                 sizeof(uint32_t), 4);

    /**
    cout << "----------" << endl;
    cout << "thisOffset: " << 
        (char *) jobj - (char *) datfile->dataSection << endl;
    cout << "childOffset: " << jobj->childOffset << endl;
    cout << "nextPeerOffset: " << jobj->nextPeerOffset << endl;
    **/

    if (jobj->childOffset != 0) {
        JObj * child = new JObj(datfile, (jointdata_header *) 
                (datfile->dataSection + jobj->childOffset));
        // add all of child's peers to self
        this->children.push_back(child);
        this->children.insert(
                children.end(),
                child->peers.begin(),
                child->peers.end());
    }

    if (jobj->nextPeerOffset != 0) {
        JObj * nextPeer = new JObj(datfile, (jointdata_header *)
                (datfile->dataSection + jobj->nextPeerOffset));
        this->peers.push_back(nextPeer);
        this->peers.insert(
                peers.end(),
                nextPeer->peers.begin(),
                nextPeer->peers.end());
    }

    if (jobj->dobjOffset != 0) {
        DObj * associatedObj = new DObj(datfile, (gameobject_header *)
                (datfile->dataSection + jobj->dobjOffset));
        this->associatedObjects.push_back(associatedObj);
        this->associatedObjects.insert(
                this->associatedObjects.end(),
                associatedObj->peers.begin(),
                associatedObj->peers.end());
    }
}

/* Destructors not handled yet
JObj::~JObj() {
    // call the destructor of the first peer and the first
    // child
    if (this->peers.size() > 0) {
        delete this->peers[0];
    }

    if (this->children.size() > 0) {
        delete this->children[0];
    }
}
*/

void JObj::print(int indent) {
    // record formatting
    ios::fmtflags f(cout.flags());
    
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << "jobj offset: " << hex
       << this->offset<< endl;

    cout << ind << BLUE << "unknown0x0:      "      << RESET 
         << setw(15)    << this->jobj->unknown0x0   << endl;
    cout << ind << BLUE << "flags:           "      << RESET
         << setw(15) << this->jobj->flags           <<  endl;
    cout << ind << BLUE << "childOffset:     "      << RESET 
         << setw(15) << this->jobj->childOffset     <<  endl;
    cout << ind << BLUE << "nextPeerOffset:  "      << RESET 
         << setw(15) << this->jobj->nextPeerOffset  <<  endl;
    cout << ind << BLUE << "dobjOffset:      "      << RESET 
         << setw(15) << this->jobj->dobjOffset      <<  endl;
    cout << ind << BLUE << "rotationZ:        "      << RESET 
         << setw(15) << this->jobj->rotationZ        <<  endl;
    cout << ind << BLUE << "rotationY:        "      << RESET 
         << setw(15) << this->jobj->rotationY        <<  endl;
    cout << ind << BLUE << "rotationX:        "      << RESET 
         << setw(15) << this->jobj->rotationX        <<  endl;
    cout << ind << BLUE << "scaleX:          "      << RESET 
         << setw(15) << this->jobj->scaleX          <<  endl;
    cout << ind << BLUE << "scaleY:          "      << RESET 
         << setw(15) << this->jobj->scaleY          <<  endl;
    cout << ind << BLUE << "scaleZ:          "      << RESET 
         << setw(15) << this->jobj->scaleZ          <<  endl;
    cout << ind << BLUE << "translationX:    "      << RESET 
         << setw(15) << this->jobj->translationX    <<  endl;
    cout << ind << BLUE << "translationY:    "      << RESET 
         << setw(15) << this->jobj->translationY    <<  endl;
    cout << ind << BLUE << "translationZ:    "      << RESET 
         << setw(15) << this->jobj->translationZ    <<  endl;
    cout << ind << BLUE << "inverseTransOff: "      << RESET 
         << setw(15) << this->jobj->inverseTransOff <<  endl;
    cout << ind << BLUE << "unknown0x3C: "          << RESET 
         << setw(15) << this->jobj->unknown0x3C     <<  endl;
   
    cout << ind << BLUE << "--------- objects ---------" << RESET << endl;
    for(DObj* d : this->associatedObjects) {
        d->print(indent + 1);
    }
    cout << ind << BLUE << "---------------------------" << RESET << endl;

    for(JObj* j : this->children) {
        j->print(indent + 1);
    }

    // reset formatting
    cout.flags(f);
}


void JObj::serialize() {
    ofstream fout;
    fout.open(JOINT_OUTPUT_PATH);
    fout.precision(6);
    fout << fixed;
    fout << "HIERARCHY" << endl 
         << "ROOT ";
    this->_serialize_bvh_structure(fout, 0);

    fout << "MOTION " << endl
         << "Frames: 2" << endl
         << "Frame Time: 1" << endl;
    this->_serialize_bvh_parameters(fout);
    fout << endl;
    this->_serialize_bvh_parameters(fout);
    fout.close();
}

void JObj::_serialize_bvh_structure(ofstream & fout, int indent) {

    string ind(indent, '\t');
    fout << "joint_" << this->offset << endl;
    fout << ind << "{" << endl;
    fout << ind << '\t' << "OFFSET " << this->jobj->translationX << " " 
                        << this->jobj->translationY << " " 
                        << this->jobj->translationZ << " " << endl;
    fout << ind << '\t' << "CHANNELS 3 "
         << "Zrotation Xrotation Yrotation "
         // << "Zscale Xscale Yscale"
         << endl;
    if (this->children.size() != 0) {
        for (JObj *j : this->children) {
            fout << ind << '\t' << "JOINT ";
            j->_serialize_bvh_structure(fout, indent + 1);
        }
    }
    else {
        fout << ind << '\t' << "End Site" << endl;
        fout << ind << '\t' << "{" <<endl;

        fout << ind << "\t\t" << "OFFSET " 
             << "0.0000 0.0000 0.0000" <<endl;

        fout << ind << '\t' << "}" <<endl;
    }
    fout << endl << ind << "}" << endl;

}


void JObj::_serialize_bvh_parameters(ofstream & fout) {
    fout << this->jobj->rotationZ << " "
         << this->jobj->rotationX << " "
         << this->jobj->rotationY << " "
         /*
         << this->jobj->scaleZ << " "
         << this->jobj->scaleX << " "
         << this->jobj->scaleY << " "
         */
        ;
     for (JObj *j : this->children) {
        j->_serialize_bvh_parameters(fout);
    }
}

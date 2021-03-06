#include "macros.hpp"
#include "event_mapper.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <map>
#include <cmath>
#include <bitset>
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

    if (jobj->childOffset != 0) {
        // cout << "## entering child of " << this->offset << endl;
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
        // cout << "## entering peer of " << this->offset << endl;
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

    serialize_state st = {
        false, false, false
    };

    this->_serialize_bvh_structure(fout, 0, st);

    fout << "MOTION " << endl
         << "Frames: 100" << endl
         << "Frame Time: 1" << endl;

    this->_serialize_bvh_parameters(fout, 360.0 / 3.14159 / 2);
    fout << endl;
}

int JObj::_serialize_bvh_structure(
        ofstream & fout, 
        int indent, serialize_state st) {

    string ind(indent, '\t');
    fout << "joint_" << hex << this->offset << endl;
    fout << ind << "{" << endl;
    fout << ind << '\t' << "OFFSET ";

    cout << hex << this->offset << ": " 
         << dec << bitset<32>(this->jobj->flags).to_string() << endl;
   
    float   tX = this->jobj->translationX,
            tY = this->jobj->translationY,
            tZ = this->jobj->translationZ;

    //
    // TODO process flags
    //
   
    if (this->jobj->flags & 0x20000) {
        cout << RED << "JOBJ_USE_QUATERNION" << RESET << endl;
    } else {
        // joint uses Euler angles, not Quaternions
    }

    if (((this->jobj->flags >> 3) & 1) == 1 && 
        ((this->jobj->flags >> 0) & 1) == 0) {
        cout << BLUE << "inverting" << RESET << endl;
        st.x = ! st.x;
        st.y = ! st.y;
    }

    fout << (st.x ? -1 : 1) * tX << " " 
         << (st.y ? -1 : 1) * tY << " " 
         << (st.z ? -1 : 1) * tZ << " " << endl;

    fout << ind << '\t' << "CHANNELS 3 "
         << "Zrotation Yrotation Xrotation "
         // << "Zscale Xscale Yscale"
         << endl;

    int sum_child = 1;
    if (this->children.size() != 0) {
        for (JObj *j : this->children) {
            fout << ind << '\t' << "JOINT ";
            sum_child += j->_serialize_bvh_structure(
                    fout, indent + 1, st);
        }
    }
    else {
        // check that end nodes all have a rotation of 0
        /*
        if (abs(j->jobj->rotationZ) > 0.00001 ||
            abs(j->jobj->rotationY) > 0.00001 ||
            abs(j->jobj->rotationX) > 0.00001) {
            cout << "expected end node to have rotation 0" << endl;
            cout << "end node has rotation " 
                 << " Z=" << j->jobj->rotationZ
                 << " Y=" << j->jobj->rotationY
                 << " X=" << j->jobj->rotationX
                 << endl;
            exit(1);
        }
        */

        fout << ind << '\t' << "End Site" << endl;
        fout << ind << '\t' << "{" <<endl;

        fout << ind << "\t\t" << "OFFSET " 
             << "0 0 0 "
            /*
             << j->jobj->translationX << " " 
             << j->jobj->translationY << " " 
             << j->jobj->translationZ << " " 
             */
             << endl;

        fout << ind << '\t' << "}" <<endl;
    }

    fout << endl << ind << "}" << endl;

    return sum_child;
}


int JObj::_serialize_bvh_parameters(ofstream & fout, 
        float scaling_factor) {
    float rotZ = this->jobj->rotationZ * scaling_factor, 
          rotY = this->jobj->rotationY * scaling_factor,
          rotX = this->jobj->rotationX * scaling_factor;

    //
    // process flags
    //
    
    fout << rotZ << " "
         << rotX << " "
         << rotY << " ";

    int sum_child = 1;
    for (JObj *j : this->children) {
        sum_child += j->_serialize_bvh_parameters(
                fout, scaling_factor);
    }

    return sum_child;
}

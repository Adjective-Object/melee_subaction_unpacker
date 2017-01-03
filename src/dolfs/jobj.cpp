#include "macros.hpp"
#include "event_mapper.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>
#include <cmath>
#include <bitset>
#include <cstring>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Exporter.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

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


void JObj::serialize(aiScene *scene) {
//    ofstream fout;
//    fout.open(JOINT_OUTPUT_PATH);
//    fout.precision(6);
//    fout << fixed;
//    fout << "HIERARCHY" << endl 
//         << "ROOT ";
//
//    serialize_state st = {
//        false, false, false
//    };
//
//    this->_serialize_bvh_structure(fout, 0, st);
//
//    fout << "MOTION " << endl
//         << "Frames: 100" << endl
//         << "Frame Time: 1" << endl;
//
//    this->_serialize_bvh_parameters(fout, 360.0 / 3.14159 / 2);
//    fout << endl;

    // make a mesh
    aiMesh * mesh = new aiMesh();
    mesh->mBones = new aiBone*[this->totalNumBones()];
    mesh->mNumBones = 0;

    // add the mesh to the scene
    scene->mMeshes = new aiMesh*[ 1 ];
    scene->mMeshes[0] = mesh;
    scene->mNumMeshes = 1;

    // tell the root node it holds the global mesh '0'
    scene->mRootNode->mMeshes = new unsigned int[ 1 ];
    scene->mRootNode->mMeshes[ 0 ] = 0;
    scene->mRootNode->mNumMeshes = 1;

    // populate bones list of the mesh
    this->_serialize_bones_to_mesh(*mesh, *(scene->mRootNode));

}

size_t JObj::totalNumBones() {
    size_t boneCt = 1;

    for (JObj * j: this->children) {
        boneCt += j->totalNumBones();
    }

    return boneCt;
}

void JObj::_serialize_bones_to_mesh(aiMesh & mesh, aiNode & parentNode) {

    // create the bone and put it on the mesh bone table
    aiBone * bone = new aiBone();
    mesh.mBones[mesh.mNumBones] = bone;
    mesh.mNumBones++;

    // attach a new node for the bone in the hierarchy
    aiNode * node = new aiNode();
    if (this->children.size() > 0) {
        node->mChildren = new aiNode*[this->children.size()];
    }

    node->mNumChildren = 0;
    parentNode.mChildren[parentNode.mNumChildren] = node;
    parentNode.mNumChildren++;

    // create transform for the node
    aiVector3t<float> translate = aiVector3t<float>(
            this->jobj->translationX,
            this->jobj->translationY,
            this->jobj->translationZ);
    aiMatrix4x4::Translation(translate, node->mTransformation);
    aiMatrix4x4 tmp;

    // process flags to do additional transformations
    // cout << std::bitset<32>(this->jobj->unknown0x0) << endl;
    // cout << std::bitset<32>(this->jobj->unknown0x3C) << endl;
    cout << this->offset << " "
         << std::bitset<32>(this->jobj->flags) << endl;
    if (((this->jobj->flags >> 3) & 1) == 1 && 
        ((this->jobj->flags >> 0) & 1) == 0) {
        cout << BLUE << "inverting" << RESET << endl;
        
        aiMatrix4x4::RotationX(90.0f / 360.0f * 3.14159 * 180, tmp);
        // node->mTransformation *= tmp;

        translate = aiVector3t<float>(1, -1, 1);
        aiMatrix4x4::Scaling(translate, tmp);
        node->mTransformation *= tmp;
    }

    if (((this->jobj->flags >> 2) & 1) == 1) {
        translate = aiVector3t<float>(-1, -1, 1);
        aiMatrix4x4::Scaling(translate, tmp);
        node->mTransformation *= tmp;
    }

    aiMatrix4x4::RotationY((float) this->jobj->rotationY, tmp);
    node->mTransformation *= tmp;
    aiMatrix4x4::RotationX((float) this->jobj->rotationX, tmp);
    node->mTransformation *= tmp;
    aiMatrix4x4::RotationZ((float) this->jobj->rotationZ, tmp);
    node->mTransformation *= tmp;

    // generate a unique name using the pointer to the bone
    // and attach it to the child nodes in the scene
    std::stringstream ss;
    ss << "joint_" << this->offset;
    std::string name = ss.str(); 

    // assign the same names to the node and the bone
    node->mName = aiString(name);
    bone->mName = aiString(name);


    // attach each child
    if (this->children.size() != 0) {
        for (JObj *j : this->children) {
            j->_serialize_bones_to_mesh(mesh, *node);
        }
    }
}


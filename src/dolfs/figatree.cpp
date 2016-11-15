#include <iostream>
#include <iomanip>
#include <bitset>

#include "helpers.hpp"
#include "config.hpp"
#include "dolfs/dolfs.hpp"
#include "dolfs/figatree.hpp"
#include "dolfs/animation_track.hpp"
#include "gxtypes.hpp"

#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/mesh.h>
#include <assimp/Exporter.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

FigaTree::FigaTree(
        const DatFile * datfile, figatree_header * fig) :
        fig(fig), datfile(datfile) {

    fix_endianness(fig, sizeof(figatree_header), sizeof(uint32_t));

    this->trackCtTable = new TrackCtTable(
            datfile,
            (unsigned char *) (datfile->dataSection + fig->boneTableOffset));

    this->animDatas = new TrackHeader * [trackCtTable->numTracks];
    for (size_t i=0; i<trackCtTable->numTracks; i++) {
        this->animDatas[i] = new TrackHeader(
            datfile, 
            (track_header*) (
                datfile->dataSection + 
                    fig->animDataOffset +
                    sizeof(track_header) * i
            ));

        if (i != 0) {
            this->animDatas[i - 1]->informNextOffset(
                min(
                    (uint32_t) this->animDatas[i - 1]->animhead->length,

                    this->animDatas[i]->animhead->animdataOffset -
                    this->animDatas[i - 1]->animhead->animdataOffset
                )
            );
        }
    }

    TrackHeader * last = this->animDatas[trackCtTable->numTracks - 1];
    last->
        informNextOffset(
        get_available_size(
            datfile->dataSection + last->animhead->animdataOffset, 
            last->animhead->length));



}

void FigaTree::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << YELLOW << "figatree at 0x" << hex 
         << (size_t) this->fig - (size_t) this->datfile->dataSection
         << RESET << endl;
    cout << ind << YELLOW << "unknown0x0   " 
                << RESET << setw(8) << this->fig->unknown0x0 << endl;
    cout << ind << YELLOW << "unknown0x4   " 
                << RESET << setw(8) << this->fig->unknown0x4 << endl;

    cout << ind << YELLOW << "num_frames   " 
                << RESET << setw(8) << this->fig->num_frames << endl;

    cout << ind << YELLOW << "keyframCountsOffset " 
                << RESET << setw(8)
                << ("0x" + to_string_hex(this->fig->boneTableOffset)) << endl;

    cout << ind << YELLOW << "keyframe Counts" << endl;
    
    trackCtTable->print(indent + 1);


    cout << ind << YELLOW << "animDataOffset " 
                << RESET << setw(8)
                << ("0x" + to_string_hex(this->fig->animDataOffset)) << endl;

    cout << ind << YELLOW << "content of animData" << endl;

    int tracks_count = 0;
    // ignore bone 0, since bone 0 shouldn't be animated?
    for (size_t i=1; i<this->trackCtTable->length; i++) {
        int numTracks = this->trackCtTable->head[i];
        for (int t=0; t<numTracks; t++) {
            cout << ind << YELLOW << "bone " << dec << i
                    << " track " << dec << t
                    << " (track " << dec << tracks_count << " overall)"
                    << RESET << endl;
            animDatas[tracks_count]->print(indent + 1);
            tracks_count++;
        }
    }
}

void FigaTree::serialize(aiScene *scene) {
    aiNodeAnim * animations = this->writeAllBoneTracks(scene->mMeshes[0]);
    scene->mAnimations[0] = animation;
}





TrackCtTable::TrackCtTable(const DatFile * datfile, unsigned char * head) :
    datfile(datfile), head(head) {

    unsigned char * c = head;
    numTracks = 0;
    numBones = 0;
    while (*c != 0xFF) {
        numBones++;
        numTracks += *c;
        c++;
    }
    this->length = c - head;
    cout << RED << numTracks << RESET << endl;
    
}

void TrackCtTable::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');
    cout << ind << BLUE << "length: " << length << RESET << endl;
    cout << ind << BLUE;
    for (size_t i=0; i<length; i++) {
        cout << (int) head[i] << " ";
        if (i % 8 == 0 && i != 0) {
            cout << RESET << endl << ind << BLUE;
        }
    }
    cout << endl;
}

void TrackCtTable::serialize(aiScene *scene) {
    // TODO
}

aiNodeAnim * FigaTree::writeBoneTracks(
        char * mNodeName,
        TrackHeader * headers,
        size_t len) {
    unsigned int numFrames = this->fig->num_frames;

    // init the anim
    aiNodeAnim * newAnim = new aiNodeAnim();
    newAnim->mNodeName = aiString(mNodeName);
    newAnim->mNumPositionKeys = (int) numFrames;
    newAnim->mPositionKeys = new aiVectorKey[(int) numFrames];
    newAnim->mNumRotationKeys= (int) numFrames;
    newAnim->mRotationKeys = new aiQuatKey[(int) numFrames];
    newAnim->mNumScalingKeys = (int) numFrames;
    newAnim->mScalingKeys = new aiVectorKey[(int) numFrames];

    // initialize
    for(unsigned int i=0; i<numFrames; i++) {
        aiVectorKey* currentKey = &(newAnim->mPositionKeys[i]);
        currentKey->mTime = 1.0F/60;
        currentKey->mValue = aiVector3D(0, 0, 0);
    }

    // write
    for(unsigned int i=0; i<len; i++) {
        headers[i].writeTrack(newAnim, numFrames);
    }

    return newAnim;
}

aiNodeAnim * FigaTree::writeAllBoneTracks(aiMesh * mesh) {
    unsigned int currentTrack = 0;
    unsigned int currentBone = 0;
    unsigned int numBones = trackCtTable->numBones;
    aiNodeAnim * anims = new aiNodeAnim[trackCtTable->numBones];

    while(currentTrack < numBones) {
        // write some number of tracks corresponding to the thing
        unsigned int thisStep = trackCtTable->head[currentTrack];

        aiBone * bone = mesh->mBones[currentBone];
        aiString * name = &(bone->mName);

        writeBoneTracks(name->data, this->animDatas[currentTrack], thisStep);

        currentTrack += thisStep;
        currentBone++;
    }

    return anims;
};








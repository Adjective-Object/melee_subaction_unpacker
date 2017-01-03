#include <iostream>
#include <iomanip>
#include <bitset>
#include <set>

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
    aiAnimation * animation = new aiAnimation();
    cout << "this many tracks: " << dec << trackCtTable->numTracks << endl;
    vector<aiNodeAnim *> *anims = this->writeAllBoneTracks(scene->mMeshes[0]);
    animation->mNumChannels = anims->size();
    animation->mChannels = new aiNodeAnim *[anims->size()];
    std::copy(anims->begin(), anims->end(), animation->mChannels);

    // add the animation to the scene
    scene->mAnimations = new aiAnimation*[1];
    scene->mAnimations[0] = animation;
    scene->mNumAnimations = 1;

    // TODO tell the root node it holds some animation?
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
        aiString * mNodeName,
        TrackHeader ** headers,
        size_t len) {

    // scan through the channels and get the times for each of rotation, scaling
    // keys

    set<int> positionKeyframeTimes;
    set<int> rotationKeyframeTimes;
    for(unsigned int i=0; i<len; i++) {
        headers[i]->readKeyframeTimes(positionKeyframeTimes, rotationKeyframeTimes);
    }

    // init the anim
    aiNodeAnim * newAnim = new aiNodeAnim();
    newAnim->mNodeName = *mNodeName;
    newAnim->mNumPositionKeys = (int) positionKeyframeTimes.size();
    if (positionKeyframeTimes.size()) {
        newAnim->mPositionKeys = new aiVectorKey[(int) positionKeyframeTimes.size()];
    }

    newAnim->mNumRotationKeys= (int) rotationKeyframeTimes.size();
    if (rotationKeyframeTimes.size()) {
        newAnim->mRotationKeys = new aiQuatKey[(int) rotationKeyframeTimes.size()];
    }

    newAnim->mNumScalingKeys = (int) 0;
    if (false) {
        newAnim->mScalingKeys = new aiVectorKey[(int) 0];
    }

    int i=0;
    cout << "position times ";
    for (int posTime: positionKeyframeTimes) {
        cout << posTime << " ";
        aiVectorKey* currentKey = &(newAnim->mPositionKeys[i]);
        currentKey->mTime = posTime;
        currentKey->mValue = aiVector3D(0, 0, 0);
        i++;
    }
    cout << endl;

    i=0;
    cout << "rotation times ";
    for (int rotTime: rotationKeyframeTimes) {
        cout << rotTime << " ";
        aiQuatKey * currentRotKey = &(newAnim->mRotationKeys[i]);
        currentRotKey->mTime = i ; // * 1.0F/60;
        currentRotKey->mValue = aiQuaternion(0, 0, 0, 0);
        i++;
    }
    cout << endl;

    // write
    for(unsigned int i=0; i<len; i++) {
        headers[i]->writeTrack(newAnim, 20);
    }

    return newAnim;
}

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
inline aiVector3D rotationMatrixToEulerAngles(aiMatrix4x4 &R) {
    float sy = sqrt(R.a1 * R.a1 + R.b1 * R.b1);
 
    bool singular = sy < 1e-6; // If
 
    float x, y, z;
    if (!singular)
    {
        x = atan2(R.c2, R.c3);
        y = atan2(-R.c1, sy);
        z = atan2(R.b1, R.a1);
    }
    else
    {
        x = atan2(-R.b3, R.b2);
        y = atan2(-R.c1, sy);
        z = 0;
    }
    return aiVector3D(x, y, z);
}

void makeRelative(aiNodeAnim * anim, aiBone * bone) {
    aiMatrix4x4 mat;
    memcpy(&mat, &bone->mOffsetMatrix, sizeof(aiMatrix4x4));
    // assume the matrix is an affine transformation
    // so normalize the matrix wrt the bottom left value (d4)
    mat.a1 /= mat.d4;
    mat.a2 /= mat.d4;
    mat.a3 /= mat.d4;
    mat.a4 /= mat.d4;
    mat.b1 /= mat.d4;
    mat.b2 /= mat.d4;
    mat.b3 /= mat.d4;
    mat.b4 /= mat.d4;
    mat.c1 /= mat.d4;
    mat.c2 /= mat.d4;
    mat.c3 /= mat.d4;
    mat.c4 /= mat.d4;
    mat.d1 /= mat.d4;
    mat.d2 /= mat.d4;
    mat.d3 /= mat.d4;
    mat.d4 = 1;
    
    // zero last column (a4 b4 c4 d4) and extract as translate
    float tx = mat.a4, ty = mat.b4, tz = mat.c4;
    mat.a4 = 0;
    mat.b4 = 0;
    mat.c4 = 0;

    // get scale from the length of the first 3 columns
    float sx = aiVector3D(mat.a1, mat.b1, mat.c1).Length();
    float sy = aiVector3D(mat.a2, mat.b2, mat.c2).Length();
    float sz = aiVector3D(mat.a3, mat.b3, mat.c3).Length();

    // normalize by scale
    mat.a1 /= sx;
    mat.b1 /= sx;
    mat.c1 /= sx;
    mat.a2 /= sy;
    mat.b2 /= sy;
    mat.c2 /= sy;
    mat.a3 /= sz;
    mat.b3 /= sz;
    mat.c3 /= sz;

    aiVector3D rotation = rotationMatrixToEulerAngles(mat);
    aiQuaternion x = aiQuaternion(aiVector3D(1, 0, 0), rotation.x);
    aiQuaternion y = aiQuaternion(aiVector3D(0, 1, 0), rotation.y);
    aiQuaternion z = aiQuaternion(aiVector3D(0, 0, 1), rotation.z);

    for (size_t i = 0; i < anim->mNumPositionKeys; i++) {
        aiVectorKey *pos = &(anim->mPositionKeys[i]);
        pos->mValue += aiVector3D(tx, ty, tz);
    }
    for (size_t i = 0; i < anim->mNumRotationKeys; i++) {
        aiQuatKey *rot = &(anim->mRotationKeys[i]);
        rot->mValue = z * y * x;// * rot->mValue;
    }
    for (size_t i = 0; i < anim->mNumScalingKeys; i++) {
        aiVectorKey *scale = &(anim->mScalingKeys[i]);
        scale->mValue += aiVector3D(tx, ty, tz);
    }
}

vector<aiNodeAnim *> *FigaTree::writeAllBoneTracks(aiMesh * mesh) {
    unsigned int currentTrack = 0;
    cout << "this many bones : " << dec << trackCtTable->numBones << endl;
    unsigned int numBones = (unsigned int) trackCtTable->numBones;
    vector<aiNodeAnim *> *anims = new vector<aiNodeAnim *>();

    for(unsigned int currentBone = 0; currentBone < numBones; currentBone++) {
        // write some number of tracks corresponding to the thing
        unsigned int thisStep = trackCtTable->head[currentBone];

        if (thisStep == 0) {
            continue;
        }

        cout << "writing tracks " << currentTrack << " .. "
             << (currentTrack + thisStep)
             << " to bone " << currentBone
             << endl;

        aiBone * bone = mesh->mBones[currentBone];
        aiString * name = &(bone->mName);

//        printf("%*s\n", name->length, name->data);

        aiNodeAnim * anim = writeBoneTracks(
            name,
            &this->animDatas[currentTrack],
            thisStep);

        makeRelative(anim, bone);
    
        anims->push_back(anim);

        currentTrack += thisStep;
    }

    return anims;
};








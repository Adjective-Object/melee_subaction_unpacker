#include "dolfs/dolfs.hpp"
#include "dolfs/animation_track.hpp"
#include "helpers.hpp"
#include "config.hpp"
#include "gxtypes.hpp"

#include <stdint.h>
#include <iomanip>
#include <iostream>
#include <bitset>
#include <cmath>

#include <assimp/scene.h>
#include <assimp/anim.h>
#include <assimp/mesh.h>
#include <assimp/Exporter.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/vector3.h>

TrackHeader::TrackHeader(
        const DatFile * datfile, track_header * animhead) :
        datfile(datfile), animhead(animhead) {
    fix_endianness(& (animhead->length),            sizeof(uint16_t));
    fix_endianness(& (animhead->unknown_padding),   sizeof(uint16_t));
    fix_endianness(& (animhead->unknown_padding_2),     sizeof(uint8_t));
    fix_endianness(& (animhead->track_type),     sizeof(uint8_t));
    fix_endianness(& (animhead->value_format),     sizeof(uint8_t));
    fix_endianness(& (animhead->tan_format),     sizeof(uint8_t));
    fix_endianness(& (animhead->animdataOffset),    sizeof(uint32_t));


    /*    

    // assumptions about flags
    size_t length = sizeof(uint32_t) * 2;
    if (((animhead->unknown_flags >> 20) & 0xF) == 0x8 ||
        ((animhead->unknown_flags >> 20) & 0xF) == 0x6) {
        length = sizeof(uint32_t) * 1;
    }
    else {
        length = sizeof(uint32_t) * 2;
    }

    */
}

void TrackHeader::informNextOffset(size_t nextOffset) {
    void * content = (datfile->dataSection + animhead->animdataOffset);
    size_t length = nextOffset;

    this->headerInspector= new DatInspector(
            datfile, content, length);

    this->bodyInspector = new DatInspector(
            datfile, content, length);

}


static const char * str_tracktype(uint8_t tracktype) {
    switch(tracktype) {
        case X_ROTATION: return "x rotation";
        case Y_ROTATION: return "y rotation";
        case Z_ROTATION: return "z rotation";
        case X_TRANSLATION: return "x translation";
        case Y_TRANSLATION: return "y translation";
        case Z_TRANSLATION: return "z translation";
        case X_SCALE: return "x scale";
        case Y_SCALE: return "y scale";
        case Z_SCALE: return "z scale";
        default: return "unknown";
    }
}

static const char * str_datatype(uint8_t datatype) {
    uint8_t type = datatype >> 5;
    // uint8_t scaling = datatype & 0b11111;
    switch (type) {
        case GX_U8: return "uint8_t"; 
        case GX_S8: return "int8_t"; 
        case GX_U16: return "uint16_t";
        case GX_S16: return "int16_t";
        case GX_F32: return "float_32";
        default: return "unknown datatype";
    }
}

static const char * str_interpolation_format(uint8_t datatype) {
    switch (datatype) {
        case UNMANAGED: return "unmanaged";
        case STEP: return "step";
        case LINEAR: return "linear";
        case HERMITE_VALUE: return "hermite_val";
        case HERMITE_VALUE_AND_TANGENT: return "hermite_both";
        case HERMITE_TANGENT: return "hermite_tan";
        case CONSTANT: return "const";
        default: return "unknown interpolation format";
    }
}


void TrackHeader::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');
    cout << ind << GREEN << "TrackHeader at : 0x" 
         << hex << ((uint8_t *) animhead - (uint8_t *) datfile->dataSection)
         << RESET << endl;
    cout << ind << GREEN << "length: " << animhead->length << RESET << endl;
    cout << ind << GREEN << "unknown_padding: " 
         << animhead->unknown_padding << RESET << endl;
    cout << ind << GREEN << "track_type:   " 
         << hex << "0x" << setfill('0') << setw(2) 
                << +animhead->track_type 
                << " (" << dec << +animhead->track_type << ": " 
                << str_tracktype(animhead->track_type) << ")"
                << RESET << endl;

    cout << ind << GREEN << "value_format: " 
         << hex << "0x" << setfill('0') << setw(2) 
                << +animhead->value_format 
                << " (" << dec << +animhead->value_format << ")" 
                << " " << str_datatype(animhead->value_format)
                << RESET << endl;

    cout << ind << GREEN << "tan_format: "
         << hex << "0x" << setfill('0') << setw(2)
                << +animhead->tan_format
                << " (" << dec << +animhead->tan_format  << ")" 
                << " " << str_datatype(animhead->value_format)
                << RESET << endl;

    cout << ind << GREEN << "animdataOffset: " 
         << hex << "0x" << animhead->animdataOffset << RESET << endl;

    uint8_t* anim_track = (uint8_t*)(datfile->dataSection + animhead->animdataOffset);
    uint8_t* anim_track_origin = anim_track;

    // for reference
    this->headerInspector->printRaw(indent);

    //while (anim_track < anim_track_origin + animhead->length) {

        void * old_anim_track = anim_track;
        anim_track = this->print_anim_data(
            indent + 1, 
            anim_track,
            +animhead->value_format, 
            +animhead->tan_format);

        bodyInspector->move(
                old_anim_track,
                (((size_t) anim_track) - ((size_t) old_anim_track))
                );
        this->bodyInspector->printRaw(indent + 1);
    //}

    size_t endingOffset = ((size_t) anim_track) - (size_t) datfile->dataSection;
    cout << "last value endingOffset " << hex << endingOffset << endl;
    cout << "expected length: " << dec << animhead->length 
         << " real length: " << dec
         << endingOffset - animhead->animdataOffset << endl;
}


uint8_t * TrackHeader::print_anim_data(
        int indent, uint8_t * anim_track,
        uint8_t valType, uint8_t tanType) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << BLUE << ind
         << "keyframe command at 0x" 
         << hex << anim_track - (uint8_t *)datfile->dataSection
         << RESET << endl;

    cout << ind
         << RED << "vvvVVVVV"
         << YELLOW << "tttTTTTT"
         << GREEN << "?xxxxxxx"
         << RESET << endl;

    GXCompType vtype = (GXCompType) ((+valType) >> 5);
    uint8_t val_scale = valType & 0x1F;

    GXCompType ttype = (GXCompType) ((+tanType) >> 5);
    uint8_t tan_scale = tanType & 0x1F;

    cout << ind << BLUE 
         << std::bitset<8>(valType)
         << " value format: " << setfill(' ') 
         << setw(8) << gx_name((GXCompType) vtype) << setw(2) << dec << vtype
         << " scaled " << dec << +val_scale << RESET << endl;

    cout << ind << BLUE
         << std::bitset<8>(tanType)
         << " tan format:   "
         << setw(8) << gx_name((GXCompType) ttype) << setw(2) << dec << vtype
         << " scaled " << dec << +tan_scale << RESET << endl; 

    // read the header of this 'animation command'
    uint8_t interpolation_type = (anim_track[0] >> 4) & 0b0111;

    unsigned int keyframe_count = anim_track[0] & 0x0F;
    uint8_t * track = read_varlen_int(&anim_track[0], & keyframe_count);

    keyframe_count = keyframe_count + 1;

    cout << ind << GREEN << "interpolation type: ("
         << std::dec << +interpolation_type
         << " " << str_interpolation_format(interpolation_type)
         << ") " << RESET << endl;

    cout << ind << GREEN << "keyframe count:     (" 
         << +keyframe_count
         << ") " << RESET << endl;
    // cout << ind << YELLOW << "i = " << i << RESET << endl;


    // now read the interpolation data
    return (uint8_t *) print_gx_frames(
            indent, datfile->dataSection, track, 
            interpolation_type,
            keyframe_count, 
            vtype, val_scale, 
            ttype, tan_scale);
}

void * print_gx_frames(int indent, void * origin, void* data, 
        uint8_t interpolation_type,
        size_t numFrames, 
        GXCompType data_type, int data_scale,
        GXCompType tan_type, int tan_scale ) {

    if (interpolation_type == UNMANAGED) {
       // numFrames = 2;
    }

    string ind(indent * INDENT_SIZE, ' ');
    uint8_t * head = (uint8_t*) data;
    for (size_t i=0; i<numFrames; i++) {
        // value
        cout << GREEN << ind << "0x" << hex << setfill('0') << setw(4) 
            << (size_t)(head - (uint8_t *) origin) << RESET << ' ';
        cout << setfill(' ') << dec;
        switch(interpolation_type) {
            case STEP:
            case LINEAR:
            case HERMITE_VALUE:
            case HERMITE_VALUE_AND_TANGENT:
            case CONSTANT:
            case UNMANAGED:
                cout << "value: " << setw(5);
                head = print_gx(head, data_type, data_scale);
                if (interpolation_type != HERMITE_VALUE_AND_TANGENT) break;

            case HERMITE_TANGENT:
                cout << "     tan: " << setw(5);
                head = print_gx(head, tan_type, tan_scale);
                break;

            default:
                cout << "assumed value: " << setw(5);
                head = print_gx(head, data_type, data_scale);
                cout << "     tan: " << setw(5);
                head = print_gx(head, tan_type, tan_scale);
                if (interpolation_type != HERMITE_VALUE_AND_TANGENT) break;

                break;
        }

        // duration
        unsigned int duration;
        duration = *head & 0x7F;
        head = read_varlen_int(head, &duration);
        cout << "    duration: " << setw(5) << duration << endl;
    }

    return head;
}

uint8_t * read_varlen_int(uint8_t * head, unsigned int * out) {
    unsigned int shift = 7;
    while (*head & 0x80) {
        head++;
        *out = *out | ((*head) << shift);
        shift += 7;
    }
    return head + 1;
}

uint8_t * print_gx(uint8_t * data, GXCompType type, int scale) {
    int val;
    switch(type) {
        case GX_U8:
            // fix_endianness((uint8_t *) data, sizeof(uint8_t));
            val = *((uint8_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) 
                 << " (" << setw(4) << val << "/ 2^" << scale << ")";
            return data + sizeof(uint8_t);
        case GX_S8:
            // fix_endianness((uint8_t *) data, sizeof(uint8_t));
            val = *((int8_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) 
                 << " (" << setw(4) << val << "/ 2^" << scale << ")";
            return data + sizeof(int8_t);
        case GX_U16:
            // fix_endianness((uint8_t *) data, sizeof(uint16_t));
            val = *((uint16_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) 
                 << " (" << setw(4) << val << "/ 2^" << scale << ")";
            return data + sizeof(uint16_t);
        case GX_S16:
            // fix_endianness((uint8_t *) data, sizeof(uint16_t));
            val = *((int16_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) 
                 << " (" << setw(4) << val << "/ 2^" << scale << ")";
            return data + sizeof(int16_t);
        case GX_F32:
            // fix_endianness((float *) data, sizeof(float));
            val = *((float *) data);
            cout << setw(12) << val;
            return data + sizeof(float);
        default:
            cout << setw(12) << RED
                 << "unknown GxCompType " 
                 << hex << type << RESET;
            return data + 1;
    }
}


uint8_t * read_gx(uint8_t * data, GXCompType type, int scale, float * out) {
    int val;
    switch(type) {
        case GX_U8:
        default:
            // fix_endianness((uint8_t *) data, sizeof(uint8_t));
            val = *((uint8_t *) data);
            *out = (((float) val)) / pow(2.0, scale);
            return data + 1;
        case GX_S8:
            // fix_endianness((uint8_t *) data, sizeof(uint8_t));
            val = *((int8_t *) data);
            *out =  (((float) val)) / pow(2.0, scale);
            return data + 1;
        case GX_U16:
            // fix_endianness((uint8_t *) data, sizeof(uint16_t));
            val = *((uint16_t *) data);
            *out =  (((float) val)) / pow(2.0, scale);
            return data + 1;
        case GX_S16:
            // fix_endianness((uint8_t *) data, sizeof(uint16_t));
            val = *((int16_t *) data);
            *out =  (((float) val)) / pow(2.0, scale);
            return data + 1;
        case GX_F32:
            // fix_endianness((float *) data, sizeof(float));
            val = *((float *) data);
            *out =  (((float) val)) / pow(2.0, scale);
            return data + 1;
    }
}

uint8_t * readKeyframeHead(
        uint8_t* track,
        keyframe_head * keyframe) {
    
    // read the header of this 'animation command'
    keyframe->interpolation_type = (*track >> 4) & 0b0111;
    unsigned int numFrames;
    numFrames = *track & 0x0F;
    track = read_varlen_int(track, &numFrames);
    numFrames ++;
    keyframe->numFrames = numFrames;
    return track;
}

void readConfig(GXConfig * g, track_header * animhead) {
    g->val_type = (GXCompType)((+animhead->value_format) >> 5);
    g->tan_type = (GXCompType)((+animhead->tan_format) >> 5);
    g->val_scale = +animhead->value_format & 0x1F;
    g->val_scale = +animhead->value_format & 0x1F;
}

uint8_t * read_gx_pair(
        uint8_t * buffer,
        uint8_t interpolation_type,
        GXConfig * g,
        float *value,
        float * tangent) {

    switch(interpolation_type) {
        case STEP:
        case LINEAR:
        case HERMITE_VALUE:
        case HERMITE_VALUE_AND_TANGENT:
        case CONSTANT:
        case UNMANAGED:
            buffer = read_gx(buffer, g->val_type, g->val_scale, value);
            if (interpolation_type != HERMITE_VALUE_AND_TANGENT) break;

        case HERMITE_TANGENT:
            buffer = read_gx(buffer, g->tan_type, g->tan_scale, tangent);
            break;

        default:
            break;
    }
    return buffer;
}

void TrackHeader::writeTrack(
        aiNodeAnim * newAnim,
        size_t maxFrames
        ) {
    GXConfig g;
    readConfig(&g, this->animhead);
    
    uint8_t * buffer = (uint8_t *) (datfile->dataSection + animhead->animdataOffset);

    keyframe_head track;
    buffer = readKeyframeHead(buffer, &track);

    size_t remainingKeyFrames = track.numFrames;
    size_t remainingFrames = 0;

    float tangent = 0, value = 0, tangent_next = 0, value_next = 0;

    buffer = read_gx_pair(
            buffer,
            track.interpolation_type,
            &g,
            &value_next,
            &tangent_next);
    
    for(unsigned int i=0; i<maxFrames; i++) {
        if (remainingKeyFrames <= 0) {
            buffer = readKeyframeHead(buffer, &track);
            remainingKeyFrames = track.numFrames;
        }

        if (remainingFrames <= 0) {
            value = value_next;
            tangent = tangent_next;
            buffer = read_gx_pair(
                    buffer,
                    track.interpolation_type,
                    &g,
                    &value_next,
                    &tangent_next);
        }

        double interpolatedValue = 
            value + (value_next - value) * 
            (1 - (remainingFrames / (float) track.numFrames));

        aiVectorKey* currentPKey = &(newAnim->mPositionKeys[i]);
        aiVectorKey* currentSKey = &(newAnim->mScalingKeys[i]);
        aiQuatKey* currentRKey = &(newAnim->mRotationKeys[i]);
        // TODO fill me out
        switch(this->animhead->track_type) {
            case X_ROTATION: 
                currentRKey->mValue.x = interpolatedValue;
                break;
            case Y_ROTATION: 
                currentRKey->mValue.y = interpolatedValue;
                break;
            case Z_ROTATION: 
                currentRKey->mValue.z = interpolatedValue;
                break;
            case X_TRANSLATION: 
                currentPKey->mValue.x = interpolatedValue;
                break;
            case Y_TRANSLATION: 
                currentPKey->mValue.y = interpolatedValue;
                break;
            case Z_TRANSLATION: 
                currentPKey->mValue.z = interpolatedValue;
                break;
            case X_SCALE: 
                currentSKey->mValue.x = interpolatedValue;
                break;
            case Y_SCALE: 
                currentSKey->mValue.y = interpolatedValue;
                break;
            case Z_SCALE: 
                currentSKey->mValue.z = interpolatedValue;
                break;
            default: 
                break;
        }

    }
}

void TrackHeader::serialize() {
}


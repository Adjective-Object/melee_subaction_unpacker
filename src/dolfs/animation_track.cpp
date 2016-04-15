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

    this->targetInspector = new DatInspector(
            datfile, content, length);

}


static const char * str_tracktype(uint8_t tracktype) {
    switch(tracktype) {
        case 1: return "x rotation";
        case 2: return "y rotation";
        case 3: return "z rotation";
        case 5: return "x translation";
        case 6: return "y translation";
        case 7: return "z translation";
        case 8: return "x scale";
        case 9: return "y scale";
        case 10: return "z scale";
        default: return "unknown";
    }
}




void TrackHeader::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');
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
                << RESET << endl;

    cout << ind << GREEN << "tan_format: " 
         << hex << "0x" << setfill('0') << setw(2) 
                << +animhead->tan_format << RESET << endl;

    cout << ind << GREEN << "animdataOffset: " 
         << hex << "0x" << animhead->animdataOffset << RESET << endl;

    uint8_t* anim_track = (uint8_t*)(datfile->dataSection + animhead->animdataOffset);

    // for reference
    this->targetInspector->printRaw(indent);

    anim_track = this->print_anim_data(
        indent + 1, 
        anim_track,
        +animhead->value_format, 
        +animhead->tan_format);
    size_t endingOffset = ((size_t) anim_track) - (size_t) datfile->dataSection;
    cout << "last value endingOffset " << hex << endingOffset << endl;
    cout << "expected length: " << dec << animhead->length 
         << " real length: " << dec
         << endingOffset - animhead->animdataOffset << endl;
}


uint8_t * TrackHeader::print_anim_data(
        int indent, uint8_t * anim_track, uint8_t valType, uint8_t tanType) {
    string ind(indent * INDENT_SIZE, ' ');

    GXCompType vtype = (GXCompType) (GX_F32 - ((+valType) >> 5));
    uint8_t val_scale = valType & 0x1F;
    GXCompType ttype = (GXCompType) (GX_F32 - ((+tanType) >> 5));
    uint8_t tan_scale = tanType & 0x1F;

    cout << ind 
         << std::bitset<8>(valType)
         << " value format: " << setfill(' ') << setw(8) << gx_name((GXCompType) vtype)
         << " scaled " << dec << +val_scale << endl;
    cout << ind 
         << std::bitset<8>(tanType)
         << " tan format:   " << setw(8) << gx_name((GXCompType) ttype)
         << " scaled " << dec << +tan_scale << endl; 

    // read the header of this 'animation command'
    uint8_t interpolation_type = anim_track[0] >> 4;
    uint8_t keyframe_count = (anim_track[0] & 0x0F);

    int i = 0;
    while (anim_track[i] & 0x80) {
        i++;
        keyframe_count = keyframe_count | (anim_track[i] << 7);
    }

    keyframe_count = keyframe_count + 1;

    cout << ind << GREEN << "interpolation type: ("
         << std::dec << +interpolation_type << ") " << RESET << endl;
    cout << ind << GREEN << "keyframe count:     (" 
         << std::dec << +keyframe_count << ") " << RESET << endl;
    // cout << ind << YELLOW << "i = " << i << RESET << endl;


    // now read the interpolation data
    return (uint8_t *) print_gx_frames(
            indent, &(anim_track[i + 1]), 
            interpolation_type,
            keyframe_count, 
            vtype, val_scale, 
            ttype, tan_scale);
}

void * print_gx_frames(int indent, void* data, 
        uint8_t interpolation_type,
        size_t numFrames, 
        GXCompType data_type, int data_scale,
        GXCompType tan_type, int tan_scale ) {

    string ind(indent * INDENT_SIZE, ' ');
    char * head = (char*) data;
    for (size_t i=0; i<numFrames; i++) {
        // value
        cout << ind << "value: " << setw(5);
        head = print_gx(head, data_type, data_scale);

        // tangent
        switch(interpolation_type) {
            case 0:
            case 1:
                break;

            default:
                cout << "     tan: " << setw(5);
                head = print_gx(head, tan_type, tan_scale);
        }

        // duration
        int duration;
        head = read_varlen_int(head, &duration);
        cout << "    duration: " << setw(5) << duration << endl;
    }

    return head;
}

char * read_varlen_int(char * head, int * out) {
    *out = *head & 0x7F;
    while (*head & 0x80) {
        head++;
        *out = *out | ((*head) << 7);
    }
    return head;
}

char * print_gx(char * data, GXCompType type, int scale) {
    int val;
    switch(type) {
        case GX_U8:
            val = *((uint8_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) << " (" << val << "/ 2^" << scale << ")";
            return data + sizeof(uint8_t);
        case GX_S8:
            val = *((int8_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) << " (" << val << "/ 2^" << scale << ")";
            return data + sizeof(int8_t);
        case GX_U16:
            val = *((uint16_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) << " (" << val << "/ 2^" << scale << ")";
            return data + sizeof(uint16_t);
        case GX_S16:
            val = *((int16_t *) data);
            cout << setw(12) << (val / pow(2.0, scale)) << " (" << val << "/ 2^" << scale << ")";
            return data + sizeof(int16_t);
        case GX_F32:
            val = *((float *) data);
            cout << setw(12) << val;
            return data + sizeof(float);
        default:
            cout << setw(12) << RED
                 << "unknown GxCompType " 
                 << type << RESET;
            return data + 1;
    }
}

void TrackHeader::serialize() {}



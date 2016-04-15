#include <iostream>
#include <iomanip>
#include <bitset>

#include "helpers.hpp"
#include "config.hpp"
#include "dolfs/dolfs.hpp"
#include "dolfs/figatree.hpp"
#include "dolfs/animation_track.hpp"
#include "gxtypes.hpp"

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

    TrackHeader * last = this->animDatas[trackCtTable->length - 1];
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
    for (size_t i=0; i<this->trackCtTable->length; i++) {
        int numTracks = this->trackCtTable->head[i];
        for (int t=0; t<max(1, numTracks); t++) {
            cout << ind << YELLOW << "bone " << dec << i
                    << " track " << dec << t
                    << " (track " << dec << tracks_count << " overall)"
                    << RESET << endl;
            animDatas[tracks_count]->print(indent + 1);
            tracks_count++;
        }
    }
}

void FigaTree::serialize() {
    // TODO
}





TrackCtTable::TrackCtTable(const DatFile * datfile, unsigned char * head) :
    datfile(datfile), head(head) {

    unsigned char * c = head;
    numTracks = 0;
    while (*c != 0xFF) {numTracks += *c; c++;}
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

void TrackCtTable::serialize() {
    // TODO
}








#include <iostream>
#include <iomanip>
#include <bitset>

#include "helpers.hpp"
#include "config.hpp"
#include "dolfs/dolfs.hpp"
#include "dolfs/figatree.hpp"

FigaTree::FigaTree(
        const DatFile * datfile, figatree_header * fig) :
        fig(fig), datfile(datfile) {

    fix_endianness(fig, sizeof(figatree_header), sizeof(uint32_t));

    this->boneIndexTable = new BoneIndexTable(
            datfile,
            (unsigned char *) (datfile->dataSection + fig->boneTableOffset));

    this->animDatas = new AnimDataHeader * [boneIndexTable->length];
    for (size_t i=0; i<boneIndexTable->length; i++) {
        this->animDatas[i] = new AnimDataHeader(
            datfile, 
            (animdata_header*) (
                datfile->dataSection + 
                    fig->animDataOffset +
                    sizeof(animdata_header) * i
            ),
            boneIndexTable->head[i]
        );

        if (i != 0) {
            this->animDatas[i - 1]->informNextOffset(
                min(
                    (uint32_t) this->animDatas[i - 1]->animhead->length - 1,

                    this->animDatas[i]->animhead->animdataOffset -
                    this->animDatas[i - 1]->animhead->animdataOffset
                )
            );
        }
    }

    AnimDataHeader * last = this->animDatas[boneIndexTable->length - 1];
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

    cout << ind << YELLOW << "boneTableOffset " 
                << RESET << setw(8)
                << ("0x" + to_string_hex(this->fig->boneTableOffset)) << endl;

    cout << ind << YELLOW << "content of bone table" << endl;
    
    boneIndexTable->print(indent + 1);


    cout << ind << YELLOW << "animDataOffset " 
                << RESET << setw(8)
                << ("0x" + to_string_hex(this->fig->animDataOffset)) << endl;

    cout << ind << YELLOW << "content of animData" << endl;

    for (size_t i=0; i<this->boneIndexTable->length; i++) {
        cout << ind << YELLOW << "bone " << dec << i
                    << " (boneflag = " << dec << +(this->boneIndexTable->head[i]) << ")"
                    << RESET << endl;
        animDatas[i]->print(indent + 1);
    }
}

void FigaTree::serialize() {
    // TODO
}





BoneIndexTable::BoneIndexTable(const DatFile * datfile, unsigned char * head) :
    datfile(datfile), head(head) {

    unsigned char * c = head;
    while (*c != 0xFF) { c++; }
    this->length = c - head;
}

void BoneIndexTable::print(int indent) {
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

void BoneIndexTable::serialize() {
    // TODO
}







AnimDataHeader::AnimDataHeader(
        const DatFile * datfile, animdata_header * animhead, 
        unsigned char boneflag) :
        datfile(datfile), animhead(animhead), boneflag(boneflag) {
    fix_endianness(& (animhead->length),            sizeof(uint16_t));
    fix_endianness(& (animhead->unknown_padding),   sizeof(uint16_t));
    fix_endianness(& (animhead->unknown_flags),     sizeof(uint32_t));
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

void AnimDataHeader::informNextOffset(size_t nextOffset) {
    void * content = (datfile->dataSection + animhead->animdataOffset);
    size_t length = nextOffset;

    // fix_endianness(content, length, sizeof(uint16_t));
    this->targetInspector = new DatInspector(
            datfile, content, length);

}

void AnimDataHeader::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');
    cout << ind << GREEN << "length: " << animhead->length << RESET << endl;
    cout << ind << GREEN << "unknown_padding: " 
         << animhead->unknown_padding << RESET << endl;
    cout << ind << GREEN << "unknown_flags: " 
         << hex << "0x" << setfill('0') << setw(8) 
                << animhead->unknown_flags << RESET << endl;
    cout << ind << GREEN << "unknown_flags: " << RESET
         << bitset<32>(animhead->unknown_flags) << endl;
    cout << ind << GREEN << "animdataOffset: " 
         << hex << "0x" << animhead->animdataOffset << RESET << endl;

    switch (this->animhead->unknown_flags) {
        case 0x022d0000:
            this->targetInspector->printRaw(indent + 1, 21, 3);
            break;

        case 0x022e8800:
            this->targetInspector->printRaw(indent + 1, 9, 3);
            break;

        case 0x02000000:
            this->targetInspector->printRaw(indent + 1, 4);
            break;


        default:
            this->targetInspector->printRaw(indent + 1, 9);
    }

}

void AnimDataHeader::serialize() {}



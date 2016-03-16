#include <iostream>
#include <iomanip>

#include "helpers.hpp"
#include "config.hpp"
#include "dolfs/dolfs.hpp"
#include "dolfs/figatree.hpp"

FigaTree::FigaTree(
        const DatFile * datfile, figatree_header * fig) :
        fig(fig), datfile(datfile)
{
    fix_endianness(fig, sizeof(figatree_header), sizeof(uint32_t));
}

void FigaTree::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << BLUE << "dataSection -> rootlist:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->rootList) -
                ((char *) this->datfile->dataSection))

        << "     dataSection -> fig:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->fig) - 
                ((char *) this->datfile->dataSection))

        << endl << ind <<  "body     -> dataSection:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->dataSection) - 
                ((char *) MMAP_ORIGIN))

        << "     body        -> fig:" << hex 
        << setw(15) << (intptr_t) (
                ((char *) this->fig) - ((char *) MMAP_ORIGIN)
        ) << RESET << endl;

    cout << ind
         << CYAN << setw(5) << "~" << " " 
         << setw(15) << "hex"
         << " "
         << setw(15) << "dec"
         << " "
         << setw(15) << "float"
         << CYAN << endl;

    uint32_t * figg = (uint32_t *) fig;
    float * fligg = (float *) fig;

    for (uint i=0; i<sizeof(fig->a)/sizeof(uint32_t); i++) {
        cout << ind
             << CYAN << setw(4) << i << RESET << " " 
             << setw(15) << "0x" + to_string_hex(figg[i])
             << " "
             << setw(15) << dec << figg[i]
             << " "
             << setw(15) << fligg[i]
             << endl;
    }
}

void FigaTree::serialize() {
    // TODO
}



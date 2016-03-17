#include <iostream>
#include <iomanip>

#include "helpers.hpp"
#include "config.hpp"
#include "dolfs/dolfs.hpp"
#include "dolfs/figatree.hpp"

FigaTree::FigaTree(
        const DatFile * datfile, figatree_header * fig) :
        fig(fig), datfile(datfile) {
    fix_endianness(fig, sizeof(figatree_header), sizeof(uint32_t));
    this->pointedData = new DatInspector(
            datfile, 
            ((char *) MMAP_ORIGIN) + this->fig->probAnOffset,
            sizeof(float) * 27);
}

void FigaTree::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << YELLOW << "unknown0x0   " 
                << RESET << setw(8) << this->fig->unknown0x0 << endl;
    cout << ind << YELLOW << "unknown0x4   " 
                << RESET << setw(8) << this->fig->unknown0x4 << endl;
    cout << ind << YELLOW << "num_frames   " 
                << RESET << setw(8) << this->fig->num_frames << endl;
    cout << ind << YELLOW << "probAnOffset " 
                << RESET << setw(8)
                << ("0x" + to_string_hex(this->fig->probAnOffset)) << endl;

    this->pointedData->print(indent + 1);

}

void FigaTree::serialize() {
    // TODO
}



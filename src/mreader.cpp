#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "macros.h"
#include "dolfs.h"

using namespace std;

// checks the host endianness
// Create a temp file to work on and memory map it
void * makeOperatingFile(string filepath) {
    string tmpfilepath = "._" + filepath + ".tmp";
    
    ifstream infile (filepath, std::ios::binary);
    ofstream tempfile (tmpfilepath, std::ios::binary);
    
    tempfile << infile.rdbuf();

    infile.close();
    tempfile.close();

    struct stat st;
    if(stat(filepath.c_str(), &st) != 0) {
        perror("statting file");
        exit(1);
    }

    int fd = open(tmpfilepath.c_str(), O_RDWR);
    void * mapOrigin = mmap(
        nullptr, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapOrigin == MAP_FAILED) {
        perror(("mmapping file" + filepath).c_str());
        exit(1);
    }

    return mapOrigin;
}

int main(int argc, char** argv) {
    cout << endiswp(
        "host endianness matches gamecube datfile endianness",
        "host endianness does not match gamecube datfile endianness") << endl;

    char * filename = argv[1];
    printf("%s\n", filename);
    dat_header * datfile = (dat_header *) makeOperatingFile(filename);
    cout << "made temp file '._" << filename << ".tmp'" << endl;
    cout << "mmap origin: " << datfile << endl;

    DatFile * dat = new DatFile(datfile);
    dat->print();

    return 0;
}


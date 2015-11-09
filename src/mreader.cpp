#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <libgen.h>

#include "macros.h"
#include "dolfs.h"
#include "config.h"

using namespace std;

char * c_str_copy(string filepath) {
    const char * raw_fpath = filepath.c_str();
    char * rfp = (char *) malloc(strlen(raw_fpath));
    return rfp;
}

string string_dirname(string filepath) {
    char * fp = c_str_copy(filepath);
    string toret = string(dirname(fp));
    free(fp);
    return toret;
}

string string_basename(string filepath) {
    char * fp = c_str_copy(filepath);
    string toret = string(basename(fp));
    free(fp);
    return toret;
}

// checks the host endianness
// Create a temp file to work on and memory map it
void * makeOperatingFile(string filepath) {

    // get the fname and dirname 
    string tmpfilepath = 
        string_dirname(filepath) +  "/._" + 
        string_basename(filepath) + ".tmp";

    cout << "making temp file " << tmpfilepath << endl;
    
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
        perror(("mmapping file " + filepath + " failed").c_str());
        exit(1);
    }

    return mapOrigin;
}

int main(int argc, char** argv) {
    cout << endiswp(
        "host endianness matches gamecube datfile endianness",
        "host endianness does not match gamecube datfile endianness") << endl;

    char * filename = parseConf(argc, argv);
    
    printf("%s\n", filename);
    dat_header * datfile = (dat_header *) makeOperatingFile(filename);
    cout << "mmap origin: " << datfile << endl;

    DatFile * dat = new DatFile(datfile);
    dat->print();

    return 0;
}


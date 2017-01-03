#include <iostream>
#include <fstream>
#include <cstring>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <libgen.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>

#include "macros.hpp"
#include "event_mapper.hpp"
#include "config.hpp"
#include "helpers.hpp"

using namespace std;

char *c_str_copy(string filepath) {
  const char *raw_fpath = filepath.c_str();
  char *rfp = (char *)malloc(strlen(raw_fpath) + 1);
  strcpy(rfp, raw_fpath);
  return rfp;
}

string string_dirname(string filepath) {
  char *fp = c_str_copy(filepath);
  string toret = string(dirname(fp));
  free(fp);
  return toret;
}

string string_basename(string filepath) {
  char *fp = c_str_copy(filepath);
  string toret = string(basename(fp));
  free(fp);
  return toret;
}

// checks the host endianness
// Create a temp file to work on and memory map it
void *makeOperatingFile(string filepath) {

  // get the fname and dirname
  string tmpfilepath =
      string_dirname(filepath) + "/._" + string_basename(filepath) + ".tmp";

  cout << "making temp file " << tmpfilepath << endl;

  ifstream infile(filepath, std::ios::binary);
  ofstream tempfile(tmpfilepath, std::ios::binary);

  tempfile << infile.rdbuf();

  infile.close();
  tempfile.close();

  struct stat st;
  if (stat(filepath.c_str(), &st) != 0) {
    perror("statting file");
    exit(1);
  }

  int fd = open(tmpfilepath.c_str(), O_RDWR);
  void *mapOrigin =
      mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapOrigin == MAP_FAILED) {
    perror(("mmapping file " + filepath + " failed").c_str());
    exit(1);
  }

  return mapOrigin;
}

int main(int argc, char **argv) {
    cout << endiswp("host endianness matches gamecube datfile endianness",
                  "host endianness does not match gamecube datfile endianness")
       << endl;

    char **filenames = parseConf(argc, argv);
    initializeEventMap("./melee.langdef");

    aiScene * scene;
    if (EXPORT) {
        // generate the scene
        scene = new aiScene();
        scene->mRootNode = new aiNode();
        scene->mRootNode->mChildren = new aiNode*[1];
        scene->mRootNode->mNumChildren = 0;
    }



    for (unsigned int i=0; i<NUM_FILES; i++) {
        char * filename = filenames[i];
        printf("%d: %s\n", i, filename);

        void * mmap_origin = makeOperatingFile(filename);
        MMAP_ORIGIN = mmap_origin;

        cout << "mmap origin: " << mmap_origin << endl;
        dat_header *datfile = (dat_header *)((char *) mmap_origin + ROOT_OFFSET);
        cout << "ROOT OFFSET: " << ROOT_OFFSET << endl;
        cout << "datfile origin: " << datfile << endl;

        DatFile *dat = new DatFile(datfile);
        if (EXPORT) {
            dat->serialize(scene);
        } else {
            dat->print();
        }
    }

    if (EXPORT) {
        // dump to file
        cout << "writing file " << JOINT_OUTPUT_PATH << endl;
        Assimp::Exporter exporter = Assimp::Exporter();
        const aiExportFormatDesc * exportFormatDesc =
                exporter.GetExportFormatDescription(EXPORT_FORMAT);
        cout << "format: " << exportFormatDesc->id << endl;
        int error_code = exporter.Export(scene, exportFormatDesc->id, JOINT_OUTPUT_PATH);
        if (0 > error_code) {
            cout << "assimp error (";
            switch(error_code) {
                case aiReturn_FAILURE:
                    cout << "FAILURE";
                    break;
                case aiReturn_OUTOFMEMORY:
                    cout << "OUT OF MEMORY";
                    break;
                default:
                    cout << "??";
                    break;
            }
            cout << ") exporting scene" <<endl;
        }
    }

    return 0;
}

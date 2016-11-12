#include "config.hpp"
#include <cstdint>
#include <iostream>
#include <unistd.h>

#include <assimp/Exporter.hpp>

unsigned int INDENT_SIZE = 4;

uint32_t SPECIAL_SUBACTION_OFFSET = 0;
uint32_t SPECIAL_SUBACTION_COUNT = 0;
uint32_t ROOT_OFFSET = 0;
bool DETAILED_SUBACTION_PRINT = false;
bool EXPORT;
size_t EXPORT_FORMAT;
size_t NUM_FILES;

char const * JOINT_OUTPUT_PATH = "output.bvh";

// shiek offset 0x8FC8
// shiek count 0x2C

using namespace std;

static void listOutputFormats() {
    Assimp::Exporter exporter = Assimp::Exporter();
    size_t num_formats = exporter.GetExportFormatCount();

    for (size_t i=0; i<num_formats; i++) {
        const aiExportFormatDesc * exportFormatDesc = 
            exporter.GetExportFormatDescription(i);
        cout << "           " << exportFormatDesc->id << endl;
    }
}

static void assignOutputFormat(const char* s) {
    Assimp::Exporter exporter = Assimp::Exporter();
    size_t num_formats = exporter.GetExportFormatCount();

    for (size_t i=0; i<num_formats; i++) {
        const aiExportFormatDesc * desc = 
            exporter.GetExportFormatDescription(i);
        if (strcmp(desc->id, s) == 0) {
            EXPORT_FORMAT = i;
            return;
        }
    }
    cout << "unrecognized format " << s;
    cout << "output format must be one of" << endl;
    listOutputFormats();
    exit(1);
}

static void printHelp(char *pname) {
  cout << "usage:\n    " << pname << "[..args]"
       << "datfile" << endl;
  cout << "    "
       << "-x   : export instead of printing" << endl;
  cout << "    "
       << "-s n : Special Subaction Table Offset" << endl;
  cout << "    "
       << "-c n : Special Subaction Table Count" << endl;
  cout << "    "
       << "-d   : detailed subaction output" << endl;
  cout << "    "
       << "-j   : output path for joint bdf" << endl;
  cout << "    "
       << "-r   : root offset in datafile" << endl;
  cout << "    "
       << "-f   : output format when exporting, one of " << endl;
  listOutputFormats();
}

char **parseConf(int argc, char **argv) {
  char option_char;
  char *progname = argv[0];
  ios::fmtflags f(cout.flags());

  while ((option_char = getopt(argc, argv, "s:c:dxj:r:f:")) != EOF) {
    switch (option_char) {
    case 's':
      sscanf(optarg, "%x", &SPECIAL_SUBACTION_OFFSET);
      cout << "special subaction offset:" << hex << SPECIAL_SUBACTION_OFFSET
           << endl;
      break;
    case 'c':
      sscanf(optarg, "%x", &SPECIAL_SUBACTION_COUNT);
      cout << "special subaction count:" << hex << SPECIAL_SUBACTION_COUNT
           << endl;
      break;
    case 'd':
      DETAILED_SUBACTION_PRINT = true;
      break;
    case 'x':
      EXPORT= true;
      break;
    case 'j':
      JOINT_OUTPUT_PATH=optarg;
      break;
    case 'r':
      sscanf(optarg, "%x", &ROOT_OFFSET);
      break;
    case 'f':
      assignOutputFormat(optarg);
      break;
    case '?':
      printHelp(progname);
      exit(1);
    }
  }
  cout.flags(f);
  NUM_FILES = argc = optind;

  return argv + optind;
}

#include "config.hpp"
#include <cstdint>
#include <iostream>
#include <unistd.h>

unsigned int INDENT_SIZE = 4;

uint32_t SPECIAL_SUBACTION_OFFSET = 0;
uint32_t SPECIAL_SUBACTION_COUNT = 0;
bool DETAILED_SUBACTION_PRINT = false;
bool EXPORT;

char const * JOINT_OUTPUT_PATH = "output.bvh";

// shiek offset 0x8FC8
// shiek count 0x2C

using namespace std;

static void printHelp(char *pname) {
  cerr << "usage:\n    " << pname << "[..args]"
       << "datfile" << endl;
  cerr << "    "
       << "-x   : export instead of printing" << endl;
  cerr << "    "
       << "-s n : Special Subaction Table Offset" << endl;
  cerr << "    "
       << "-c n : Special Subaction Table Count" << endl;
  cerr << "    "
       << "-d   : detailed subaction output" << endl;
  cerr << "    "
       << "-j   : output path for joint bdf" << endl;
}

char *parseConf(int argc, char **argv) {
  char option_char;
  char *progname = argv[0];
  ios::fmtflags f(cout.flags());

  while ((option_char = getopt(argc, argv, "s:c:dxj:")) != EOF) {
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


    case '?':
      printHelp(progname);
      exit(1);
    }
  }
  cout.flags(f);

  return argv[optind];
}

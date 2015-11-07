#include "config.h"
#include <cstdint>
#include <iostream>
#include <unistd.h>

unsigned int INDENT_SIZE = 4;

uint32_t SPECIAL_SUBACTION_OFFSET = 0;
uint32_t SPECIAL_SUBACTION_COUNT = 0;

// shiek offset 0x8FC8
// shiek count 0x2C

using namespace std;

static void printHelp(char * pname) {
    cerr << "usage:\n    " << pname << "[..args]" << "datfile" << endl;
    cerr << "    " << "-s : Special Subaction Table Offset" << endl;
    cerr << "    " << "-s : Special Subaction Table Count" << endl;
}

char * parseConf(int argc, char ** argv) {
    char option_char;
    char * progname = argv[0];

    while ((option_char = getopt(argc, argv, "s:c:")) != EOF) {
        switch (option_char) {  
             case 's': 
                sscanf(optarg,
                   "%x", &SPECIAL_SUBACTION_OFFSET);
                cout << "special subaction offset:"
                     << hex << SPECIAL_SUBACTION_OFFSET << endl;
                break;
            case 'c':
                sscanf(optarg,
                   "%x", &SPECIAL_SUBACTION_COUNT);
                cout << "special subaction count:"
                     << hex << SPECIAL_SUBACTION_COUNT << endl;
                break;
            case '?':
                printHelp(progname);
                exit(1);
        }
    }

    return argv[optind];
}

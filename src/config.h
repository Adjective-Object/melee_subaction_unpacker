#ifndef MREADER_CONFIG
#define MREADER_CONFIG

#include <cstdint>

extern unsigned int INDENT_SIZE;

extern uint32_t SPECIAL_SUBACTION_OFFSET;
extern uint32_t SPECIAL_SUBACTION_COUNT;

char * parseConf(int argc, char ** args);

#endif

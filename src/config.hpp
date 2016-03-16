#ifndef MREADER_CONFIG
#define MREADER_CONFIG

#include <cstdint>

extern unsigned int INDENT_SIZE;

extern uint32_t SPECIAL_SUBACTION_OFFSET;
extern uint32_t SPECIAL_SUBACTION_COUNT;
extern bool DETAILED_SUBACTION_PRINT;
extern bool EXPORT;

extern char const * JOINT_OUTPUT_PATH;

char *parseConf(int argc, char **args);

#endif

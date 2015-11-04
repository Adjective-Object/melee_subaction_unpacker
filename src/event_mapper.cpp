#include "event_mapper.h"
#include <sstream>
#include <cstring>

#include <iostream>
#include <iomanip>

using namespace std;

map<EVENT_ID, unsigned int> evt_lengths{
    {TIMER_SYNC, 0x4},
    {TIMER_ASYNC, 0x4},
    {SET_LOOP, 0x4},
    {EXEC_LOOP, 0x4},
    {GOTO, 0x8},
    {RETURN, 0x4},
    {SUBROUTINE, 0x8},
    {AUTOCANCEL, 0x4},
    {ALLOW_INTERRUPT, 0x4},
    {START_SMASH_CHARGE, 0x08},

    {HITBOX, 0x14},
    {TERMINATE_ALL_COLLISSION, 0x4},
    {BODY_STATE, 0x4},
    {THROW, 0x10},
    {REVERSE_DIRECTION, 0x4},
    {UNKNOWN_FALLSPEED_MOD, 0x8},
    {AIRSTOP, 0x8},


    {GENERATE_ARTICLE, 0x4},
    {SELF_DAMAGE, 0x4},

    {GRAPHIC_EFFECT, 0x14},
    {BODY_AURA_GROUP_1, 0x8},
    {BODY_AURA_GROUP_2, 0x8},

    {SOUND_EFFECT, 0xC},
    {SOUND_EFFECT_RANDOM_SMASH, 0x4},
};



char * evt_to_str(char * buffer, unsigned char * evt) {
    ostringstream convert;
    unsigned int length = evt_lengths[(EVENT_ID) *evt];
    for(unsigned int i=0; i<length; i++) {
        convert << hex << setfill('0') << setw(2) << +evt[i];
        convert << " ";
    }
    string converted = convert.str();
    strcpy(buffer, converted.c_str());
    return buffer;
}


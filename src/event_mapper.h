#ifndef MREADER_EVT_MAPPER
#define MREADER_EVT_MAPPER

#include <cstdint>
#include <string>
#include <map>
#include "dolfs.h"

using namespace std;

// mostly from http://opensa.dantarion.com/wiki/Events_(Melee)
enum EVENT_ID {
    SUBACTION_TERMINATOR = 0x00,
    
    TIMER_SYNC = 0x04,  // wait for n frames from now
    TIMER_ASYNC = 0x08, // wait until the nth frame from
                        // subaction start
    SET_LOOP = 0x0C,
    EXEC_LOOP = 0x10,
    GOTO = 0x1C,
    RETURN = 0x18,
    SUBROUTINE = 0x14,
    AUTOCANCEL = 0x4C,
    ALLOW_INTERRUPT = 0x5c,
    START_SMASH_CHARGE = 0xE0,

    HITBOX = 0x2C,
    TERMINATE_ALL_COLLISSION = 0x40,
    BODY_STATE = 0x68,
    THROW = 0x88,
    REVERSE_DIRECTION = 0x50,
    UNKNOWN_FALLSPEED_MOD = 0x4C,
    AIRSTOP = 0x4D, // 0x4D000001 stop 0x4D000002 cancel
    UNKNWON_MOVEMENT_RELATED = 0xd8,

    GENERATE_ARTICLE = 0xAC, // ??
    SELF_DAMAGE = 0xCC, // damage or heal

    GRAPHIC_EFFECT = 0x28,
    COMBINED_GRAPHIC_SOUND = 0xDC,
    // BODY AURA EFFECTS
    // SEE http://smashboards.com/threads/changing-color-effects-in-melee.313177/page-2#post-14490878
    BODY_AURA_GROUP_1 = 0xB8,
    BODY_AURA_GROUP_2 = 0xB9,
    SOUND_EFFECT = 0x44,
    SOUND_EFFECT_RANDOM_SMASH = 0x48,

    UNKNOWN_A2 = 0xA2, // length estimated from consec. commands
                       // in PlFe.dat (len 0x4), assuming no fall
                       // through from DamageFall -> Wait1
    

    UNKNOWN_D8 = 0xD8, // length estimated from consecutive
                       // d8 commands in PlFe.dat 's WalkMiddle
                       // action

    UNKNOWN_DC = 0xDC, // likely related to landing somehow
                       // length based on the assumption that
                       // PlySeak5K_Share_ACTION_SquatRv_figatree
                       // PlySeak5K_Share_ACTION_Landing_figatree (23)
                       // PlySeak5k_Share_ACTION_Landing_figatree (24)
                       // are consecutive and non-overlapping

    UNKNOWN_D0 = 0xD0, // length estimated based on the same block as
                       // DC. Possibly some kind of control structure?
    
    UNKNOWN_20 = 0x20, // totally unknown purpose, length assumed from
    
    UNKNOWN_B4 = 0xB4, // only ever happens following a 
                       // throw command?

    UNKNOWN_C6 = 0xC6, // unknown, starts all of Seak's RapidJab
                       // commands

    UNKNOWN_7C = 0x7C, // length estimated as 0x4 based on the 
                       // assumption of 0 fallthrough in the 
                       // consecutive SquatWait actions in
                       // PlSk.dat, as well as the assumption
                       // that instruction 0xD0 is of length 4

    UNKNOWN_70 = 0x70, // based on the assumption of
                       // consecutive 0x70 commands in PlySeak's
                       // AttackLw4 and AttackHi4 commands

    UNKNOWN_A8 = 0xA8, // can't be length 4 because PlySeak Swing3
                       // calls it at offset 0x12, and if it
                       // were len 4, the next instruction
                       // would be "08 00 00 0a 2d 81 38 0d"
                       // or a HUGE timer wait
                       // assumed to be length 8

    UNKNOWN_60 = 0x60, // appears in most ItemShoot and 
                       // ItemScopeFire actions. Likely related
                       // to ammunition. Length assumed to be
                       // 4 because if it were, it would be
                       // followed by a graphic effect at
                       // almost every occurence

    UNKNOWN_A0 = 0xA0, // appears at the beginning of shoot itemA
                       // subactions. Length estimated based on
                       // breakdown of PlySeak5k ItemShoot
                       // if length 4, followed by D0, an async
                       // timer, and  some graphic effects

    UNKNOWN_8C = 0x8C, // appears in both screw damage and
                       // screw attack subactions. TODO:
                       // check if it appears in Samus's up b.
                       // all calls enocountered are of the form
                       // " 8c 00 00 00 ", so it is likely a
                       // 4-long single instruction, usually
                       // followed by a sound effect?

    UNKNOWN_94 = 0x94, // based on its use in PlySeak SpecialHi
                       // at offsets 0x14 and 0x20,
                       // it is likely a 4 long instruction that
                       // takes a single parameter
    
    UNKNOWN_03 = 0x03, // somehow related to heavy walk

    UNKNOWN_64 = 0x64, // based on consecutive DownBoundU and DownWaitU
                       // subactions

    UNKNOWN_C5 = 0xC5, // usually called as C5 FF FF FF

    UNKNOWN_0D = 0x0D, // from PlySeak Swing4

    // Things past this point are more or less shots in the dark
    UNKNOWN_29 = 0x29, // appears in all of PlFE's aereal attacks

    UNKNOWN_C4 = 0xC4, // appears in all of PlFE's aereal attacks

    UNKNOWN_2D = 0x2D, // appears paired with hitboxes, with many
                       // of the same arguments
                       // This probably means that it's a bone-tied
                       // graphic effect

    UNKNOWN_23 = 0x23, // appears in Attack12 in PlSk. NO IDEA
    UNKNOWN_6C = 0x6C, // called as 6C 00 00 00
    UNKNOWN_74 = 0x74, // called as 6C 00 00 00
    UNKNOWN_B5 = 0xB5, // called as 6C 00 00 00

    UNKNOWN_78 = 0x78, // either 0x4 or 0x10
    UNKNOWN_77 = 0x77,
    UNKNOWN_30 = 0x30,
    UNKNOWN_31 = 0x31,
    UNKNOWN_99 = 0x99,

    UNKNOWN_A4 = 0xA4, // Guess based on PlZd
    UNKNOWN_C8 = 0xC8, // Guess based on PlZd (Dash)

};

typedef struct event_descriptor {
    unsigned int length;
    string name;
} event_descriptor;

extern map<EVENT_ID, event_descriptor> evts;
extern event_descriptor EVT_UNKNOWN;

char * evt_to_str(char * buffer, unsigned char * evt);
uint32_t * str_to_evt(string & string);

unsigned char * print_action(
        int indent, 
        const DatFile * datfile,
        unsigned int offset);
 

#endif

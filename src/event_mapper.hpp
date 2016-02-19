#ifndef MREADER_EVT_MAPPER
#define MREADER_EVT_MAPPER

#include <cstdint>
#include <string>
#include <map>
#include "dolfs.hpp"

using namespace std;

// mostly from http://opensa.dantarion.com/wiki/Events_(Melee)
#define SUBACTION_TERMINATOR  0x00 
    
#define TIMER_SYNC  0x04   // wait for n frames from now
#define TIMER_ASYNC  0x08  // wait until the nth frame from
                           // subaction start
#define SET_LOOP  0x0C 
#define EXEC_LOOP  0x10 
#define GOTO  0x1C 
#define RETURN  0x18 
#define SUBROUTINE  0x14 
#define AUTOCANCEL  0x4C 
#define ALLOW_INTERRUPT  0x5c 
#define START_SMASH_CHARGE  0xE0 

#define HITBOX  0x2C 
#define TERMINATE_ALL_COLLISSION  0x40 
#define BODY_STATE  0x68 
#define THROW  0x88 
#define REVERSE_DIRECTION  0x50 
#define UNKNOWN_FALLSPEED_MOD  0x4C 
#define AIRSTOP  0x4D  // 0x4D000001 stop 0x4D000002 cancel
#define UNKNWON_MOVEMENT_RELATED  0xd8 

#define GENERATE_ARTICLE  0xAC  // ??
#define SELF_DAMAGE  0xCC  // damage or heal

#define GRAPHIC_EFFECT  0x28 
#define COMBINED_GRAPHIC_SOUND  0xDC 
        // BODY AURA EFFECTS
        // SEE http://smashboards.com/threads/changing-color-effects-in-melee.313177/page-2#post-14490878
#define BODY_AURA_GROUP_1  0xB8 
#define BODY_AURA_GROUP_2  0xB9 
#define SOUND_EFFECT  0x44 
#define SOUND_EFFECT_RANDOM_SMASH  0x48 

#define UNKNOWN_D8  0xD8  // length estimated from consecutive
                          // d8 commands in PlFe.dat 's WalkMiddle
                          // action

#define UNKNOWN_DC  0xDC  // likely related to landing somehow
                          // length based on the assumption that
                          // PlySeak5K_Share_ACTION_SquatRv_figatree
                          // PlySeak5K_Share_ACTION_Landing_figatree (23)
                          // PlySeak5k_Share_ACTION_Landing_figatree (24)
                          // are consecutive and non-overlapping

#define UNKNOWN_D0  0xD0  // length estimated based on the same block as
                      // DC. Possibly some kind of control structure?
    
#define UNKNOWN_20  0x20  // totally unknown purpose  length assumed from

#define UNKNOWN_C6  0xC6  // unknown  starts all of Seak's RapidJab
                       // commands

#define UNKNOWN_7C  0x7C  // length estimated as 0x4 based on the 
                       // assumption of 0 fallthrough in the 
                       // consecutive SquatWait actions in
                       // PlSk.dat  as well as the assumption
                       // that instruction 0xD0 is of length 4

#define UNKNOWN_70  0x70  // based on the assumption of
                       // consecutive 0x70 commands in PlySeak's
                       // AttackLw4 and AttackHi4 commands

#define UNKNOWN_A8  0xA8  // can't be length 4 because PlySeak Swing3
                       // calls it at offset 0x12  and if it
                       // were len 4  the next instruction
                       // would be "08 00 00 0a 2d 81 38 0d"
                       // or a HUGE timer wait
                       // assumed to be length 8

#define UNKNOWN_60  0x60  // appears in most ItemShoot and 
                       // ItemScopeFire actions. Likely related
                       // to ammunition. Length assumed to be
                       // 4 because if it were  it would be
                       // followed by a graphic effect at
                       // almost every occurence

#define UNKNOWN_A0  0xA0  // appears at the beginning of shoot itemA
                       // subactions. Length estimated based on
                       // breakdown of PlySeak5k ItemShoot
                       // if length 4  followed by D0  an async
                       // timer  and  some graphic effects

#define UNKNOWN_8C  0x8C  // appears in both screw damage and
                       // screw attack subactions. TODO:
                       // check if it appears in Samus's up b.
                       // all calls enocountered are of the form
                       // " 8c 00 00 00 "  so it is likely a
                       // 4-long single instruction  usually
                       // followed by a sound effect?

#define UNKNOWN_94  0x94  // based on its use in PlySeak SpecialHi
                       // at offsets 0x14 and 0x20 
                       // it is likely a 4 long instruction that
                       // takes a single parameter

#define UNKNOWN_64  0x64  // based on consecutive DownBoundU and
                       // DownWaitU subactions. 
                       // Toggle Form Appears in lots of
                       // missed tech animations (DownBoundU/D)
                       // 64 00 00 0 -> .. -> 64 00 00 01
                       // 
                       // considered actions:
                       //   - align to terrain
                       //   - invuln toggle?
                       //
                       // 64 00 00 02 
                       // appears on 
                       //   frame 6 of marth's upb
                       //   frame 6 of peach's upb
                       //   frame 7 of mario's upb
                       //   frame 9 of luigi's upb
                       //   frame 0 of kirby's SpecialHi
                       //   frame 0 and 52 of kiby's ThrowHi
                       //   frame 0 of kirby's ThrowB
                       //   frame 0 and 45 of kirby's ThrowB
                       //   frame 10 of roy's upb

#define UNKNOWN_0D  0x0D  // from PlySeak Swing4

#define UNKNOWN_E8  0xE8  // at least 0x16 long, based on Gannon's
                       // second instruction in AttackHi3. This
                       // is the first place I saw it..


#define UNKNOWN_05  0x05  // has to be length 0x16 based on Kirby's 
                       // neutral special

#define UNKNOWN_90  0x90  // appears a _LOT_ in bowser/Koopa's datfile

// Things past this point are more or less shots in the dark
#define UNKNOWN_29  0x29  // appears in all of PlFE's aereal attacks

#define UNKNOWN_C4  0xC4  // appears in all of PlFE's aereal attacks

#define UNKNOWN_6C  0x6C  // called as 6C 00 00 00
#define UNKNOWN_74  0x74 

#define UNKNOWN_78  0x78  // either 0x4 or 0x10
#define UNKNOWN_30  0x30 

#define UNKNOWN_A4  0xA4  // Guess based on PlZd
#define UNKNOWN_C8  0xC8  // Guess based on PlZd (Dash)
#define UNKNOWN_B4  0xB4 

#define UNKNOWN_34  0x34 
#define UNKNOWN_98  0x98 
#define UNKNOWN_D4  0xD4 

#define UNKNOWN_38  0x38  // unique to jigg's and yoshi's nspecials?

#define UNKNOWN_54  0x54  // appears in ganon / captain's downspecials
                       // as well as kirby's Nspecial

#define UNKNOWN_9C  0x9C  // appears in kirby's adopted neutral
                       // specials, followed by an aura usually.
/*
typedef struct event_descriptor {
    string name;
    vector<arg_descriptor> args;
} event_descriptor;

extern map<unsigned char, event_descriptor> evts;
extern event_descriptor EVT_UNKNOWN;
*/

void initializeEventMap(string filepath);

char * evt_to_str(char * buffer, unsigned char * evt);
uint32_t * str_to_evt(string & string);

unsigned char * print_action(
        int indent, 
        const DatFile * datfile,
        unsigned int offset);
 

#endif

#include "event_mapper.h"
#include "config.h"
#include "helpers.h"
#include "dolfs.h"
#include <sstream>
#include <cstring>

#include <iostream>
#include <iomanip>

using namespace std;

map<EVENT_ID, event_descriptor> evts{
    {TIMER_SYNC, {0x4, "timer_sync"}},
    {TIMER_ASYNC, {0x4, "timer_async"}},
    {SET_LOOP, {0x4, "setloop"}},
    {EXEC_LOOP, {0x4, "execloop"}},
    {GOTO, {0x8, "goto"}},
    {RETURN, {0x4, "return"}},
    {SUBROUTINE, {0x8, "subroutine"}},
    {AUTOCANCEL, {0x4, "autocancel"}},
    {ALLOW_INTERRUPT, {0x4, "iasa"}},
    {START_SMASH_CHARGE, {0x08, "smashchrg"}},

    {HITBOX, {0x14, "hitbox"}},
    {TERMINATE_ALL_COLLISSION, {0x4, "endcolls"}},
    {BODY_STATE, {0x4, "bodystate"}},
    {THROW, {0x10, "throw"}},
    {REVERSE_DIRECTION, {0x4, "reverse"}},
    {UNKNOWN_FALLSPEED_MOD, {0x8, "fallmod?"}},

    {GENERATE_ARTICLE, {0x4, "gen_article"}},
    {SELF_DAMAGE, {0x4, "selfhp"}},

    {GRAPHIC_EFFECT, {0x14, "graphic"}},
    {BODY_AURA_GROUP_1, {0x8, "bodyaura_1"}},
    {BODY_AURA_GROUP_2, {0x8, "bodyaura_2"}},

    {SOUND_EFFECT, {0xC, "sfx"}},
    {SOUND_EFFECT_RANDOM_SMASH, {0x4, "smash_sfx"}},
    {SUBACTION_TERMINATOR, {0x4, "end"}},
    
    // related to actionability? appears in a lot of 
    // effect-target / hit animation states
    // could also be related to character rotation
    // ADDENDUM: appears in wait and win animations
    // on timers a bunch -- texture swapping?
    {UNKNOWN_A2, {0x4, "texswap?"}},
    {UNKNOWN_D0, {0x4, "unknown"}},
    {UNKNOWN_D8, {0x10,"unknown"}},
    {UNKNOWN_DC, {0xC, "unknown"}},
    {UNKNOWN_20, {0x4, "unknown"}},
    {UNKNOWN_B4, {0xC, "unknown"}},
    {UNKNOWN_C6, {0x8, "unknown"}},
    {UNKNOWN_7C, {0x4, "unknown"}},
    {UNKNOWN_70, {0x4, "unknown"}},
    {UNKNOWN_A8, {0x8, "unknown"}},

    {UNKNOWN_60, {0x4, "shootitem1?"}},
    {UNKNOWN_A0, {0x4, "shootitem2?"}},
    {UNKNOWN_8C, {0x4, "screwatk?"}},

    {UNKNOWN_94, {0x4,  "unknown"}},
    {UNKNOWN_03, {0x10, "unknown"}},
    {UNKNOWN_C4, {0x4,  "unknown"}},
    {UNKNOWN_2D, {0x14, "bonegfx?"}},
    {UNKNOWN_23, {0x8,  "unknown"}},
    {UNKNOWN_64, {0x4,  "unknown"}},
    {UNKNOWN_C5, {0x4,  "unknown"}},
    {UNKNOWN_29, {0x4,  "unknown"}},
    {UNKNOWN_6C, {0x4,  "unknown"}},
    {UNKNOWN_74, {0x4,  "unknown"}},
    {UNKNOWN_B5, {0x4,  "unknown"}},

    {UNKNOWN_78, {0x4,  "unknown"}},
    {UNKNOWN_77, {0x4,  "unknown"}},
    {UNKNOWN_0D, {0xC,  "unknown"}},
    {UNKNOWN_30, {0x4,  "unknown"}},
    {UNKNOWN_31, {0x4,  "unknown"}},
    {UNKNOWN_99, {0x18, "unknown"}},
    
    {UNKNOWN_A4, {0x4, "unknown"}},
    {UNKNOWN_C8, {0x4, "unknown"}},

    {UNKNOWN_4E, {0x4, "unknown"}},
    {UNKNOWN_4F, {0x4, "unknown"}},
    
    
    {UNKNOWN_34, {0x4, "unknown"}},
    {UNKNOWN_38, {0x4, "unknown"}},
    {UNKNOWN_E8, {0x10, "unknown"}},

    {UNKNOWN_05, {0x10, "unknown"}},
    {UNKNOWN_90, {0x4, "unknown"}},
    {UNKNOWN_01, {0x8, "unknown"}},
    {UNKNOWN_71, {0x4, "unknown"}},

    // conflicts with itaru's definition of airstop (0x8 long)
    // but without it, PlyEmblem's SpecialLw falls into 
    // SpecialLwHit
    {AIRSTOP, {0x4, "airstop?"}},


};

event_descriptor EVT_UNKNOWN = {0x1, "UNKNOWN"};


char * evt_to_str(char * buffer, unsigned char * evt) {
    ostringstream convert;
    event_descriptor e;

    string color;

    if (evts.count((EVENT_ID) *evt)) {
        e = evts[(EVENT_ID) *evt];
        color = BLUE;
    } else {
        e = EVT_UNKNOWN;
        color = RED;
    }

    convert << color << '|' << setfill(' ') << setw(11) << e.name << " ";
    convert << hex << setfill('0') << setw(2) << +evt[0] << " " << RESET;
    for(unsigned int i=1; i<e.length; i++) {
        convert << hex << setfill('0') << setw(2) << +evt[i];
        convert << " ";
    }
    string converted = convert.str();
    strcpy(buffer, converted.c_str());
    return buffer;
}

unsigned char * print_action(
        int indent, 
        const DatFile * datfile,
        unsigned int offset) {

    unsigned char * event = (unsigned char *)
        (datfile->dataSection + offset);
    
    if (*event == SUBACTION_TERMINATOR)
        return NULL;
    
    string ind = string(indent * INDENT_SIZE, ' '); 
    ios::fmtflags f( cout.flags() ); 

    cout << ind << GREEN << "offset: " << RESET << "0x" << hex << offset << endl;

    char s[256];
    while(*event != SUBACTION_TERMINATOR && *event != RETURN) {
        // if not a command of known lengthm, 
        // dump the following memory
        if (!evts.count((EVENT_ID) *event)) {
            cout << ind << evt_to_str(s, event) << endl; 
            cout_hex(indent + 2, event, 12, 4);
            cout << ind << GREEN << "endoff: " << RESET << "0x"
                 << (intptr_t) (event - (unsigned char *)datfile->dataSection) << endl;
            return event;
        }
 
        unsigned int length = evts[(EVENT_ID) *event].length;
        cout << ind << evt_to_str(s, event) << endl; 
        
        if (*event == SUBROUTINE || *event == GOTO) {

            uint32_t subroutine_offset = 0;
            for (int i=1; i<8; i++) {
                subroutine_offset = subroutine_offset << 8;
                subroutine_offset = 
                    subroutine_offset | event[i];
            }

            if (subroutine_offset != offset) {
                unsigned char * exitCommand =
                    print_action(indent +1, datfile, 
                        subroutine_offset);
                if (*event == GOTO || 
                        (exitCommand != NULL && 
                            *exitCommand == SUBACTION_TERMINATOR)) {
                    cout << ind << GREEN << "endoff: " << RESET << "0x"
                         << (intptr_t) (event - (unsigned char *)datfile->dataSection) << endl;
                    return event;
                }
            } else {
                cout << ind << RED << "[RECURSION NOT ALLOWED "
                     << RESET << "0x" << subroutine_offset 
                     << RED << "]" << RESET << endl;
                cout << ind << GREEN << "endoff: " << RESET << "0x"
                     << (intptr_t) (event - (unsigned char *)datfile->dataSection) << endl;
                return NULL;
            }
        }

        event += length;
    }
    cout << ind << evt_to_str(s, event) << endl; 
    cout << ind << GREEN << "endoff: " << RESET << "0x"
         << (intptr_t) (event - (unsigned char *)datfile->dataSection) << endl;

    cout.flags( f );
    return event;
}




#include "event_mapper.h"
#include "config.h"
#include "helpers.h"
#include "dolfs.h"
#include <sstream>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <utility>

using namespace std;

map<unsigned char, event_descriptor> evts {
    // argument lengths are specified in half-byte multiples (4 bits)
    {0x00, {"end", {{RAW, 6}} }},
    {TIMER_SYNC,  {"timer_sync", {{RAW, 6}} }},
    {TIMER_ASYNC, {"timer_async", {{RAW, 6}} }},
    {SET_LOOP, {"setloop", {{RAW, 6}} }},
    {EXEC_LOOP, {"execloop", {{RAW, 6}} }},
    {GOTO, {"goto", {{RAW, 14}} }},
    {RETURN, {"return", {{RAW, 6}} }},
    {SUBROUTINE, {"subroutine", {{RAW, 14}} }},
    {AUTOCANCEL, {"autocancel", {{RAW, 6}} }},
    {ALLOW_INTERRUPT, {"iasa", {{RAW, 6}} }},
    {START_SMASH_CHARGE, {"smashchrg", {{RAW, 14}} }},

    {HITBOX, {"hitbox", {{RAW, 38}} }},
    {TERMINATE_ALL_COLLISSION, {"endcolls", {{RAW, 6}} }},
    {BODY_STATE, {"bodystate", {{RAW, 6}} }},
    {THROW, {"throw", {{RAW, 30}} }},
    {REVERSE_DIRECTION, {"reverse", {{RAW, 6}} }},
    {UNKNOWN_FALLSPEED_MOD, {"fallmod?", {{RAW, 20}} }},

    {GENERATE_ARTICLE, {"gen_article", {{RAW, 6}} }},
    {SELF_DAMAGE, {"selfhp", {{RAW, 6}}}},

    {GRAPHIC_EFFECT, {"graphic", {{RAW, 38}} }},
    {BODY_AURA_GROUP_1, {"bodyaura_1", {{RAW, 14}} }},
    {BODY_AURA_GROUP_2, {"bodyaura_2", {{RAW, 14}} }},

    {SOUND_EFFECT, {"sfx", {{RAW, 22}} }},
    {SOUND_EFFECT_RANDOM_SMASH, {"smash_sfx", {{RAW, 6}} }},

    {0xD0, {"unknown_D0", {{RAW, 6}}  }},
    {0xD8, {"unknown_D8", {{RAW, 22}} }},
    {0xDC, {"unknown_DC", {{RAW, 22}} }},
    {0x20, {"unknown_20", {{RAW, 6}}  }},
    {0xB4, {"unknown_B4", {{RAW, 22}}  }},
    {0xC6, {"unknown_C6", {{RAW, 14}} }},
    {0x7C, {"unknown_7C", {{RAW, 6}} }},
    {0x70, {"unknown_70", {{RAW, 6}} }},
    {0xA8, {"unknown_A8", {{RAW, 14}} }},

    {0x60, {"shootitem1?",{{RAW, 6}} }},
    {0xA0, {"texswap?",   {{RAW, 6}} }},
    {0x8C, {"unknown_8c", {{RAW, 6}} }},

    {0x94, {"unknown_94", {{RAW, 6}} }},
    {0xC4, {"unknown_C4", {{RAW, 6}} }},
    {0x64, {"unknown_64", {{RAW, 6}} }},
    {0x29, {"unknown_29", {{RAW, 6}} }},
    {0x6C, {"unknown_6C", {{RAW, 6}} }},
    {0x74, {"unknown_74", {{RAW, 6}} }},

    {0x78, {"unknown_78", {{RAW, 6}} }},
    {0x0D, {"unknown_0D", {{RAW, 10}} }},
    {0x30, {"unknown_30", {{RAW, 6}} }},
    {0xA4, {"unknown_A4", {{RAW, 6}} }},
    {0xC8, {"unknown_C8", {{RAW, 6}} }},
    {0x34, {"unknown_34", {{RAW, 6}} }},
    {0x38, {"roll_38?",   {{RAW, 6}} }},
    {0xE8, {"unknown_E8", {{RAW, 30}} }},
    {0x90, {"unknown_90", {{RAW, 6}} }},
    {0x98, {"unknown_98", {{RAW, 38}} }},
    {0x54, {"unknown_54", {{RAW, 6}} }},
    {0xD4, {"unknown_D4", {{RAW, 6}} }},
    {0x9C, {"unknown_9C", {{RAW, 30}} }},

    // conflicts with itaru's definition of airstop (0x8 long)
    // but without it, PlyEmblem's SpecialLw falls into 
    // SpecialLwHit
    {AIRSTOP, {"airstop?", {{RAW, 14}} }},
};


event_descriptor EVT_UNKNOWN = {"UNKNOWN", {{}} };

void initializeEventMap(string filepath) {

}

char * evt_to_str(char * buffer, unsigned char * evt) {
    ostringstream convert;
    event_descriptor e;
    string color;

    unsigned char opcode = (*evt & 0xfc);
    if (evts.count(opcode)) {
        e = evts[opcode];
        color = BLUE;
    } else {
        e = EVT_UNKNOWN;
        color = RED;
    }

    convert << color << '|' << setfill(' ') << setw(11) << e.name << " ";
    convert << hex << setfill('0') << setw(2) << +evt[0] << " " << RESET;

    unsigned int current_step = 2; // start just past the opcode
    for (unsigned int i=0; i< e.args.size(); i++) {
        
        unsigned long argument = 0;
        unsigned int arg_start = current_step;
        arg_descriptor arg = e.args[i];

        while(current_step - arg_start < arg.length) {
            unsigned char current = 
                (current_step % 2 == 0)
                    ? (evt[current_step / 2] & 0xf0) >> 4
                    : evt[current_step / 2] & 0x0f;
            argument = (argument << 4) | current;
            current_step++;
        }

        switch(arg.type) {
            case INT:
                convert << dec << argument;
                break;
            case FLOAT:
                convert << (float) argument;
                break; 
            default :
            case RAW:
                for(int i=(current_step - arg_start) / 2 - 1; i >= 0; i--) {
                    convert << setw(2) << hex
                            << ((argument >> (i * 8)) & 0xff);
                    if (i != 0) {
                         convert << " ";
                    }
                }
                break;
        }
    
    }
    
    string converted = convert.str();
    strcpy(buffer, converted.c_str());
    return buffer;
}

unsigned int evt_length(unsigned char opcode) {
    unsigned int len = 2;
    vector<arg_descriptor> * args = &(evts[opcode].args);
    for (unsigned int i=0; i<args->size(); i++) {
        len += (*args)[i].length;
    }
    return len/2;
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
    for(unsigned char opcode = 0xfc & *event;
            opcode != SUBACTION_TERMINATOR && opcode != RETURN;
            opcode = 0xfc & *event) {

        // if not a command of known length,
        // dump the following memory
        if (!evts.count(opcode)) {
            cout << ind << evt_to_str(s, event) << endl; 
            cout << ind << "     opcode: "
                 << setw(2) << hex << +opcode << endl;
            cout_hex(indent + 2, event, 12, 4);
            cout << ind << GREEN << "endoff: " << RESET << "0x"
                 << (intptr_t) (event - (unsigned char *)datfile->dataSection) << endl;
            return event;
        }
 
        unsigned int length = evt_length(opcode);
        cout << ind << evt_to_str(s, event) << endl; 
        
        if (opcode == SUBROUTINE || opcode == GOTO) {

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
                if (opcode == GOTO || 
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




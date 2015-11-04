#ifndef MREADER_EVT_MAPPER
#define MREADER_EVT_MAPPER

#include <cstdint>
#include <string>
#include <map>

using namespace std;

enum EVENT_ID {
    SUBACTION_TERMINATOR = 0x00,
    
    TIMER_SYNC = 0x04,
    TIMER_ASYNC = 0x08,
    SET_LOOP = 0x0C,
    EXEC_LOOP = 0x10,
    GOTO = 0x14,
    RETURN = 0x18,
    SUBROUTINE = 0x1C,
    AUTOCANCEL = 0x4C,
    ALLOW_INTERRUPT = 0x5c,
    START_SMASH_CHARGE = 0xE0,

    HITBOX = 0x2C,
    TERMINATE_ALL_COLLISSION = 0x40,
    BODY_STATE = 0x04,
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
};

extern map<EVENT_ID, unsigned int> evt_lengths;

char * evt_to_str(char * buffer, unsigned char * evt);
uint32_t * str_to_evt(string & string);

#endif

#include "event_mapper.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include "dolfs.hpp"
#include <sstream>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <utility>

extern "C" {
#include "binscript.h"
}

using namespace std;
/*
map<unsigned char, event_descriptor> evts {
    // argument lengths are specified in bits
    {0x00, {"end", {{"?", RAW, 26}} }},
    {TIMER_SYNC,  {"timer_sync", {{"?", RAW, 26}} }},
    {TIMER_ASYNC, {"timer_async", {{"?", RAW, 26}} }},
    {SET_LOOP, {"setloop", {{"?", RAW, 26}} }},
    {EXEC_LOOP, {"execloop", {{"?", RAW, 26}} }},
    {GOTO, {"goto", {{"?", RAW, 58}} }},
    {RETURN, {"return", {{"?", RAW, 26}} }},
    {SUBROUTINE, {"subroutine", {{"?", RAW, 58}} }},
    {AUTOCANCEL, {"autocancel", {{"?", RAW, 26}} }},
    {ALLOW_INTERRUPT, {"iasa", {{"?", RAW, 26}} }},
    {START_SMASH_CHARGE, {"smashchrg", {{"?", RAW, 58}} }},

    {HITBOX, {"hitbox", {
                     {"id", INT, 3}, // hitbox id
                     {"?1", RAW, 5}, // unknown 1
                     {"bone", INT, 7}, // bone attachment
                     {"?2", RAW, 2},  // unknown 2
                     {"dmg", INT, 9},  // damage
                     {"size", FLOAT, 16}, // size
                     {"z", FLOAT, 16}, // zoff
                     {"y", FLOAT, 16}, // yoff
                     {"x",FLOAT, 16}, // xoff
                     {"angle", FLOAT, 9}, // angle
                     {"growth", FLOAT, 9}, // kb growth
                     {"weight_dep", FLOAT, 9}, // weight dep. set kb
                     {"?3", RAW, 3}, // unknown 3
                     {"hitbox_inter", HEX, 2}, // hitbox interaction
                     {"base_kb", FLOAT, 9}, // base kb
                     {"elem", HEX, 5}, // Element
                     {"?4", RAW, 1}, // Unknown 4
                     {"shielddmg", FLOAT, 7}, // shield dmg
                     {"sfx", HEX, 8}, // sound effect
                     {"hurtbox_inter", HEX, 2}, // hurtbox interaction
    }}},

    {TERMINATE_ALL_COLLISSION, {"endcolls", {{"?", RAW, 26}} }},
    {BODY_STATE, {"bodystate", {{"?", RAW, 26}} }},
    {THROW, {"throw", {{"?", RAW, 122}} }},
    {REVERSE_DIRECTION, {"reverse", {{"?", RAW, 26}} }},
    {UNKNOWN_FALLSPEED_MOD, {"fallmod?", {{"?", RAW, 82}} }},

    {GENERATE_ARTICLE, {"gen_article", {{"?", RAW, 26}} }},
    {SELF_DAMAGE, {"selfhp", {{"?", RAW, 26}}}},

    {GRAPHIC_EFFECT, {"graphic", {{"?", RAW, 154}} }},
    {BODY_AURA_GROUP_1, {"bodyaura_1", {{"?", RAW, 58}} }},
    {BODY_AURA_GROUP_2, {"bodyaura_2", {{"?", RAW, 58}} }},

    {SOUND_EFFECT, {"sfx", {{"?", RAW, 90}} }},
    {SOUND_EFFECT_RANDOM_SMASH, {"smash_sfx", {{"?", RAW, 26}} }},

    {0xD0, {"unknown_D0", {{"?", RAW, 26}}  }},
    {0xD8, {"unknown_D8", {{"?", RAW, 90}} }},
    {0xDC, {"unknown_DC", {{"?", RAW, 90}} }},
    {0x20, {"unknown_20", {{"?", RAW, 26}}  }},
    {0xB4, {"unknown_B4", {{"?", RAW, 90}}  }},
    {0xC6, {"unknown_C6", {{"?", RAW, 58}} }},
    {0x7C, {"unknown_7C", {{"?", RAW, 26}} }},
    {0x70, {"unknown_70", {{"?", RAW, 26}} }},
    {0xA8, {"unknown_A8", {{"?", RAW, 58}} }},

    {0x60, {"shootitem1?",{{"?", RAW, 26}} }},
    {0xA0, {"texswap?",   {{"?", RAW, 26}} }},
    {0x8C, {"unknown_8c", {{"?", RAW, 26}} }},

    {0x94, {"unknown_94", {{"?", RAW, 26}} }},
    {0xC4, {"unknown_C4", {{"?", RAW, 26}} }},
    {0x64, {"unknown_64", {{"?", RAW, 26}} }},
    {0x29, {"unknown_29", {{"?", RAW, 26}} }},
    {0x6C, {"unknown_6C", {{"?", RAW, 26}} }},
    {0x74, {"unknown_74", {{"?", RAW, 26}} }},

    {0x78, {"unknown_78", {{"?", RAW, 26}} }},
    {0x0D, {"unknown_0D", {{"?", RAW, 42}} }},
    {0x30, {"unknown_30", {{"?", RAW, 26}} }},
    {0xA4, {"unknown_A4", {{"?", RAW, 26}} }},
    {0xC8, {"unknown_C8", {{"?", RAW, 26}} }},
    {0x34, {"unknown_34", {{"?", RAW, 26}} }},
    {0x38, {"roll_38?",   {{"?", RAW, 26}} }},
    {0xE8, {"unknown_E8", {{"?", RAW, 122}} }},
    {0x90, {"unknown_90", {{"?", RAW, 26}} }},
    {0x98, {"unknown_98", {{"?", RAW, 154}} }},
    {0x54, {"unknown_54", {{"?", RAW, 26}} }},
    {0xD4, {"unknown_D4", {{"?", RAW, 26}} }},
    {0x9C, {"unknown_9C", {{"?", RAW, 122}} }},

    // conflicts with itaru's definition of airstop (0x8 long)
    // but without it, PlyEmblem's SpecialLw falls into
    // SpecialLwHit
    {AIRSTOP, {"airstop?", {{"?", RAW, 14}} }},
};

event_descriptor EVT_UNKNOWN = {"UNKNOWN", {{}} };
*/

language_def melee_lang;
void initializeEventMap(string filepath) {
  FILE *f = fopen(filepath.c_str(), "r");
  detailed_parse_error *p =
      parse_language_from_file(&melee_lang, f, filepath.c_str());

  if (p != NULL) {
    cout << "encountered error while parsing" << filepath << endl;
    print_err(p);
    exit(1);
  }

  fclose(f);
}

string indent_function_call(string call, size_t base_indent,
                            size_t columnwidth) {
  size_t indent = call.find_first_of('(') + 1;

  string out = call.substr(0, indent);
  call = call.substr(indent);

  size_t currentLineLength = indent;
  while (call.length() > 0) {
    // cout << call << endl;
    // cout << out  << endl;

    size_t nextSpace = call.find_first_of(' ');
    if (nextSpace == string::npos)
      nextSpace = call.length();
    else
      nextSpace += 1;

    if (nextSpace + currentLineLength > columnwidth) {
      out += "\n";
      out += string(base_indent + indent, ' ');
      currentLineLength = indent;
    }

    out += call.substr(0, nextSpace);
    call = call.substr(nextSpace);
    currentLineLength += nextSpace;
  }
  return out;
}

unsigned char *print_action(int indent, const DatFile *datfile,
                            unsigned int offset) {

  unsigned char *event = (unsigned char *)(datfile->dataSection + offset);

  if (*event == SUBACTION_TERMINATOR)
    return NULL;

  string ind = string(INDENT_SIZE * indent, ' ');
  ios::fmtflags f(cout.flags());

  cout << ind << GREEN << "offset: " << RESET << "0x" << hex << offset << endl;
  string *evtname = new string("<event @ offset ... >");
  binscript_consumer *action_conv =
      binscript_mem_consumer(&melee_lang, event, evtname->c_str(), BIN2SCRIPT);
  consumer_set_size(action_conv, MANUAL_CUTOFF, 0);

  cout << ind << "first value:" << setfill('0') << setw(2) << hex
       << (int)(*event) << endl;

  bool exited_safely = false;

  char buffer[2048];
  for (function_call *c = binscript_next(action_conv); c != NULL;
       c = binscript_next(action_conv)) {

    std::stringstream stream;
    stream << setfill('0') << setw(2) << std::hex
           << (c->defn->function_binary_value
               << melee_lang.function_name_bitshift);
    std::string fnname(stream.str());

    string_encode_function_call(buffer, c);

    string s = fnname + " " + string(buffer);
    string call = indent_function_call(s, INDENT_SIZE * indent, 50);
    cout << ind << call << endl;

    // exit current scope on 'exit' or 'goto' calls
    if (strcmp(c->defn->name, "return") == 0 ||
        strcmp(c->defn->name, "exit") == 0) {
      free_call(c);
      exited_safely = true;
      break;
    }

    // intercept gotos
    bool isgoto = 0 == strcmp(c->defn->name, "goto");
    bool issubroutine = 0 == strcmp(c->defn->name, "subroutine");
    if (isgoto || issubroutine) {
      // binscripter hex type is raw data (does not alter endianness)
      // we want to get the last argument (the address), so we extract it
      // and switch the endianness if it differs from the host
      size_t last_arg_index = c->defn->argc - 1;
      long int jmp_target = *((long int *)c->args[last_arg_index]);
      if (BS_ENDIAN_MATCH((&melee_lang))) {
        swap_endian_on_field(
            &jmp_target,
            bits2bytes(c->defn->arguments[last_arg_index]->bitwidth));
      }

      if (jmp_target != offset) {
        print_action(indent + 1, datfile, jmp_target);
      } else {
        cout << ind << RED << "[RECURSION NOT ALLOWED]" << RESET << endl;
      }

      if (isgoto) {
        free_call(c);
        exited_safely = true;
        break;
      }
    }

    free_call(c);
  }

  // if not exiting on a 'return' or 'exit', shit urself
  if (!exited_safely) {
    cout_hex(indent + 2, event, 12, 4);
    cout << ind << GREEN << "endoff: " << RESET << "0x"
         << (intptr_t)(event - (unsigned char *)datfile->dataSection) << endl;
    return event;
  }

  cout.flags(f);
  return event;
}

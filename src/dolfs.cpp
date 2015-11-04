#include "dolfs.h"
#include "macros.h"
#include "event_mapper.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <cstring>

#include "helpers.h"

using namespace std;

DatFile::DatFile(dat_header * header) : header(header) {
    // Fix the endianness on the header itself
    fix_endianness(header, sizeof(dat_header), sizeof(uint32_t)); 
    
    // Use the header to get references to the rest of the sections
    // of the file
    this->dataSection = (char *)(header + 1);
    this->offsetTable = (uint32_t *)
        (dataSection + header->dataSectionSize);
    this->rootList= (dat_root_node *) 
        (this->offsetTable + header->offsetTableCount);
    this->stringTable = (char *) 
        (this->rootList+ header->rootCountA + header->rootCountB);

    // Fix endianness of the offset table and node list
    // You can't fly jets if you're colourblind
    fix_endianness(offsetTable, 
        sizeof(uint32_t) * header->offsetTableCount, 4);
    fix_endianness(rootList, 
        sizeof(dat_root_node) * (header->rootCountA + header->rootCountB), 4);

    // step into data section and try to initialize control structures for
    // the contents, and add them to the children map;
    children = map<string, DataProxy *>();

    unsigned int i;
    for (i = 0; i < header->rootCountA + header->rootCountB; i++) {
        string name = stringTable + rootList[i].stringTableOffset;
        void * target = dataSection + rootList[i].dataSectionOffset;
        if (memcmp("ftData", name.c_str(), 6) == 0) {
            children[name] = new FtData(this, (ftdata_header *) target);
        } else {
            children[name] = new AnonymousData(target);
        }
    }
}

void DatFile::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind <<"addr of root list: " << this->rootList << endl;

    cout << ind << "root count A: " << header->rootCountA << endl;
    cout << ind << "root count B: " << header->rootCountB << endl;

    unsigned int root_ct = header->rootCountA + header->rootCountB;
    cout << ind << "total root count: " << root_ct << endl;

    map<string, DataProxy *>::iterator iter;
    for (iter = children.begin(); iter != children.end(); ++iter) {
        cout << ind << iter->first << endl;
        iter->second->print(indent + 1);
    }

}


FtData::FtData(const DatFile * datfile, ftdata_header * ftheader) {
    this->datfile = datfile;
    this->ftheader = ftheader;
    fix_endianness(ftheader, sizeof(ftdata_header), 4);
    
    // initialize the attributes & subactions
    this->attributes = new FtDataAttributes(
            datfile, 
            (ftdata_attribute_table *) 
                (datfile->dataSection + ftheader->attributesStart));

    ftdata_subaction_header * headers = 
        (ftdata_subaction_header *) 
            (datfile->dataSection + ftheader->subactionsStart);

    this->subactions = vector<FtDataSubaction *>(NUM_SUBACTIONS);
    for(unsigned int i=0; i<NUM_SUBACTIONS; i++) {
       subactions[i] = new FtDataSubaction(datfile, i, &(headers[i]));
    }
}

void FtData::print(int indent /*= 0*/) {
    string ind(indent * INDENT_SIZE, ' ');

    // print attribute table
    cout << ind << "[attribute_table]" << endl;
    this->attributes->print(indent + 1);

    // print subactions
    cout << ind << "[subactions]" << endl;
    for (size_t i=0; i<NUM_SUBACTIONS; i++) {
        this->subactions[i]->print(indent + 1);
    }
}


FtDataAttributes::FtDataAttributes(const DatFile * datfile,
        ftdata_attribute_table * attribute_table) {
    this->datfile = datfile;
    this->attributes = attribute_table;
    fix_endianness(attribute_table, sizeof(ftdata_attribute_table), 4);
}

void FtDataAttributes::print(int indent /*=0*/) {
    string ind(indent * INDENT_SIZE, ' ');

    cout << ind << "[attribute_table]" << endl;
    cout << ind << "walk_initial_velocity =             " << attributes->walk_initial_velocity << endl;
    cout << ind << "walk_acceleration =                 " << attributes->walk_acceleration << endl;
    cout << ind << "walk_maximum_velocity =             " << attributes->walk_maximum_velocity << endl;
    cout << ind << "walk_animation_speed =              " << attributes->walk_animation_speed << endl;
    cout << ind << "walk_mid_point =                    " << attributes->walk_mid_point << endl;
    cout << ind << "fast_walk_min =                     " << attributes->fast_walk_min << endl;
    cout << ind << "friction =                          " << attributes->friction << endl;
    cout << ind << "dash_initial_velocity =             " << attributes->dash_initial_velocity << endl;
    cout << ind << "stopturn_initial_velocity =         " << attributes->stopturn_initial_velocity << endl;
    cout << ind << "run_initial_velocity =              " << attributes->run_initial_velocity << endl;
    cout << ind << "run_max_velocity =                  " << attributes->run_max_velocity << endl;
    cout << ind << "run_animation_scaling =             " << attributes->run_animation_scaling << endl;
    cout << ind << "run_acceleration =                  " << attributes->run_acceleration << endl;
    cout << ind << "unknown0x34 =                       " << attributes->unknown0x34 << endl;
    cout << ind << "jump_startup_lag =                  " << attributes->jump_startup_lag << endl;
    cout << ind << "jump_h_initial_velocity =           " << attributes->jump_h_initial_velocity << endl;
    cout << ind << "jump_v_initial_velocity =           " << attributes->jump_v_initial_velocity << endl;
    cout << ind << "ground_air_jump_momentum_mult =     " << attributes->ground_air_jump_momentum_mult << endl;
    cout << ind << "jump_h_max_velocity =               " << attributes->jump_h_max_velocity << endl;
    cout << ind << "shorthop_v_initial_velocity =       " << attributes->shorthop_v_initial_velocity << endl;
    cout << ind << "air_jump_multiplier =               " << attributes->air_jump_multiplier << endl;
    cout << ind << "unknown0x54 =                       " << attributes->unknown0x54 << endl;
    cout << ind << "number_of_jumps =                   " << attributes->number_of_jumps << endl;
    cout << ind << "gravity =                           " << attributes->gravity << endl;
    cout << ind << "terminal_velocity =                 " << attributes->terminal_velocity << endl;
    cout << ind << "aerial_mobility =                   " << attributes->aerial_mobility << endl;
    cout << ind << "aerial_stopping_mobility =          " << attributes->aerial_stopping_mobility << endl;
    cout << ind << "max_aerial_h_velocity =             " << attributes->max_aerial_h_velocity << endl;
    cout << ind << "air friction =                      " << attributes->air_friction << endl;
    cout << ind << "fast_fall_terminal_velocity =       " << attributes->fast_fall_terminal_velocity << endl;
    cout << ind << "unknown0x78 =                       " << attributes->unknown0x78 << endl;
    cout << ind << "jab_2_window =                      " << attributes->jab_2_window << endl;
    cout << ind << "jab_3_window =                      " << attributes->jab_3_window << endl;
    cout << ind << "turn_duration =                     " << attributes->turn_duration << endl;
    cout << ind << "weight =                            " << attributes->weight << endl;
    cout << ind << "model_scaling =                     " << attributes->model_scaling << endl;
    cout << ind << "shield_size =                       " << attributes->shield_size << endl;
    cout << ind << "shield_break_initial_velocity =     " << attributes->shield_break_initial_velocity << endl;
    cout << ind << "rapid_jab_window =                  " << attributes->rapid_jab_window << endl;
    cout << ind << "unknown0x9C =                       " << attributes->unknown0x9C << endl;
    cout << ind << "unknown0xA0 =                       " << attributes->unknown0xA0 << endl;
    cout << ind << "unknown0xA4 =                       " << attributes->unknown0xA4 << endl;
    cout << ind << "ledgejump_horizontal_velocity =     " << attributes->ledgejump_horizontal_velocity << endl;
    cout << ind << "ledgejump_vertical_velocity =       " << attributes->ledgejump_vertical_velocity << endl;
    cout << ind << "item_throw_velocity =               " << attributes->item_throw_velocity << endl;
    cout << ind << "unknown0xB4 =                       " << attributes->unknown0xB4 << endl;
    cout << ind << "unknown0xB8 =                       " << attributes->unknown0xB8 << endl;
    cout << ind << "unknown0xBC =                       " << attributes->unknown0xBC << endl;
    cout << ind << "unknown0xC0 =                       " << attributes->unknown0xC0 << endl;
    cout << ind << "unknown0xC4 =                       " << attributes->unknown0xC4 << endl;
    cout << ind << "unknown0xC8 =                       " << attributes->unknown0xC8 << endl;
    cout << ind << "unknown0xCC =                       " << attributes->unknown0xCC << endl;
    cout << ind << "unknown0xD0 =                       " << attributes->unknown0xD0 << endl;
    cout << ind << "unknown0xD4 =                       " << attributes->unknown0xD4 << endl;
    cout << ind << "unknown0xD8 =                       " << attributes->unknown0xD8 << endl;
    cout << ind << "unknown0xDC =                       " << attributes->unknown0xDC << endl;
    cout << ind << "kirby_star_dmg =                    " << attributes->kirby_star_dmg << endl;
    cout << ind << "normal_landing_lag =                " << attributes->normal_landing_lag << endl;
    cout << ind << "n_air_landing_lag =                 " << attributes->n_air_landing_lag << endl;
    cout << ind << "f_air_landing_lag =                 " << attributes->f_air_landing_lag << endl;
    cout << ind << "b_air_landing_lag =                 " << attributes->b_air_landing_lag << endl;
    cout << ind << "d_air_landing_lag =                 " << attributes->d_air_landing_lag << endl;
    cout << ind << "u_air_landing_lag =                 " << attributes->u_air_landing_lag << endl;
    cout << ind << "victory_screen_scaling =            " << attributes->victory_screen_scaling << endl;
    cout << ind << "unknown0x1000 =                     " << attributes->unknown0x1000 << endl;
    cout << ind << "walljump_h_velocity =               " << attributes->walljump_h_velocity << endl;
    cout << ind << "walljump_v_velocity =               " << attributes->walljump_v_velocity << endl;
    cout << ind << "unknown0x10C =                      " << attributes->unknown0x10C << endl;
    cout << ind << "unknown0x110 =                      " << attributes->unknown0x110 << endl;
    cout << ind << "unknown0x114 =                      " << attributes->unknown0x114 << endl;
    cout << ind << "unknown0x118 =                      " << attributes->unknown0x118 << endl;
    cout << ind << "unknown0x11C =                      " << attributes->unknown0x11C << endl;
    cout << ind << "unknown0x120 =                      " << attributes->unknown0x120 << endl;
    cout << ind << "unknown0x124 =                      " << attributes->unknown0x124 << endl;
    cout << ind << "unknown0x128 =                      " << attributes->unknown0x128 << endl;
    cout << ind << "unknown0x12C =                      " << attributes->unknown0x12C << endl;
    cout << ind << "unknown0x130 =                      " << attributes->unknown0x130 << endl;
    cout << ind << "unknown0x134 =                      " << attributes->unknown0x134 << endl;
    cout << ind << "unknown0x138 =                      " << attributes->unknown0x138 << endl;
    cout << ind << "unknown0x13C =                      " << attributes->unknown0x13C << endl;
    cout << ind << "unknown0x140 =                      " << attributes->unknown0x140 << endl;
    cout << ind << "unknown0x144 =                      " << attributes->unknown0x144 << endl;
    cout << ind << "unknown0x148 =                      " << attributes->unknown0x148 << endl;
    cout << ind << "bubble ratio =                      " << attributes->bubble_ratio << endl;
    cout << ind << "unknown0x150 =                      " << attributes->unknown0x150 << endl;
    cout << ind << "unknown0x154 =                      " << attributes->unknown0x154 << endl;
    cout << ind << "unknown0x158 =                      " << attributes->unknown0x158 << endl;
    cout << ind << "unknown0x15C =                      " << attributes->unknown0x15C << endl;
    cout << ind << "ice_traction =                      " << attributes->ice_traction << endl;
    cout << ind << "unknown0x164 =                      " << attributes->unknown0x164 << endl;
    cout << ind << "unknown0x168 =                      " << attributes->unknown0x168 << endl;
    cout << ind << "camera_target_zoom_bone =           " << attributes->camera_target_zoom_bone << endl;
    cout << ind << "unknown0x170 =                      " << attributes->unknown0x170 << endl;
    cout << ind << "unknown0x174 =                      " << attributes->unknown0x174 << endl;
    cout << ind << "unknown0x178 =                      " << attributes->unknown0x178 << endl;
    cout << ind << "special jump action =               " << attributes->special_jump_action << endl;
    cout << ind << "weight dep. throw speed flags =     " << attributes->weight_dependent_throw_speed_flags << endl;
}

FtDataSubaction::FtDataSubaction(const DatFile * datfile,
        unsigned int index,
        ftdata_subaction_header * subheader) {
    this->datfile = datfile;
    this->header = subheader;
    fix_endianness(subheader, sizeof(subaction_header), 4);
    this->index = index;
    
    uint32_t stringOffset = this->header->stringOffset ;
    this->name = (stringOffset == 0) 
        ? "<no name>" 
        : datfile->dataSection + stringOffset;

    this->events = this->datfile->dataSection + 
        (this->header->eventsOffset);
}

void FtDataSubaction::print(int indent /*=0*/) {
    ios::fmtflags f( cout.flags() ); 

    unsigned char * event = (unsigned char *) this->events;
    if (*event == SUBACTION_TERMINATOR) {
        return;
    }

    string ind = string(indent * INDENT_SIZE, ' ');
    cout << ind << left << setw(3) << index << ": " << name << endl;
    ind = string ((indent + 1) * INDENT_SIZE, ' ');
    cout << ind << "event offset: " 
        << hex << this->header->eventsOffset << endl;
    
    char s[256];
    while(*event != SUBACTION_TERMINATOR) {
        unsigned int length = evt_lengths[(EVENT_ID) *event];
        if (length == 0) {
            cout << ind ;
            printf("%02x UNRECOGNIZED\n", (unsigned) *event);
            break;
        }
        cout << ind << evt_to_str(s,  event) << endl; 
        event += length;
    }
    cout.flags( f );
}

AnonymousData::AnonymousData(void * data): data(data){}
void AnonymousData::print(int indent /*=0*/) {
    string ind(indent * INDENT_SIZE, ' ');
    cout << ind << "Anonymous data at " << data << endl;
}

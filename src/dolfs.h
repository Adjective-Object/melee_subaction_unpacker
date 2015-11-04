#ifndef DOLFS_MREADER
#define DOLFS_MREADER
#include <cstdint>
#include <vector>
#include <map>

using namespace std;

#define INDENT_SIZE 2

typedef struct dat_header {
    uint32_t fileSize; // size of main data block
    uint32_t dataSectionSize;
    uint32_t offsetTableCount;
    uint32_t rootCountA;
    // 0x10
    uint32_t rootCountB;
    uint32_t unknown0x14; // '001B' in main Pl*.dat files
    uint32_t unknown0x18;
    uint32_t unknown0x1C;
    // 0x20
} __attribute__((packed)) dat_headyyer;

typedef struct dat_root_node {
    uint32_t dataSectionOffset;
    uint32_t stringTableOffset;
} __attribute__((packed)) dat_root_node;

// control structures that wrap the mapped raw structs
class DataProxy {
public:
    virtual void print(int indent = 0) = 0;
};

class DatFile : public DataProxy {
protected:
    // references to commonly used locations in the .dat file
public: // fuck it yolo
    dat_header * header;
    dat_root_node * rootList;
    uint32_t * offsetTable;
    char * dataSection;
    char * stringTable;

    // list of wrappers for children in this datfile
    map<string, DataProxy *> children;
public:
    DatFile(dat_header * head);
    void print(int indent = 0);
};



typedef struct ftdata_header {
    uint32_t attributesStart;
    uint32_t attirbutesEnd;
    uint32_t unknown0x08;
    uint32_t subactionsStart;
    uint32_t unknown0x10;
    uint32_t subactionsEnd;
    char unknown0x18[18];
} __attribute__((packed)) ftdata_header;

#ifndef __STDC_IEC_559__
#error "Requires IEEE 754 floating point!"
#endif

typedef struct ftdata_attribute_table {
    // 0x00
    float walk_initial_velocity;
    float walk_acceleration;       // ?
    float walk_maximum_velocity;
    float walk_animation_speed;
    // 0x10
    float walk_mid_point;          // ?
    float fast_walk_min;           // same as 0xc ?
    float friction;
    float dash_initial_velocity;
    // 0x20
    float stopturn_initial_velocity; // ??
    float run_initial_velocity;    // dash and run
    float run_max_velocity;        // dash and run
    float run_animation_scaling;
    // 0x30
    float run_acceleration;        // ?
    float unknown0x34;          // always 3
    float jump_startup_lag;     //(jump squat frames)
    float jump_h_initial_velocity; // ???
    // 0x40
    float jump_v_initial_velocity;  // ???
    float ground_air_jump_momentum_mult;
    float jump_h_max_velocity;      // ???
    float shorthop_v_initial_velocity;
    // 0x50
    float air_jump_multiplier;
    float unknown0x54;              // same as 0x04c
    uint32_t number_of_jumps;
    float gravity;
    //0x60
    float terminal_velocity;
    float aerial_mobility;
    float aerial_stopping_mobility;
    float max_aerial_h_velocity;
    //0x70
    float air_friction;
    float fast_fall_terminal_velocity;
    float unknown0x78;
    float jab_2_window;
    // 0x80
    float jab_3_window;
    float turn_duration;
    float weight;
    float model_scaling;
    // 0x90
    float shield_size;
    float shield_break_initial_velocity;
    uint32_t rapid_jab_window;
    float unknown0x9C;
    // 0xA0
    float unknown0xA0; // usually 0
    float unknown0xA4; // either 0 or 1
    float ledgejump_horizontal_velocity;
    float ledgejump_vertical_velocity;
    // 0xB0
    float item_throw_velocity;
    float unknown0xB4;
    float unknown0xB8;
    float unknown0xBC;
    // 0xC0
    float unknown0xC0;
    float unknown0xC4;
    float unknown0xC8;
    float unknown0xCC;
    // 0xD0
    float unknown0xD0;
    float unknown0xD4;
    float unknown0xD8;
    float unknown0xDC;
    // 0xE0
    float kirby_star_dmg;
    float normal_landing_lag;
    float n_air_landing_lag;
    float f_air_landing_lag;
    // 0xF0
    float b_air_landing_lag;
    float d_air_landing_lag;
    float u_air_landing_lag;
    float victory_screen_scaling;
    // 0x100
    float unknown0x1000;        // = 160?
    float walljump_h_velocity;
    float walljump_v_velocity;
    float unknown0x10C;
    // 0x110
    float unknown0x110;
    float unknown0x114;
    float unknown0x118;
    float unknown0x11C;
    // 0x120
    float unknown0x120;
    float unknown0x124;
    float unknown0x128;
    float unknown0x12C;
    // 0x130
    float unknown0x130;
    float unknown0x134;
    float unknown0x138;
    float unknown0x13C;
    // 0x140
    float unknown0x140;
    float unknown0x144;
    float unknown0x148;
    float bubble_ratio;            // 10 as int
    // 0x150
    float unknown0x150;
    float unknown0x154;
    float unknown0x158;
    float unknown0x15C;
    // 0x160
    float ice_traction;            // ?
    float unknown0x164;
    float unknown0x168;
    uint32_t camera_target_zoom_bone;
    // 0x170
    float unknown0x170;
    float unknown0x174;         // = 0x2D8?
    float unknown0x178;
    uint32_t special_jump_action;  // -1 for special
    // x180
    uint32_t weight_dependent_throw_speed_flags; 
} __attribute__((packed)) attribute_table;

typedef struct ftdata_subaction_header {
   uint32_t stringOffset; // offset to the null terminated subaction name
   uint32_t animationOffset;
   uint32_t unknown0x08;
   uint32_t eventsOffset; // offset to the null terminated subaction body
   uint16_t positionalFlags; // related to changing position
   uint16_t characterID;
   uint32_t unknown0x14;
} __attribute__((packed)) subaction_header;


class FtDataAttributes : public DataProxy {
    const DatFile * datfile;
    const ftdata_attribute_table * attributes;
public:
    FtDataAttributes(const DatFile * datheader,
            ftdata_attribute_table * header);
    void print(int indent = 0);
};

#define NUM_SUBACTIONS 0x126
class FtDataSubaction : public DataProxy {
    const DatFile * datfile;
    ftdata_subaction_header * header;
    const char * name;
    char * events;
    unsigned int index;
public:
    FtDataSubaction(const DatFile * datheader,
            unsigned int index,
            subaction_header * header);
    void print(int indent);
};


class FtData : public DataProxy {
    const DatFile * datfile;
    ftdata_header * ftheader;
    FtDataAttributes * attributes;
    vector<FtDataSubaction *> subactions;
public:
    FtData(const DatFile * datheader, ftdata_header * header);
    void print(int indent = 0);
};

class AnonymousData : public DataProxy {
    void * data;
public:
    AnonymousData(void * address);
    void print(int indent = 0);
};


// fix the endianness on a dolfile to match the host endianness
void dolfile_init(dat_header * datfile);
void print_dolfile_index(dat_header * datfile, unsigned int indent = 0);
dat_header * access_node(dat_header * header, dat_root_node * node);

#endif

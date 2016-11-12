#ifndef DOLFS_MREADER
#define DOLFS_MREADER
#include <cstdint>
#include <vector>
#include <map>

using namespace std;

/**
 * struct to be memory mapped onto a datfile
 * not a class to avoid weirdness with vft, etc
 **/
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


/**
 * root node of the data file
 **/
typedef struct dat_root_node {
  uint32_t dataSectionOffset;
  uint32_t stringTableOffset;
} __attribute__((packed)) dat_root_node;

/**
 * control structures that wrap the mapped raw structs
 * DataProxy provides a way to print the data,
 * and in the future, should provide a way
 * to encode/decode it from/to an easier editable file
 **/
class DataProxy {
public:
  virtual void print(int indent = 0) = 0;
  virtual void serialize() = 0;
};



class DatFile : public DataProxy {
public:
  // references to commonly used locations in the .dat file
  dat_header *header;
  dat_root_node *rootList;
  uint32_t *offsetTable;
  char *dataSection;
  char *stringTable;

  // list of wrappers for children in this datfile
  map<string, DataProxy *> children;

public:
  DatFile(dat_header *head);
  void print(int indent = 0);
  virtual void serialize();
  bool ptrIsRelocation(void * p) const;
};



/**
 * Fallback class for representing unknown data sections
 **/
class AnonymousData : public DataProxy {
  const DatFile * datfile;
  void *data;

public:
  AnonymousData(const DatFile * datfile, void *address);
  void print(int indent = 0);
  virtual void serialize(){};
};



/**
 * Inspector Class for looking at a range of memory
 **/
class DatInspector : public DataProxy {
    const DatFile * datfile;
    void * data;
    size_t size;
public:
    DatInspector(const DatFile * datfile, void * data, size_t size);
    void print(int indent = 0);
    void move(void * data, size_t size);
    void printRaw(int indent = 0, size_t columnWidth = 8, size_t = -1);
    virtual void serialize(){};
};



/**
 * fixes the endianness on a dolfile to match the host endianness.
 * Usually performed on an mmapped copy of a datfile
 **/
void dolfile_init(dat_header *datfile);

void print_dolfile_index(dat_header *datfile, unsigned int indent = 0);
dat_header *access_node(dat_header *header, dat_root_node *node);

#endif

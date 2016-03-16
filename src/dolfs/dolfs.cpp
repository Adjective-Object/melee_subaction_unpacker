#include "macros.hpp"
#include "event_mapper.hpp"
#include <iostream>
#include <iomanip>
#include <map>
#include <cstring>

#include "helpers.hpp"
#include "config.hpp"

#include "dolfs/dolfs.hpp"
#include "dolfs/ftdata.hpp"
#include "dolfs/jobj.hpp"

using namespace std;

DatFile::DatFile(dat_header *header) : header(header) {
  // Fix the endianness on the header itself
  fix_endianness(header, sizeof(dat_header), sizeof(uint32_t));

  // Use the header to get references to the rest of the sections
  // of the file
  this->dataSection = (char *)(header + 1);
  this->offsetTable = (uint32_t *)(dataSection + header->dataSectionSize);
  this->rootList =
      (dat_root_node *)(this->offsetTable + header->offsetTableCount);
  this->stringTable =
      (char *)(this->rootList + header->rootCountA + header->rootCountB);

  // Fix endianness of the offset table and node list
  // You can't fly jets if you're colourblind
  fix_endianness(offsetTable, sizeof(uint32_t) * header->offsetTableCount, 4);
  fix_endianness(rootList, sizeof(dat_root_node) *
                               (header->rootCountA + header->rootCountB),
                 4);

  // step into data section and try to initialize control structures for
  // the contents, and add them to the children map;
  children = map<string, DataProxy *>();

  unsigned int i;
  for (i = 0; i < header->rootCountA + header->rootCountB; i++) {
    string name = stringTable + rootList[i].stringTableOffset;
    void *target = dataSection + rootList[i].dataSectionOffset;

    // ftData (player files)
    if (memcmp("ftData", name.c_str(), 6) == 0) {
      children[name] = new FtData(this, (ftdata_header *)target);
    }

    // _joint (joint files)
    else if (hasEnding(name, "_joint")) {
        children[name] = new JObj(this, (jointdata_header *)target);
    }

    // fallback
    else {
      children[name] = new AnonymousData(target);
    }
  }
}

void DatFile::print(int indent) {
  string ind(indent * INDENT_SIZE, ' ');

  cout << ind << "addr of root list: " << this->rootList << endl;

  cout << ind << "root count A: " << header->rootCountA << endl;
  cout << ind << "root count B: " << header->rootCountB << endl;

  unsigned int root_ct = header->rootCountA + header->rootCountB;
  cout << ind << "total root count: " << root_ct << endl;

  ios::fmtflags f(cout.flags());

  cout << ind << "relocation table offset: " << hex
       << (char *)offsetTable - (char *)header << endl;

  cout << ind << "rootList offset: " << hex << (char *)rootList - (char *)header
       << endl;

  cout << ind << "string table offset: " << hex
       << (char *)stringTable - (char *)header << endl;

  cout.flags(f);

  map<string, DataProxy *>::iterator iter;
  for (iter = children.begin(); iter != children.end(); ++iter) {
    cout << ind << iter->first << endl;
    iter->second->print(indent + 1);
  }
}

void DatFile::serialize() {
    map<string, DataProxy *>::iterator iter;
    for (iter = children.begin(); iter != children.end(); ++iter) {
        iter->second->serialize();
    }
}

AnonymousData::AnonymousData(void *data) : data(data) {}
void AnonymousData::print(int indent /*=0*/) {
  string ind(indent * INDENT_SIZE, ' ');
  cout << ind << "Anonymous data at " << data << endl;
}

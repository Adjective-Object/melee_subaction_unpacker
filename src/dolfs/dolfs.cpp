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
#include "dolfs/figatree.hpp"

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

  /*
  cout << "root nodes:" << endl;
  for (i = 0; i < header->rootCountA + header->rootCountB; i++) {
    string name = stringTable + rootList[i].stringTableOffset;
    cout << name << endl;
  }
  */

  for (i = 0; i < header->rootCountA + header->rootCountB; i++) {
    string name = stringTable + rootList[i].stringTableOffset;
    void *target = dataSection + rootList[i].dataSectionOffset;

    // ftData (player files)
    if (memcmp("ftData", name.c_str(), 6) == 0) {
      children[name] = new FtData(this, (ftdata_header *)target);
    }

    // _metanim_joint (joint files)
    else if (hasEnding(name, "_matanim_joint")) {
      children[name] = new AnonymousData(this, target);
    }

    // _joint (joint files)
    else if (hasEnding(name, "_joint")) {
        children[name] = new JObj(this, (jointdata_header *)target);
    }

    // figatree (anim files) 
    else if (hasEnding(name, "_figatree")) {
        children[name] = new FigaTree(this, (figatree_header *)target);
    }

    // fallback
    else {
      children[name] = new AnonymousData(this, target);
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

AnonymousData::AnonymousData(const DatFile * datfile, void *data) : 
    datfile(datfile), data(data) {}
void AnonymousData::print(int indent /*=0*/) {
  string ind(indent * INDENT_SIZE, ' ');
  cout << ind << "Anonymous data at 0x"
       << hex << (int) ((char*) data - this->datfile->dataSection) << endl;
}





DatInspector::DatInspector(const DatFile * datfile, void * data, size_t size) :
    datfile(datfile), data(data), size(size) {
    fix_endianness(data, size, sizeof(uint32_t));
    // fix_endianness(data, size, sizeof(uint16_t));
}

void DatInspector::print(int indent) {
    string ind(indent * INDENT_SIZE, ' ');

    cout 
        << endl << ind << CYAN << "data:        " << (void *) this->data
        << endl << ind << CYAN << "dataSection: " << (void *) this->datfile->dataSection

        << endl << ind << BLUE  << "dataSection -> rootlist:   " << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->rootList) -
                ((char *) this->datfile->dataSection))

        << endl << ind << BLUE  << "dataSection -> offsetTable:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->offsetTable) -
                ((char *) this->datfile->dataSection))

        << endl << ind << BLUE  << "dataSection -> data:       " << hex
        << setw(15) << (
                ((ptrdiff_t) this->data) - 
                ((ptrdiff_t) this->datfile->dataSection))


        << endl << ind << GREEN << "origin      -> dataSection:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->dataSection) - 
                ((char *) MMAP_ORIGIN))

        << endl << ind << GREEN << "origin      -> offsetTable:" << hex
        << setw(15) << (intptr_t) (
                ((char *) this->datfile->offsetTable) - 
                ((char *) MMAP_ORIGIN))

        << endl << ind << GREEN << "origin      -> data:       " << hex 
        << setw(15) << (intptr_t) (
                ((char *) this->data) - ((char *) MMAP_ORIGIN)
        ) << RESET << endl;

    cout << endl << ind
         << CYAN << REVERSE << setw(5) << "~ " << " " 
         << setw(15) << "hex" 
         << " "
         << setw(15) << "dec" 
         << " "
         << setw(15) << "float" 
         << " "
         << setw(18) << "shorts" 
         << " "
         << setw(18) << "chars" 
         << " "
         << RESET << RESETREVERSE << endl;

    uint32_t * datint= (uint32_t *) data;
    float * datfloat = (float *) data;
    short * datshort = (short *) data;
    char * datchar = (char *) data;

    for (uint i=0; i<this->size/sizeof(uint32_t); i++) {
        cout << ind
             << CYAN << REVERSE << setw(4) << i <<  " " 
             << RESET << RESETREVERSE << " " 
             << setw(15) << "0x" + to_string_hex(datint[i])
             << " "
             << setw(15) << dec << datint[i]
             << " "
             << setw(15) << datfloat[i]

             << "     "
             << setw(6) << datshort[i * 2]
             << "  "
             << setw(6) << datshort[i * 2 + 1]

             << "     "
             << setw(3) << +(datchar[i * 4])
             << " "
             << setw(3) << +(datchar[i * 4 + 1])
             << " "
             << setw(3) << +(datchar[i * 4 + 2])
             << " "
             << setw(3) << +(datchar[i * 4 + 3])

             << endl;
    }
}




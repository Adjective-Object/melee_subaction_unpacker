#include <stddef.h>
#include <cstdio>
#include <cstring>

#include "helpers.hpp"
#include "config.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

#include <vector>
#include <utility>

using namespace std;

void * MMAP_ORIGIN;

void cout_hex(int indent, unsigned char *c, size_t rowsize, size_t rows) {
  string ind = string(indent * INDENT_SIZE, ' ');
  for (unsigned int i = 0; i < rows; i++) {
    cout << ind;
    for (unsigned int j = 0; j < rowsize; j++) {
      cout << hex << setfill('0') << setw(2) << +c[i * rowsize + j] << " ";
    }
    cout << endl;
  }
}

void print_hex(char *c, size_t ct) {
  for (size_t i = 0; i < ct; i++) {
    printf("%02x", (int)c[i]);
    printf("%c", c[i]);
  }
}

#if IS_BIG_ENDIAN
void fix_endianness(void *location, size_t bytes, size_t step){}
size_t get_available_size(void * location, size_t bytes) {
    return bytes;
}
#else
static vector<pair<size_t, size_t>> * swapped_ranges = NULL;
void check_bounds(size_t sloc, size_t bytes) {
    sloc = sloc - (size_t) MMAP_ORIGIN;

    cout << "check_bounds(0x" << hex << sloc << ", "
       << "0x" << hex << bytes << ")"
       << endl;

    if (swapped_ranges == NULL) {
        swapped_ranges = new vector<pair<size_t, size_t>>();
    } else {
        for (pair<size_t, size_t> p : *swapped_ranges){
            if (sloc + bytes > p.first && 
                p.second > sloc) {
                cout << RED << "intersection of segments (" 
                     << hex << sloc << ", " << hex << sloc + bytes << ") and ("
                     << hex << p.first << ", " << hex << p.second << ")"
                     << RESET << endl;
            }
        }
    }
    swapped_ranges->push_back(make_pair(sloc, sloc +  bytes));
}

/**
 * Gets the available space, up to size <bytes> until the next endian-fixed
 * region
 **/
size_t get_available_size(void * location, size_t bytes) {
    size_t sloc = ((size_t) location) - ((size_t) MMAP_ORIGIN);
    for (pair<size_t, size_t> p : *swapped_ranges){
        bytes = min(bytes, p.first - sloc);
    }
    return bytes - 1;
}

void fix_endianness(void *location, size_t bytes, size_t step) {
  char *l = (char *)location;

  check_bounds((size_t) location, bytes);
  
  for (size_t b = 0; b < bytes; b += step) {
    // switch the endianness on step
    for (size_t o = 0; o < step / 2; o++) {
      char tmp = l[b + o];
      l[b + o] = l[b + step - 1 - o];
      l[b + step - 1 - o] = tmp;
    }
  }
}


#include <iostream>
#include <iomanip>

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

string to_string_hex(int n) {
   std::stringstream ss;
   ss << hex << n;
   return ss.str();
}



#endif

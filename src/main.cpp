/*
* Author: Xiang-Guan Deng
* This program is to read .dtb file.
*/

#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <iomanip>

using namespace std;

typedef uint32_t fdt32_t;
typedef uint64_t fdt64_t;

#include "fdt.h"

#define DEBUG 1

#define ENDIAN_CHANGE32(x)   ((((((x & 0xFF00FF00U) >> 8U) | ((x & 0x00FF00FF) << 8U)) & 0xFFFF0000U) >> 16U) | (((((x & 0xFF00FF00U) >> 8U) | ((x & 0x00FF00FF) << 8U)) & 0x0000FFFFU) << 16U))


class Node {
public:
  Node(const string &key, const string &val) : key_(key), val_(val) {}
  Node(const string &key, const vector<string> &val_list) : key_(key), val_list_(val_list) {}
  ~Node () {

  }
  Node *next;
  Node *prev;
private:
  string key_;
  string val_;
  vector<string> val_list_;
};

class Fdt {
public:
  Fdt() {
  }
  ~Fdt() {

  }
  void parse(uint8_t *bin, uint32_t sz) {
    memcpy(&hd_, bin, sizeof(struct fdt_header));
    magic = ENDIAN_CHANGE32(hd_.magic);
    totalsize = ENDIAN_CHANGE32(hd_.totalsize);
    dt_struct_off = ENDIAN_CHANGE32(hd_.off_dt_struct);
    dt_strings_off = ENDIAN_CHANGE32(hd_.off_dt_strings);
    mem_rvsmap_off = ENDIAN_CHANGE32(hd_.off_mem_rsvmap);
    version = ENDIAN_CHANGE32(hd_.version);
    last_comp_version = ENDIAN_CHANGE32(hd_.last_comp_version);
    cpuid_phys = ENDIAN_CHANGE32(hd_.boot_cpuid_phys);
    dt_strings_sz = ENDIAN_CHANGE32(hd_.size_dt_strings);
    dt_struct_sz = ENDIAN_CHANGE32(hd_.size_dt_struct);

    name_string = "";
    for(int i=0;i<dt_strings_sz;i++) {
      name_string += bin[dt_strings_off + i];
    }

    int i = dt_struct_off;
    while(i < (dt_struct_off+dt_struct_sz)) {
      uint32_t code = *((uint32_t *)&bin[i]);
      if(FDT_BEGIN_NODE == code) {
        
        i += 4;
      }
      else if(FDT_END_NODE == code) {
        i += 4;
      }
      else if(FDT_END == code) {
        i += 4; 
      }
      else if(FDT_PROP == code) {
        i += 12;
      }
      else {
        // bypassing
        i++;
      }
    }
  }
  void print_header() {
  }
  void print_string() {
    cout << name_string << endl;
  }
  void print_node_info() {

  }
  uint32_t magic;
  uint32_t totalsize;
  uint32_t dt_struct_off;
  uint32_t dt_strings_off;
  uint32_t mem_rvsmap_off;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t cpuid_phys;
  uint32_t dt_strings_sz;
  uint32_t dt_struct_sz;

private:
  struct fdt_header hd_;
  vector<Node *>roots;
  string name_string;
};

void hexdump(uint8_t *buf, int sz);
void printString(void *buf);

void hexdump(uint8_t *buf, int sz) {
  int i;
  for(i=0;i<sz;i++) {
    cout << hex << setw(2) << setfill('0') << static_cast<int>(buf[i]) << "  ";
    if((i+1)%16==0) {
      cout << endl;
    }
  }
  if(i%16) {
    cout << endl;
  }
}

void printString(void *buf) {
  struct fdt_header *hd = (struct fdt_header *)buf;
  ENDIAN_CHANGE32(hd->size_dt_struct);
  string s = "";
  for(size_t i=ENDIAN_CHANGE32(hd->off_dt_strings);i<ENDIAN_CHANGE32(hd->off_dt_strings)+ENDIAN_CHANGE32(hd->size_dt_strings);i++) {
    s += ((char *)buf)[i];
  }
  cout << s << endl;
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  cout << "Hello world" << endl;

  if(2 != argc) {
    cerr << "Please provide valid dtb file" << endl;
    return 1;
  }

  const char *filename = argv[1];

  // Open the binary file
  ifstream infile(filename, ios::binary);

  // Check if the file was opened successfully
  if (!infile) {
    cerr << "Error opening file." << endl;
    return 1;
  }

  struct stat file_stat;
  if(0 == stat(filename, &file_stat)) {
    cout << "File " << filename << " size is " << file_stat.st_size << " bytes" << endl;
  }
  else {
    cerr << "Can not get file size." << endl;
    return 1;
  }

  uint8_t *bin = new uint8_t[file_stat.st_size];
  infile.read((char *)bin, file_stat.st_size);
  infile.close();

#if DEBUG
  hexdump(bin, file_stat.st_size);
#endif /* DEBUG */

  struct fdt_header *hd = (struct fdt_header *)bin;

  cout << hex;
  cout << "magic: " << ENDIAN_CHANGE32(hd->magic) << endl;
  cout << dec;
  cout << "totalsize: " << ENDIAN_CHANGE32(hd->totalsize) << "d" << endl;
  cout << hex;
  cout << "off_dt_struct: " << ENDIAN_CHANGE32(hd->off_dt_struct) << endl;
  cout << "off_dt_strings: " << ENDIAN_CHANGE32(hd->off_dt_strings) << endl;
  cout << "off_mem_rsvmap: " << ENDIAN_CHANGE32(hd->off_mem_rsvmap) << endl;
  cout << "version: " << ENDIAN_CHANGE32(hd->version) << endl;
  cout << "last_comp_version: " << ENDIAN_CHANGE32(hd->last_comp_version) << endl;
  cout << "boot_cpuid_phys: " << ENDIAN_CHANGE32(hd->boot_cpuid_phys) << endl;
  cout << dec;
  cout << "size_dt_strings: " <<  ENDIAN_CHANGE32(hd->size_dt_strings) << "d" << endl;
  cout << "size_dt_struct: " << ENDIAN_CHANGE32(hd->size_dt_struct) << "d" << endl;

  cout << "fdt header size is " << sizeof(struct fdt_header) << endl;
  hexdump(&bin[sizeof(struct fdt_header)], file_stat.st_size - sizeof(struct fdt_header));

  if(FDT_MAGIC != ENDIAN_CHANGE32(hd->magic)) {
    cerr << "Invalid magic code" << endl;
    return -1;
  }


  printString((void *)bin);


  /* Start parsing dtb file */
  uint32_t i = ENDIAN_CHANGE32(hd->off_dt_struct);
  cout << dec;
  while(i < (ENDIAN_CHANGE32(hd->size_dt_struct) + ENDIAN_CHANGE32(hd->off_dt_struct))) {
    uint32_t code = ENDIAN_CHANGE32(*((uint32_t *)(&bin[i])));
    if(code == FDT_BEGIN_NODE) {
      cout << endl << "===== BEGIN NODE =====" << endl;
      i += 4;
    }
    else if(code == FDT_END_NODE) {
      cout << endl << "===== END NODE =====" << endl;
      i += 4;
    }
    else if(code == FDT_END) {
      cout << endl << "===== END =====" << endl;
      i += 4;
    }
    else if(code == FDT_PROP) {
      cout << endl << "===== PROP =====" << endl;
      cout << "length is 0x" << hex << ENDIAN_CHANGE32(*((uint32_t *)(&bin[i+4]))) << endl;
      cout << "string offset is 0x" << hex << ENDIAN_CHANGE32(*((uint32_t *)(&bin[i+8]))) << endl;
      i += sizeof(struct fdt_property);
    }
    else if(code == FDT_NOP || 0 == code) {
      cout << endl << "===== NOP =====" << endl;
      i += 4;
    }
    else if(bin[i] >= 0x20) {
      cout << bin[i];
      i++;
    }
    else {
      cerr << "#";
      i++;
    }
  }
  cout << endl;


  Fdt fdt;
  fdt.parse(bin, file_stat.st_size);
  fdt.print_header();
  fdt.print_string();
  fdt.print_node_info();

  delete[] bin;

  return 0;
}


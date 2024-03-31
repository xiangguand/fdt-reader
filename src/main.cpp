/*
* Author: Xiang-Guan Deng
* This program is to read .dtb file.
*/

#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

typedef uint32_t fdt32_t;
typedef uint64_t fdt64_t;

#include "fdt.h"

#define DEBUG 0

#define ENDIAN_CHANGE32(x)   ((((((x & 0xFF00FF00U) >> 8U) | ((x & 0x00FF00FF) << 8U)) & 0xFFFF0000U) >> 16U) | (((((x & 0xFF00FF00U) >> 8U) | ((x & 0x00FF00FF) << 8U)) & 0x0000FFFFU) << 16U))

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

class Node {
public:
  Node(const string &key, const string &val) : key_(key), val_(val) { next = nullptr; }
  Node(const string &key, const vector<string> &val_list) : key_(key), val_list_(val_list) {}
  ~Node () {

  }
  Node *next;
  string key_;
  string val_;
  vector<string> val_list_;
};

class GNode {
public:
  GNode(string nm, int depth) { next = nullptr; prev = nullptr; info = nullptr; this->depth = depth; name=nm; }
  GNode() { next = nullptr; prev = nullptr; info = nullptr; depth = 0; name = ""; }
  GNode *next;
  GNode *prev;
  Node *info;
  int depth;
  string name;
};

void append_node(Node **head, Node *new_node) {
  if(*head == nullptr) {
    *head = new_node;
  }
  else {
    Node *node = *head;
    while(node->next) {
      node = node->next;
    }
    node->next = new_node;
  }
}

void append_gnode(GNode **head, GNode *new_node) {
  if(*head == nullptr) {
    *head = new_node;
  }
  else {
    GNode *node = *head;
    while(node->next) {
      node = node->next;
    }
    node->next = new_node;
    new_node->prev = node;
  }
}

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
    string current_node = "";
    GNode *cur_gnode = nullptr;
    int depth = 0;

    while(i < (dt_struct_off+dt_struct_sz)) {
      uint32_t code = ENDIAN_CHANGE32(*((uint32_t *)&bin[i]));
      if(FDT_BEGIN_NODE == code && 0 == depth) {
        // cout << "FDT_BEGIN_NODE" << endl;
        string node_name = "";
        int i_s = 0;
        while(bin[i+4+i_s]) {
          node_name += bin[i+4+i_s];
          i_s++;
        }
        if(0 == node_name.length())
        {
          node_name = "-";
        }
        // cout << "Node name: " << node_name << ", depth: " << depth << endl;
        roots[node_name] = new GNode(node_name, depth);
        cur_gnode = roots[node_name];
        current_node = node_name;
        depth++;
        i += 4 + node_name.length();
      }
      else if(FDT_BEGIN_NODE == code && 0 < depth) {
        // cout << "FDT_BEGIN_NODE" << endl;
        string child_node_name = "";
        int i_s = 0;
        while(bin[i+4+i_s]) {
          child_node_name += bin[i+4+i_s];
          i_s++;
        }

        // cout << "Child node name: " << child_node_name  << ", depth: " << depth << endl;
        GNode *new_gnode = new GNode(child_node_name, depth);
        append_gnode(&roots[current_node], new_gnode);
        cur_gnode = new_gnode;

        depth++;
        i += 4 + child_node_name.length();
      }
      else if(FDT_END_NODE == code) {
        // cout << "FDT_END_NODE" << endl;
        i += 4;
        if(cur_gnode)
          cur_gnode = cur_gnode->prev;
        depth--;
      }
      else if(FDT_END == code) {
        // cout << "FDT_END" << endl;
        i += 4; 
      }
      else if(FDT_PROP == code) {
        uint32_t dt_size = ENDIAN_CHANGE32(*((uint32_t *)&bin[i+4]));
        uint32_t name_offset = ENDIAN_CHANGE32(*((uint32_t *)&bin[i+8]));
        string dt = "";
        if(dt_size > 0) {
          dt = string((const char *)&bin[i+12], dt_size-1);
        }
        // name: dt
        string nm = "";
        int i_s = 0;
        while(bin[dt_strings_off + name_offset + i_s]) {
          nm += bin[dt_strings_off + name_offset + i_s];
          i_s++;
        }
        Node *node = new Node(nm, dt);
        append_node(&cur_gnode->info, node);

        // cout << "nm: " << nm << ": " << dt << endl;
        i += 12 + dt_size;
      }
      else {
        // bypassing
        i++;
      }
    }
  }
  string genHyphens(int count) {
    string s = "";
    for(int i=0;i<count;i++) {
      s += "-";
    }
    return s;
  }
  void print_header() {
    
    cout << "All values below are in HEX representation." << endl;
    cout << hex;
    cout << "magic: " << magic << endl;
    cout << "totalsize: " << totalsize << endl;
    cout << "off_dt_struct: " <<  dt_struct_off<< endl;
    cout << "off_dt_strings: " <<  dt_strings_off<< endl;
    cout << "off_mem_rsvmap: " << mem_rvsmap_off << endl;
    cout << "version: " << version << endl;
    cout << "last_comp_version: " <<  last_comp_version<< endl;
    cout << "boot_cpuid_phys: " <<  cpuid_phys<< endl;
    cout << "size_dt_strings: " <<  dt_strings_sz << endl;
    cout << "size_dt_struct: " <<  dt_struct_sz << endl;
    cout << "fdt header size is " << sizeof(struct fdt_header) << endl;
    cout << dec;
  }
  void print_string() {
    cout << name_string << endl;
  }

  void print_node_help(Node *info, int depth) {
    cout << genHyphens(2*depth) << ">" << info->key_ << ": " << info->val_ << endl;
    if(info->next) {
      print_node_help(info->next, depth);
    }
  }
  void print_gnode_help(GNode *node) {
    if(node->info) {
      print_node_help(node->info, node->depth);
    }
    if(node->next) {
      // cout << "=====GNode "<< node->next->name << " depth: " << node->next->depth << "=====" << endl;
      cout << genHyphens(2*node->next->depth) << node->next->name << endl;
      print_gnode_help(node->next);
    }
  }
  void print_node_info() {
    for(auto it:roots) {
      cout << "Root node: " << it.first << endl;
      print_gnode_help(it.second);
    }
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
  map<string, GNode *>roots;
  string name_string;
};

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

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

#if DEBUG
  hexdump(&bin[sizeof(struct fdt_header)], file_stat.st_size - sizeof(struct fdt_header));
#endif /* DEBUG */

  if(FDT_MAGIC != ENDIAN_CHANGE32(hd->magic)) {
    cerr << "Invalid magic code" << endl;
    return -1;
  }

  // printString((void *)bin);

  Fdt fdt;
  fdt.parse(bin, file_stat.st_size);
  fdt.print_header();

  cout << "===== Tree the node start =====" << endl;
  fdt.print_node_info();

  delete[] bin;
  
  cout << "===== Tree the node end =====" << endl;

  return 0;
}


/*
* Author: Xiang-Guan Deng
* This program is to read .dtb file.
*/

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <iomanip>

#define DEBUG 1

#define FDT_MAGIC (0xEDFE0DD0U)

/* FDT Header, copy from spec */
struct fdt_header {
  uint32_t magic;
  uint32_t totalsize;
  uint32_t off_dt_struct;
  uint32_t off_dt_strings;
  uint32_t off_mem_rsvmap;
  uint32_t version;
  uint32_t last_comp_version;
  uint32_t boot_cpuid_phys;
  uint32_t size_dt_strings;
  uint32_t size_dt_struct;
};

using namespace std;

#if DEBUG
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
#endif /* DEBUG */

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
  cout << "magic: " << hd->magic << endl;
  cout << dec;
  cout << "totalsize: " << hd->totalsize << "d" << endl;
  cout << hex;
  cout << "off_dt_struct: " << hd->off_dt_struct << endl;
  cout << "off_dt_strings: " << hd->off_dt_strings << endl;
  cout << "off_mem_rsvmap: " << hd->off_mem_rsvmap << endl;
  cout << "version: " << hd->version << endl;
  cout << "last_comp_version: " << hd->last_comp_version << endl;
  cout << "boot_cpuid_phys: " << hd->boot_cpuid_phys << endl;
  cout << dec;
  cout << "size_dt_strings: " <<  hd->size_dt_strings << "d" << endl;
  cout << "size_dt_struct: " << hd->size_dt_struct << "d" << endl;


  if(FDT_MAGIC != hd->magic) {
    cerr << "Invalid magic code" << endl;
    return -1;
  }


  delete[] bin;

  return 0;
}


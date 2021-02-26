#include <stdint.h>
#include <iostream>
#include <string.h>
#include "./LogFS/LogFS.h"

#define SIXTEEN_MB 16 * 1024 * 1024

class MiniSPIFFS : public LogFS {
  public:
    uint8_t data[100];

  private:
    uint32_t writeByte(uint32_t address, uint8_t value) {
      data[address] = value;
      return address + 1;
    }
    uint8_t readByte(uint32_t address) {
      return data[address];
    }
};

void test(const char* name, bool result) {
  std::cout << (result ? "OK    " : "FAILED") << " - " << name << "\n";
}

bool notFormatted() {
  MiniSPIFFS fs;
  return fs.init() == LOGFS_ERR_NOT_FORMATTED;
}

bool differentVersion() {
  MiniSPIFFS fs;
  strcpy((char*)fs.data, "LOGFS");
  fs.data[5] = 2;
  return fs.init() == LOGFS_ERR_DIFFERENT_VERSION;
}

bool format1() {
  MiniSPIFFS fs;
  fs.format(SIXTEEN_MB);
  return fs.init() == LOGFS_OK;
}

bool formatPageSize() {
  uint16_t pageSize = 16384;
  MiniSPIFFS fs;
  fs.format(SIXTEEN_MB, pageSize);
  fs.init();
  return fs.pageSize() == pageSize;
}

bool formatFilesAmount() {
  uint16_t filesAmount = 1024;
  MiniSPIFFS fs;
  fs.format(SIXTEEN_MB, 512, filesAmount);
  fs.init();
  return fs.filesAmount() == filesAmount;
}

int main() {
  std::cout << "Start testing!\n";

  test("not formatted error", notFormatted());
  test("different version", differentVersion());
  test("format 1", format1());
  test("format page size", formatPageSize());
  test("format files amount", formatFilesAmount());




//   fs.format(16 * 1024 * 1024);

//   for (int i = 0; i < 100; i++) {
//     std::cout << int(data[i]) << ',';
//   }
//   std::cout << "\n";
}


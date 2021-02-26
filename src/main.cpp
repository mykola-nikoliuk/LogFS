#include <stdint.h>
#include <iostream>
#include <string.h>
#include "./LogFS/File.h"
#include "./LogFS/LogFS.h"

#define SIXTEEN_MB 16 * 1024 * 1024

struct MiniFSIO : public FSIO {
  public:
    uint8_t data[1024];
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
  MiniFSIO fsio;
  LogFS fs(&fsio);
  return fs.init() == LOGFS_ERR_NOT_FORMATTED;
}

bool differentVersion() {
  MiniFSIO fsio;
  LogFS fs(&fsio);
  strcpy((char*)fsio.data, "LOGFS");
  fsio.data[6] = 2;
  return fs.init() == LOGFS_ERR_DIFFERENT_VERSION;
}

bool format1() {
  MiniFSIO fsio;
  LogFS fs(&fsio);
  fs.format(SIXTEEN_MB);
  return fs.init() == LOGFS_OK;
}

bool formatPageSize() {
  uint16_t pageSize = 16384;
  MiniFSIO fsio;
  LogFS fs(&fsio);
  fs.format(SIXTEEN_MB, pageSize);
  fs.init();
  return fs.getHeader()->pageSize == pageSize;
}

bool formatFilesAmount() {
  uint16_t filesAmount = 1024;
  MiniFSIO fsio;
  LogFS fs(&fsio);
  fs.format(SIXTEEN_MB, 512, filesAmount);
  fs.init();
  return fs.getHeader()->filesAmount == filesAmount;
}

bool formatLowSpaceFileTable() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  uint16_t filesAmount = 32;
  uint16_t pageSize = 512;

  return fs.format(32, pageSize, filesAmount) == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
}

bool formatLowSpacePages() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  uint16_t filesAmount = 8;
  uint16_t pageSize = 1024;

  return fs.format(1024, pageSize, filesAmount) == LOGFS_ERR_LOW_SPACE_PAGES;
}

bool formatOk() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  uint16_t filesAmount = 1;
  uint16_t pageSize = 512;

  return fs.format(1024, pageSize, filesAmount) == LOGFS_OK;
}

int main() {
  std::cout << "Start testing!\n";

  test("not formatted error", notFormatted());
  test("different version", differentVersion());
  test("format 1", format1());
  test("format page size", formatPageSize());
  test("format files amount", formatFilesAmount());
  test("format low space for file table", formatLowSpaceFileTable());
  test("format low space for pages", formatLowSpacePages());
  test("format ok", formatOk());

//   std::cout << sizeof(struct LogFSFile) << "\n";


//   fs.format(16 * 1024 * 1024);

//   for (int i = 0; i < 100; i++) {
//     std::cout << int(data[i]) << ',';
//   }
//   std::cout << "\n";
}


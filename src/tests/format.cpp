#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./format.h"

using namespace std;

#define MEMORY_SIZE 1024 * 1024

struct MiniFSIO : public FSIO {
  public:
    uint8_t data[MEMORY_SIZE];
    uint32_t writeByte(uint32_t address, uint8_t value) {
      data[address] = value;
      return address + 1;
    }
    uint8_t readByte(uint32_t address) {
      return data[address];
    }
};

void test(const char* name, bool result) {
  cout << (result ? "OK    " : "FAILED") << " - " << name << endl;
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

bool formatInit() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  fs.format(MEMORY_SIZE);
  return fs.init() == LOGFS_OK;
}

bool formatPageSize() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  uint16_t pageSize = 16384;
  fs.format(MEMORY_SIZE, pageSize);
  fs.init();
  return fs.getHeader()->pageSize == pageSize;
}

bool formatFilesAmount() {
  MiniFSIO fsio;
  LogFS fs(&fsio);

  uint16_t filesAmount = 1024;
  fs.format(MEMORY_SIZE, 512, filesAmount);
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

void testFormat() {
  cout << "Format:" << endl;

  test("not formatted error", notFormatted());
  test("different version", differentVersion());
  test("init", formatInit());
  test("page size", formatPageSize());
  test("files amount", formatFilesAmount());
  test("low space for file table", formatLowSpaceFileTable());
  test("low space for pages", formatLowSpacePages());
  test("ok", formatOk());
}

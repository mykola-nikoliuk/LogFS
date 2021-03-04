#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./format.h"

using namespace std;

#define MEMORY_SIZE 1024 * 1024

bool notFormatted() {
  LogFSRAM fs;

  return fs.init() == LOGFS_ERR_NOT_FORMATTED;
}

bool differentVersion() {
  LogFSRAM fs;

  strcpy((char*)fs.fio.data, "LOGFS");
  fs.fio.data[6] = 2;
  return fs.init() == LOGFS_ERR_DIFFERENT_VERSION;
}

bool formatInit() {
  LogFSRAM fs;

  fs.format(MEMORY_SIZE);
  return fs.init() == LOGFS_OK;
}

bool formatPageSize() {
  LogFSRAM fs;

  uint16_t pageSize = 16384;
  fs.format(MEMORY_SIZE, pageSize);
  fs.init();
  return fs.getHeader()->pageSize == pageSize;
}

bool formatFilesAmount() {
  LogFSRAM fs;

  uint16_t filesAmount = 1024;
  fs.format(MEMORY_SIZE, 512, filesAmount);
  fs.init();
  return fs.getHeader()->filesAmount == filesAmount;
}

bool formatLowSpaceFileTable() {
  LogFSRAM fs;

  uint16_t filesAmount = 32;
  uint16_t pageSize = 512;

  return fs.format(32, pageSize, filesAmount) == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
}

bool formatLowSpacePages() {
  LogFSRAM fs;

  uint16_t filesAmount = 8;
  uint16_t pageSize = 1024;

  return fs.format(1024, pageSize, filesAmount) == LOGFS_ERR_LOW_SPACE_PAGES;
}

bool formatOk() {
  LogFSRAM fs;

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

#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./format.h"

using namespace std;

#define TEST_MEMORY_SIZE 1024 * 1024

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

  fs.format();
  return fs.init() == LOGFS_OK;
}

bool formatPageSize() {
  LogFSRAM fs;

  uint16_t pageSize = 16384;
  fs.format();
  fs.init();
  return fs.getHeader()->pageSize == pageSize;
}

bool formatLowSpaceFileTable() {
  LogFSRAM fs;

  return fs.format() == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
}

bool formatLowSpacePages() {
  LogFSRAM fs;

  return fs.format() == LOGFS_ERR_LOW_SPACE_PAGES;
}

bool formatOk() {
  LogFSRAM fs;

  return fs.format() == LOGFS_OK;
}

void testFormat() {
  cout << "Format:" << endl;

  test("not formatted error", notFormatted());
  test("different version", differentVersion());
  test("init", formatInit());
  test("page size", formatPageSize());
  test("low space for file table", formatLowSpaceFileTable());
  test("low space for pages", formatLowSpacePages());
  test("ok", formatOk());
}

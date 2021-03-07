#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../src/LogFS.h"
#include "./test.h"
#include "./format.h"

using namespace std;

#define TEST_MEMORY_SIZE 1024 * 1024

bool notFormatted() {
  LogFSRAM fs;
  return fs.init() == LOGFS_ERR_NOT_FORMATTED;
}

bool formatOk() {
  LogFSRAM fs;
  return fs.format() == LOGFS_OK;
}

bool formatInit() {
  LogFSRAM fs;
  fs.format();
  return fs.init() == LOGFS_OK;
}

bool differentVersion() {
  LogFSRAM fs;
  strcpy((char*)fs.fio.data, "LOGFS");
  fs.fio.data[6] = 2;
  return fs.init() == LOGFS_ERR_DIFFERENT_VERSION;
}

bool formatPageSize() {
  uint16_t pageSize = 64;
  RAMIO fio(MAX_MEMORY_SIZE, SECTOR_SIZE, pageSize);
  LogFS fs(&fio);
  fs.format();
  fs.init();
  return fs.getHeader()->pageSize == pageSize;
}

bool formatLowSpaceForHeader() {
  RAMIO fio(SECTOR_SIZE - 1);
  LogFS fs(&fio);
  return fs.format() == LOGFS_ERR_LOW_SPACE_HEADER;
}

bool formatLowSpaceForSectorsMap() {
  RAMIO fio(SECTOR_SIZE * 2 - 1);
  LogFS fs(&fio);
  return fs.format() == LOGFS_ERR_LOW_SPACE_SECTORS_MAP;
}

bool formatLowSpaceForSectors() {
  RAMIO fio(SECTOR_SIZE * 3 - 1);
  LogFS fs(&fio);
  return fs.format() == LOGFS_ERR_LOW_SPACE_SECTORS;
}

void testFormat() {
  cout << "Format:" << endl;

  test("not formatted error", notFormatted());
  test("ok", formatOk());
  test("init", formatInit());
  test("different version", differentVersion());
  test("page size", formatPageSize());
  test("low space for header", formatLowSpaceForHeader());
  test("low space for sectors map", formatLowSpaceForSectorsMap());
  test("low space for sectors", formatLowSpaceForSectors());
}

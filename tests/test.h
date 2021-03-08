#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include "../src/LogFS.h"
#include <iostream>

#define SECTOR_SIZE 4096
#define PAGE_SIZE 256
#define MAX_MEMORY_SIZE 16 * SECTOR_SIZE

struct RAMIO : public LogFSStorageIO {
  static uint32_t resetChipCounter;
  static uint32_t resetSectorCounter;
  static uint32_t writePageCounter;
  static uint32_t readPageCounter;

  RAMIO(uint32_t capacity = MAX_MEMORY_SIZE, uint16_t sectorSize = SECTOR_SIZE,
        uint16_t pageSize = PAGE_SIZE) : LogFSStorageIO(capacity, sectorSize, pageSize) {};

  uint8_t data[MAX_MEMORY_SIZE];

  void resetChip() {
    resetChipCounter++;
    for (uint32_t i = 0; i < getCapacity(); i++) {
      data[i] = 0xFF;
    }
  };
  void resetSector(uint32_t sectorIndex) {
    resetSectorCounter++;
    uint32_t address = sectorIndex * getSectorSize();
    uint16_t sectorSize = getSectorSize();
    for (uint32_t i = 0; i < sectorSize; i++) {
      data[address + i] = 0xFF;
    }
  };
  void writePage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    writePageCounter++;
    uint8_t *pData = (uint8_t *) pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      data[address + i] &= pData[i];
    }
  };
  void readPage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    readPageCounter++;
    uint8_t *pData = (uint8_t *) pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      pData[i] = data[address + i];
    }
  };
  static void stats() {
    std::cout << "reset chip: " << resetChipCounter << std::endl;
    std::cout << "reset sector: " << resetSectorCounter << std::endl;
    std::cout << "write page: " << writePageCounter << std::endl;
    std::cout << "read page: " << readPageCounter << std::endl;
  }
};

class LogFSRAM : public LogFS {
public:
  RAMIO fio;

  LogFSRAM() : LogFS(&fio) {};
};

class LogFSRAMTest : public LogFSRAM {
public:
  LogFSRAMTest() : LogFSRAM() {
    format();
    init();
  }
};

void testStatus();

void test(const char *name, bool result);

#endif

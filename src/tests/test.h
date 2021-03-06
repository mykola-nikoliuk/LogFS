#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include "../LogFS/LogFS.h"
#include <iostream>

#define SECTOR_SIZE 4096
#define PAGE_SIZE 256
#define MAX_MEMORY_SIZE 16 * SECTOR_SIZE

struct RAMFlashIO : public FlashIO {
  RAMFlashIO(uint32_t capacity = MAX_MEMORY_SIZE, uint16_t sectorSize = SECTOR_SIZE,
             uint16_t pageSize = PAGE_SIZE) : FlashIO(capacity, sectorSize, pageSize) {};

  uint8_t data[MAX_MEMORY_SIZE];

  uint32_t writeByte(uint32_t address, uint8_t value) {
    data[address] = value;
    return address + 1;
  }

  uint8_t readByte(uint32_t address) {
    return data[address];
  }

  virtual void resetChip() {
    for (uint32_t i = 0; i < getCapacity(); i++) {
      data[i] = 0xFF;
    }
  };
  virtual void resetSector(uint32_t sectorIndex) {
    uint32_t address = sectorIndex * getSectorSize();
    uint16_t sectorSize = getSectorSize();
    for (uint32_t i = 0; i < sectorSize; i++) {
      data[address + i] = 0xFF;
    }
  };
  virtual void writePage(uint32_t sectorIndex, uint16_t pageIndex, void* pageData) {
    uint8_t* pData = (uint8_t*)pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      data[address + i] &= pData[i];
    }
  };
  virtual void readPage(uint32_t sectorIndex, uint16_t pageIndex, void* pageData) {
    uint8_t* pData = (uint8_t*)pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      pData[i] = data[address + i];
    }
  };
};

class LogFSRAM : public LogFS {
public:
  RAMFlashIO fio;

  LogFSRAM() : LogFS(&fio) {};
};

class LogFSRAMTest : public LogFSRAM {
public:
  LogFSRAMTest() : LogFSRAM() {
    format();
    init();
  }
};

void test(const char *name, bool result);

#endif

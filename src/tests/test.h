#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include "../LogFS/LogFS.h"

#define MEMORY_SIZE 1024 * 1024
#define SECTOR_SIZE 4096
#define PAGE_SIZE 256

struct RAMFlashIO : public FlashIO {
  RAMFlashIO(uint16_t sectorSize = SECTOR_SIZE, uint16_t pageSize = PAGE_SIZE) :
    FlashIO(sectorSize, pageSize) {};

  uint8_t data[MEMORY_SIZE];

  uint32_t writeByte(uint32_t address, uint8_t value) {
    data[address] = value;
    return address + 1;
  }

  uint8_t readByte(uint32_t address) {
    return data[address];
  }

  virtual void resetChip() {
    // todo: add logic
  };
  virtual void resetSector(uint32_t sectorIndex) {
    // todo: add logic
  };
  virtual void writePage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t *pageData) {
    // todo: add logic
  };
  virtual void readPage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t *pageData) {
    // todo: add logic
  };
};

class LogFSRAM : public LogFS {
public:
  RAMFlashIO fio;

  LogFSRAM() : LogFS(&fio) {};
};

class LogFSRAMTest : public LogFSRAM {
public:
  LogFSRAMTest(uint32_t memorySize = MEMORY_SIZE) : LogFSRAM() {
    formatAndInit(MEMORY_SIZE);
  }

  void formatAndInit(uint32_t memorySize) {
    format(memorySize);
    init();
  }
};

void test(const char *name, bool result);

#endif

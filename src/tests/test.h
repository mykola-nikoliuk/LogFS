#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include "../LogFS/LogFS.h"

#define MEMORY_SIZE 1024 * 1024
#define PAGE_SIZE 512

struct RAMFSIO : public FSIO {
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

class LogFSRAM : public LogFS {
  public:
      RAMFSIO fsio;
      LogFSRAM() : LogFS(&fsio) {};
};

class LogFSRAMTest : public LogFSRAM {
  public:
    LogFSRAMTest(uint16_t filesAmount, uint16_t pageSize, uint32_t memorySize) : LogFSRAM() {
      formatAndInit(filesAmount, pageSize, memorySize);
    }
    LogFSRAMTest(uint16_t filesAmount, uint16_t pageSize) : LogFSRAM() {
      formatAndInit(filesAmount, pageSize, MEMORY_SIZE);
    }
    LogFSRAMTest(uint16_t filesAmount) : LogFSRAM() {
      formatAndInit(filesAmount, PAGE_SIZE, MEMORY_SIZE);
    }
    LogFSRAMTest() : LogFSRAM() {
      formatAndInit(64, PAGE_SIZE, MEMORY_SIZE);
    }

    void formatAndInit(uint8_t filesAmount, uint16_t pageSize, uint32_t memorySize) {
      format(memorySize, pageSize, filesAmount);
      init();
    }
};

void test(const char* name, bool result);

#endif

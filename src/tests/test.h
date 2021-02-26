#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include "../LogFS/LogFS.h"

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

void test(const char* name, bool result);

#endif

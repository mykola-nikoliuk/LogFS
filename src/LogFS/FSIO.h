#ifndef FSIO_H
#define FSIO_H

#include <stdint.h>

struct FSIO {
  virtual uint32_t writeByte(uint32_t address, uint8_t value) = 0;
  uint32_t writeInt(uint32_t address, uint32_t value);
  uint32_t writeShort(uint32_t address, uint16_t value);
  uint32_t writeBytes(uint32_t address, uint8_t* array, uint32_t length);

  virtual uint8_t readByte(uint32_t address) = 0;
  uint32_t readInt(uint32_t address);
  uint16_t readShort(uint32_t address);
  void readBytes(uint32_t address, uint8_t* array, uint32_t length);
};

#endif

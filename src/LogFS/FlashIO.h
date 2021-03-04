#ifndef FlashIO_H
#define FlashIO_H

#include <stdint.h>

struct FlashIO {
  virtual uint32_t writeByte(uint32_t address, uint8_t value) = 0;
  uint32_t writeInt(uint32_t address, uint32_t value);
  uint32_t writeShort(uint32_t address, uint16_t value);
  uint32_t writeBytes(uint32_t address, uint8_t* array, uint32_t length);

  virtual uint8_t readByte(uint32_t address) = 0;
  uint32_t readInt(uint32_t address);
  uint16_t readShort(uint32_t address);
  void readBytes(uint32_t address, uint8_t* array, uint32_t length);

  virtual void resetChip() = 0;
  virtual void resetSector(uint32_t sectorIndex) = 0;
  virtual void writePage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t* pageData) = 0;
  virtual void readPage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t* pageData) = 0;
};

#endif

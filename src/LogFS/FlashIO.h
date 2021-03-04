#ifndef FlashIO_H
#define FlashIO_H

#include <stdint.h>

struct FlashIO {
private:
  uint32_t _capacity;
  uint16_t _pageSize;
  uint16_t _sectorSize;

public:
  FlashIO(uint32_t capacity, uint16_t sectorSize, uint16_t pageSize) {
    _capacity = capacity;
    _sectorSize = sectorSize;
    _pageSize = pageSize;
  }

  void writeBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, uint8_t* data, uint16_t length);
  void readBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, uint8_t* data, uint16_t length);

  virtual void resetChip() = 0;
  virtual void resetSector(uint32_t sectorIndex) = 0;
  virtual void writePage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t* pageData) = 0;
  virtual void readPage(uint32_t sectorIndex, uint16_t pageIndex, uint8_t* pageData) = 0;

  uint16_t getCapacity() { return _capacity; }
  uint16_t getSectorSize() { return _sectorSize; }
  uint16_t getPageSize() { return _pageSize; }

  // TODO: candidates to kill
  virtual uint32_t writeByte(uint32_t address, uint8_t value) = 0;
  uint32_t writeInt(uint32_t address, uint32_t value);
  uint32_t writeShort(uint32_t address, uint16_t value);
  virtual uint8_t readByte(uint32_t address) = 0;
  uint32_t readInt(uint32_t address);
  uint16_t readShort(uint32_t address);
};

#endif

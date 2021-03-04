#include "FlashIO.h"


void FlashIO::writeBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, uint8_t* data, uint16_t length) {
  uint8_t page[_pageSize];
  for (uint32_t i = 0; i < _pageSize; i++) {
    bool isWriteRange = (i >= offset) && (i - offset < length);
    page[i] = isWriteRange ? data[i - offset] : 0xFF;
  }
  writePage(sectorIndex, pageIndex, page);
}

void FlashIO::readBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, uint8_t* data, uint16_t length) {
  uint8_t page[_pageSize];
  readPage(sectorIndex, pageIndex, page);

  for (uint32_t i = offset; i < length + offset; i++) {
    data[i - offset] = page[i];
  }
}

// TODO: candidates to kill
uint32_t FlashIO::writeInt(uint32_t address, uint32_t value) {
  writeByte(address, value >> 24);
  writeByte(address + 1, (value >> 16) & 0x00ff);
  writeByte(address + 2, (value >> 8) & 0x0000ff);
  writeByte(address + 3, value & 0x000000ff);
  return address + 4;
}

uint32_t FlashIO::writeShort(uint32_t address, uint16_t value) {
  writeByte(address, value >> 8);
  writeByte(address + 1, value & 0x00ff);
  return address + 2;
}

uint32_t FlashIO::readInt(uint32_t address) {
  return
    (readByte(address) << 24) +
    (readByte(address + 1) << 16) +
    (readByte(address + 1) << 8) +
    readByte(address + 1);
}

uint16_t FlashIO::readShort(uint32_t address) {
  uint8_t h = readByte(address);
  uint16_t l = readByte(address + 1);
  return (h << 8) | l;
}

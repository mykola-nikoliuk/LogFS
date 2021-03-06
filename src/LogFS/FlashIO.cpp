#include "FlashIO.h"


void FlashIO::writeBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, void* data, uint16_t length) {
  uint8_t page[_pageSize];
  uint8_t* pData = (uint8_t*)data;

  for (uint32_t i = 0; i < _pageSize; i++) {
    bool isWriteRange = (i >= offset) && (i - offset < length);
    page[i] = isWriteRange ? pData[i - offset] : 0xFF;
  }
  writePage(sectorIndex, pageIndex, page);
}

void FlashIO::readBytes(uint32_t sectorIndex, uint16_t pageIndex, uint16_t offset, void* data, uint16_t length) {
  uint8_t page[_pageSize];
  readPage(sectorIndex, pageIndex, page);
  uint8_t* pData = (uint8_t*)data;

  for (uint32_t i = offset; i < length + offset; i++) {
    pData[i - offset] = page[i];
  }
}
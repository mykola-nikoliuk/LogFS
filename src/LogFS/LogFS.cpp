#include <iostream>
#include <string.h>
#include "./LogFS.h"
#include "./Header.h"
#include "./config.h"


uint32_t LogFS::writeShort(uint32_t address, uint16_t value) {
  writeByte(address, value >> 8);
  writeByte(address + 1, value & 0x00ff);
  return address + 2;
}

uint32_t LogFS::writeBytes(uint32_t address, uint8_t* array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    writeByte(address + i, array[i]);
  }
  return address + length;
}

void LogFS::format(uint32_t capacity) {
  LogFS::format(capacity, DEFAULT_PAGE_SIZE, DEFAULT_MAX_FILES_AMOUNT);
}

void LogFS::format(uint32_t capacity, uint16_t pageSize) {
  LogFS::format(capacity, pageSize, DEFAULT_MAX_FILES_AMOUNT);
}

void LogFS::format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount) {
  uint32_t pagesAmount = capacity / pageSize;
  uint16_t maxPageAmountValue = -1;

  LogFSHeader header;
  header.version = 1;
  header.pageSize = pageSize;
  header.pagesAmount = pagesAmount > maxPageAmountValue ? maxPageAmountValue : pagesAmount;
  header.filesAmount = maxFilesAmount;

  std::cout << pageSize << "\n";

  uint32_t address = 0;
  address = writeBytes(address, (uint8_t*)header.name, strlen(header.name));
  address = writeByte(address, header.version);
  address = writeShort(address, header.pageSize);
  address = writeShort(address, header.pagesAmount);
  address = writeShort(address, header.filesAmount);
}

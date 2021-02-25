#include <iostream>
#include <string.h>
#include "./LogFS.h"
#include "./Header.h"
#include "./config.h"

bool LogFS::init() {
  uint32_t address = 0;

  uint8_t fsNameLength = strlen(LogFSHeader::name);
  char fsName[fsNameLength];
  readBytes(address, (uint8_t*)fsName, fsNameLength);
  address += strlen(LogFSHeader::name);

  if (strcmp(fsName, LogFSHeader::name)) {
    std::cout << "Memory is not formatted with LogFS\n";
    return false;
  }


  if (LogFSHeader::version != readByte(address)) {
    std::cout << "Incompatible version, format with current or use previous version of LogFS\n";
    return false;
  }
  address += sizeof(_header.version);

  _header.pageSize = readShort(address);
  address += sizeof(_header.pageSize);
  std::cout << "pageSize: " << _header.pageSize << "\n";

  _header.pagesAmount = readShort(address);
  address += sizeof(_header.pagesAmount);
  std::cout << "pagesAmount: " << _header.pagesAmount << "\n";

  _header.filesAmount = readShort(address);
  std::cout << "filesAmount: " << _header.filesAmount << "\n";

  return true;
}

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

uint16_t LogFS::readShort(uint32_t address) {
  uint8_t h = readByte(address);
  uint16_t l = readByte(address + 1);
  return (h << 8) | l;
}

void LogFS::readBytes(uint32_t address, uint8_t* array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    array[i] = readByte(address + i);
  }
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
  header.pageSize = pageSize;
  header.pagesAmount = pagesAmount > maxPageAmountValue ? maxPageAmountValue : pagesAmount;
  header.filesAmount = maxFilesAmount;

  uint32_t address = 0;
  address = writeBytes(address, (uint8_t*)LogFSHeader::name, strlen(LogFSHeader::name));
  address = writeByte(address, header.version);
  address = writeShort(address, header.pageSize);
  address = writeShort(address, header.pagesAmount);
  address = writeShort(address, header.filesAmount);
}

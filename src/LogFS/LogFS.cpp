#include <string.h>
#include "./LogFS.h"
#include "./Header.h"
#include "./config.h"

LogFS::LogFS(FSIO* fsio) {
  _fsio = fsio;
}

uint8_t LogFS::init() {
  uint32_t address = 0;

  uint8_t fsNameLength = strlen(LogFSHeader::name);
  char fsName[fsNameLength];
  _fsio->readBytes(address, (uint8_t*)fsName, fsNameLength);
  address += strlen(LogFSHeader::name);

  if (strcmp(fsName, LogFSHeader::name)) {
    return LOGFS_ERR_NOT_FORMATTED;
  }

  if (LogFSHeader::version != _fsio->readByte(address)) {
    return LOGFS_ERR_DIFFERENT_VERSION;
  }
  address += sizeof(_header.version);

  _header.pageSize = _fsio->readShort(address);
  address += sizeof(_header.pageSize);

  _header.pagesAmount = _fsio->readShort(address);
  address += sizeof(_header.pagesAmount);

  _header.filesAmount = _fsio->readShort(address);

  return LOGFS_OK;
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
  address = _fsio->writeBytes(address, (uint8_t*)LogFSHeader::name, strlen(LogFSHeader::name));
  address = _fsio->writeByte(address, header.version);
  address = _fsio->writeShort(address, header.pageSize);
  address = _fsio->writeShort(address, header.pagesAmount);
  address = _fsio->writeShort(address, header.filesAmount);
}

#include <string.h>
#include "./LogFS.h"
#include "./File.h"
#include "./Header.h"
#include "./config.h"

#include <iostream>

LogFS::LogFS(FSIO* fsio) {
  _fsio = fsio;
}

uint8_t LogFS::init() {
  _fsio->readBytes(0, (uint8_t*)&_header, sizeof(struct LogFSHeader));

  if (strcmp(_header.name, LogFS_NAME)) {
    return LOGFS_ERR_NOT_FORMATTED;
  }

  if (_header.version != LogFS_VERSION) {
    return LOGFS_ERR_DIFFERENT_VERSION;
  }

  return LOGFS_OK;
}

uint8_t LogFS::format(uint32_t capacity) {
  return LogFS::format(capacity, DEFAULT_PAGE_SIZE, DEFAULT_MAX_FILES_AMOUNT);
}

uint8_t LogFS::format(uint32_t capacity, uint16_t pageSize) {
  return LogFS::format(capacity, pageSize, DEFAULT_MAX_FILES_AMOUNT);
}

uint8_t LogFS::format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount) {
  LogFSHeader header;
  strcpy(header.name, LogFS_NAME);
  header.version = LogFS_VERSION;
  header.pageSize = pageSize;
  header.filesAmount = maxFilesAmount;
  header.filesStartAddress = sizeof(struct LogFSHeader);

  header.pagesMapStartAddress = header.filesStartAddress + sizeof(struct LogFSFile) * uint32_t(maxFilesAmount);
  if (header.pagesMapStartAddress > capacity) return LOGFS_ERR_LOW_SPACE_FILE_TABLE;

  int64_t memoryForPages = capacity - header.pagesMapStartAddress;

  // 9th bit to count pagesMap size;
  int64_t pagesAmount = (uint64_t(memoryForPages) * 8) / (uint64_t(pageSize) * 9);
  if (pagesAmount == 0) return LOGFS_ERR_LOW_SPACE_PAGES;

  header.pagesAmount = pagesAmount;
  header.pagesStartAddress = ceil(pagesAmount / 8.f) + header.pagesMapStartAddress;


  // start format memory
  _fsio->writeBytes(0, (uint8_t*)&header, sizeof(struct LogFSHeader));

  return LOGFS_OK;
}

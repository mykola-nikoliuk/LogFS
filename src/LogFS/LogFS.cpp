#include <string.h>
#include <math.h>
#include "./LogFS.h"
#include "./TableFile.h" // ??
#include "./Header.h" // ??
#include "./config.h"

#include <iostream>

using namespace std;

LogFS::LogFS(FSIO* fsio) {
  _fsio = fsio;
}

uint32_t LogFS::writeEmptyFileTable(uint32_t address, uint16_t filesAmount) {
  uint16_t fileSize = sizeof(struct LogFSTableFile);

  LogFSTableFile file;
  file.isEmpty = true;

  for (uint16_t i = 0; i < filesAmount; i++) {
    _fsio->writeBytes(address + i * fileSize, (uint8_t*)&file, fileSize);
  }

  return address + fileSize * filesAmount;
}

void LogFS::clearPages(uint32_t address, uint16_t pagesAmount) {
  uint32_t zero = 0;
  for (uint16_t i = 0; i < pagesAmount; i++) {
    _fsio->writeInt(
      address + i * _header.pageSize + _header.pageSize - sizeof(zero),
      zero
    );
  }
}

int32_t LogFS::allocatePage() {
  uint32_t address = _header.pagesMapStartAddress;

  for (uint16_t i = 0; i < _header.pagesAmount; i++) {
    uint8_t value = _fsio->readByte(address + i);
    if (value < 255) {
      for (uint8_t j = 0; j < 8; j++) {
        uint8_t shiftIndex = 8 - j - 1;
        uint8_t shift = 1 << shiftIndex;
        bool isPageFree = !(value & shift);
        if (isPageFree) {
          uint16_t pageIndex = i * 8 + j;
          _fsio->writeByte(address + i, value | shift);
          _fsio->writeInt(_header.pagesStartAddress + pageIndex * _header.pageSize, 0);
          _pagesUsed++;
          return pageIndex;
        }
      }
    }
  }

  return -1;
}

uint32_t LogFS::fillTableFile(char* name, LogFSTableFile* tableFile) {
  uint32_t tableFileAddress = 0;
  uint32_t tableFileSize = sizeof(struct LogFSTableFile);

  for (uint16_t i = 0; i < _header.filesAmount; i++) {
    uint32_t address = _header.filesStartAddress + i * tableFileSize;
    if (!_fsio->readByte(address)) {
      _fsio->readBytes(address, (uint8_t*)tableFile, tableFileSize);

      if (strcmp(tableFile->name, name) == 0) {
        tableFileAddress = address;
        break;
      }
    }
  }

  return tableFileAddress;
}

uint32_t LogFS::getPageAddress(uint16_t pageIndex) {
  return _header.pagesStartAddress + _header.pageSize * pageIndex;
}

uint16_t LogFS::getPagesUsed() {
  // TODO: implement
  return 0;
}

// --- PUBLIC ---

uint8_t LogFS::init() {
  _fsio->readBytes(0, (uint8_t*)&_header, sizeof(struct LogFSHeader));

  if (strcmp(_header.name, LogFS_NAME)) {
    return LOGFS_ERR_NOT_FORMATTED;
  }

  if (_header.version != LogFS_VERSION) {
    return LOGFS_ERR_DIFFERENT_VERSION;
  }

  _pagesUsed = getPagesUsed();

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

  header.pagesMapStartAddress = header.filesStartAddress + sizeof(struct LogFSTableFile) * uint32_t(maxFilesAmount);
  if (header.pagesMapStartAddress > capacity) return LOGFS_ERR_LOW_SPACE_FILE_TABLE;

  int64_t memoryForPages = capacity - header.pagesMapStartAddress;

  // 9th bit to count pagesMap size;
  int16_t pagesAmount = (uint64_t(memoryForPages) * 8) / (uint64_t(pageSize) * 9);
  if (pagesAmount == 0) return LOGFS_ERR_LOW_SPACE_PAGES;

  // TODO: check is pageAmount not bigger then sizeof(pageIndex) and raise an error
  header.pagesAmount = pagesAmount;
  header.pagesStartAddress = ceil(pagesAmount / 8.f) + header.pagesMapStartAddress;

  // start format memory
  uint32_t address = 0;
  // header
  address = _fsio->writeBytes(address, (uint8_t*)&header, sizeof(struct LogFSHeader));
  // file table
  address = writeEmptyFileTable(address, maxFilesAmount);
  // files map
  uint8_t zero = 0;
  for (uint16_t i = 0; i < pagesAmount; i++) {
    _fsio->writeByte(address + i, zero);
  }
  address += pagesAmount;

  return LOGFS_OK;
}

LogFSFile LogFS::createFile(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);

  LogFSTableFile tableFile;

  // look for free table file
  uint32_t tableFileAddress = 0;
  uint32_t tableFileSize = sizeof(struct LogFSTableFile);
  for (uint16_t i = 0; i < _header.filesAmount; i++) {
    uint32_t address = _header.filesStartAddress + i * tableFileSize;
    if (_fsio->readByte(address)) {
      _fsio->readBytes(address, (uint8_t*)&tableFile, tableFileSize);

      tableFile.isEmpty = false;
      strcpy(tableFile.name, name);

      tableFileAddress = address;
      break;
    }
  }
  if (!tableFileAddress) return LogFSFile(LOGFS_ERR_LOW_SPACE_FILE_TABLE);

  // look for free page
  uint16_t pageIndex = allocatePage();
  if (pageIndex < 0) return LogFSFile(LOGFS_ERR_LOW_SPACE_PAGES);

  // save table file
  tableFile.firstPageIndex = pageIndex;
  tableFile.lastPageIndex = pageIndex;
  tableFile.lastPageOffset = 0;
  _fsio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, tableFileSize);

  // fill the file structure

  return LogFSFile(this, &tableFile);
}

LogFSFile LogFS::openFile(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);

  LogFSTableFile tableFile;

  // look for table file
  uint32_t tableFileAddress = fillTableFile(name, &tableFile);
  if (!tableFileAddress) return LogFSFile(LOGFS_ERR_FILE_NOT_FOUND);

  // fill the file structure

  return LogFSFile(this, &tableFile);
}

uint8_t LogFS::deleteFile(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LOGFS_ERR_LONG_FILE_NAME;

  LogFSTableFile tableFile;

  // look for table file
  uint32_t tableFileAddress = fillTableFile(name, &tableFile);
  if (!tableFileAddress) return LOGFS_ERR_FILE_NOT_FOUND;

  tableFile.isEmpty = true;
  _fsio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, sizeof(struct LogFSTableFile));

  return LOGFS_OK;
}

bool LogFS::exist(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return false;

  LogFSTableFile tableFile;
  // look for table file
  if(!fillTableFile(name, &tableFile)) return false;

  return true;
}

uint32_t LogFS::getTotalSize() {
  // TODO: check for init

  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
  return _header.pagesAmount * pageBodySize;
}

uint32_t LogFS::getAvailableSize() {
  // TODO: check for init

  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
  uint32_t totalSize = getTotalSize();
  return totalSize - _pagesUsed * pageBodySize;
}

uint32_t LogFS::getUsedSize() {
  // TODO: check for init

  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
  uint32_t totalSize = getTotalSize();
  return totalSize - (_header.pagesAmount - _pagesUsed) * pageBodySize;
}

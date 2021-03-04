#include <string.h>
#include <math.h>
#include "./LogFS.h"
#include "./TableFile.h" // ??
#include "./Header.h" // ??
#include "./config.h"

#include <iostream>

using namespace std;

LogFS::LogFS(FlashIO* fio) {
  _fio = fio;
}
//
//uint32_t LogFS::writeEmptyFileTable(uint32_t address, uint16_t filesAmount) {
//  uint16_t fileSize = sizeof(struct LogFSTableFile);
//
//  LogFSTableFile file;
//  file.isEmpty = true;
//
//  for (uint16_t i = 0; i < filesAmount; i++) {
//    _fio->writeBytes(address + i * fileSize, (uint8_t*)&file, fileSize);
//  }
//
//  return address + fileSize * filesAmount;
//}
//
//void LogFS::clearPages(uint32_t address, uint16_t pagesAmount) {
//  uint32_t zero = 0;
//  for (uint16_t i = 0; i < pagesAmount; i++) {
//    _fio->writeInt(
//      address + i * _header.pageSize + _header.pageSize - sizeof(zero),
//      zero
//    );
//  }
//}
//
//int32_t LogFS::allocatePage() {
//  uint32_t address = _header.pagesMapStartAddress;
//
//  for (uint16_t i = 0; i < _header.pagesAmount; i++) {
//    uint8_t value = _fio->readByte(address + i);
//    if (value < 255) {
//      for (uint8_t j = 0; j < 8; j++) {
//        uint8_t shiftIndex = 8 - j - 1;
//        uint8_t shift = 1 << shiftIndex;
//        bool isPageFree = !(value & shift);
//        if (isPageFree) {
//          uint16_t pageIndex = i * 8 + j;
//          _fio->writeByte(address + i, value | shift);
//          _fio->writeInt(_header.sectorsStartAddress + pageIndex * _header.pageSize, 0);
//          _pagesUsed++;
//          return pageIndex;
//        }
//      }
//    }
//  }
//
//  return -1;
//}
//
//void LogFS::releasePage(uint16_t pageIndex) {
//  _pagesUsed--;
//  uint16_t pageBlockIndex = pageIndex / 8;
//  uint8_t pageBlockBit = 8 - pageIndex % 8 - 1;
//
//  uint8_t state = _fio->readByte(_header.pagesMapStartAddress + pageBlockIndex);
//  state |= 1 << pageBlockBit;
//  _fio->writeByte(_header.pagesMapStartAddress + pageBlockIndex, state);
//}
//
//uint32_t LogFS::fillTableFile(char* name, LogFSTableFile* tableFile) {
//  uint32_t tableFileAddress = 0;
//  uint32_t tableFileSize = sizeof(struct LogFSTableFile);
//
//  for (uint16_t i = 0; i < _header.filesAmount; i++) {
//    uint32_t address = _header.filesStartAddress + i * tableFileSize;
//    if (!_fio->readByte(address)) {
//      _fio->readBytes(address, (uint8_t*)tableFile, tableFileSize);
//
//      if (strcmp(tableFile->name, name) == 0) {
//        tableFileAddress = address;
//        break;
//      }
//    }
//  }
//
//  return tableFileAddress;
//}
//
//uint32_t LogFS::getPageAddress(uint16_t pageIndex) {
//  return _header.sectorsStartAddress + _header.pageSize * pageIndex;
//}

uint16_t LogFS::getPagesUsed() {
  // TODO: implement
  return 0;
}

// --- PUBLIC ---

uint8_t LogFS::init() {
  _fio->readBytes(0, 0, 0, (uint8_t*)&_header, sizeof(struct LogFSHeader));

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
  LogFSHeader header;
  strcpy(header.name, LogFS_NAME);
  header.version = LogFS_VERSION;
  header.sectorSize = _fio->getSectorSize();
  header.pageSize = _fio->getPageSize();
  header.filesStartAddress = header.sectorSize; // 2nd sector

  header.pagesMapStartAddress = header.sectorSize * 2; // third sector
  if (header.pagesMapStartAddress > capacity) return LOGFS_ERR_LOW_SPACE_FILE_TABLE;

  uint32_t pageMapSize = header.sectorSize;
  int64_t memoryForPages = capacity - header.pagesMapStartAddress - pageMapSize;

  // 9th bit to count pagesMap size;
  int16_t pagesAmount = memoryForPages / header.sectorSize;
  if (pagesAmount == 0) return LOGFS_ERR_LOW_SPACE_PAGES;

  header.pagesAmount = pagesAmount;
  header.sectorsStartAddress = header.pagesMapStartAddress + pageMapSize;

  // start format memory
  _fio->resetChip();
//  uint32_t address = 0;
  // header
  _fio->writeBytes(0, 0, 0, (uint8_t*)&header, sizeof(struct LogFSHeader));
  // not need to write table, it's already empty after chip reset
  // same for sectors map

  return LOGFS_OK;
}
//
//LogFSFile LogFS::createFile(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);
//
//  LogFSTableFile tableFile;
//
//  // look for free table file
//  uint32_t tableFileAddress = 0;
//  uint32_t tableFileSize = sizeof(struct LogFSTableFile);
//  for (uint16_t i = 0; i < _header.filesAmount; i++) {
//    uint32_t address = _header.filesStartAddress + i * tableFileSize;
//    if (_fio->readByte(address)) {
//      _fio->readBytes(address, (uint8_t*)&tableFile, tableFileSize);
//
//      tableFile.isEmpty = false;
//      strcpy(tableFile.name, name);
//
//      tableFileAddress = address;
//      break;
//    }
//  }
//  if (!tableFileAddress) return LogFSFile(LOGFS_ERR_LOW_SPACE_FILE_TABLE);
//
//  // look for free page
//  uint16_t pageIndex = allocatePage();
//  if (pageIndex < 0) return LogFSFile(LOGFS_ERR_LOW_SPACE_PAGES);
//
//  // save table file
//  tableFile.firstPageIndex = pageIndex;
//  tableFile.lastPageIndex = pageIndex;
//  tableFile.lastPageOffset = 0;
//  _fio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, tableFileSize);
//
//  // fill the file structure
//
//  return LogFSFile(this, &tableFile, tableFileAddress);
//}
//
//LogFSFile LogFS::openFile(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);
//
//  LogFSTableFile tableFile;
//
//  // look for table file
//  uint32_t tableFileAddress = fillTableFile(name, &tableFile);
//  if (!tableFileAddress) return LogFSFile(LOGFS_ERR_FILE_NOT_FOUND);
//
//  // fill the file structure
//
//  return LogFSFile(this, &tableFile, tableFileAddress);
//}
//
//uint8_t LogFS::deleteFile(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LOGFS_ERR_LONG_FILE_NAME;
//
//  LogFSTableFile tableFile;
//
//  // look for table file
//  uint32_t tableFileAddress = fillTableFile(name, &tableFile);
//  if (!tableFileAddress) return LOGFS_ERR_FILE_NOT_FOUND;
//
//  uint16_t pageIndex = tableFile.firstPageIndex;
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
//
//  while (true) {
//    releasePage(pageIndex);
//    if (pageIndex == tableFile.lastPageIndex) break;
//
//    tableFile.firstPageIndex = pageIndex = _fio->readShort(getPageAddress(pageIndex) + pageBodySize);
//  }
//
//  tableFile.isEmpty = true;
//  _fio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, sizeof(struct LogFSTableFile));
//
//  return LOGFS_OK;
//}
//
//bool LogFS::exist(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return false;
//
//  LogFSTableFile tableFile;
//  // look for table file
//  if(!fillTableFile(name, &tableFile)) return false;
//
//  return true;
//}
//
//uint32_t LogFS::getTotalSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
//  return _header.pagesAmount * pageBodySize;
//}
//
//uint32_t LogFS::getAvailableSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
//  uint32_t totalSize = getTotalSize();
//  return totalSize - _pagesUsed * pageBodySize;
//}
//
//uint32_t LogFS::getUsedSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.pagesAmount);
//  uint32_t totalSize = getTotalSize();
//  return totalSize - (_header.pagesAmount - _pagesUsed) * pageBodySize;
//}
//
//LogFSDirectory LogFS::readFiles() {
//  // TODO: check for init
//  return LogFSDirectory(this);
//}

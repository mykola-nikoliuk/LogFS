#include <string.h>
#include <math.h>
#include "./LogFS.h"
#include "./SectorFlags.h" // ??
#include "./Header.h" // ??
#include "./config.h"

#include <iostream>

using namespace std;

LogFS::LogFS(FlashIO* fio) {
  _fio = fio;
}
//
//uint32_t LogFS::writeEmptyFileTable(uint32_t address, uint16_t filesAmount) {
//  uint16_t fileSize = sizeof(struct LogFSSectorFlags);
//
//  LogFSSectorFlags file;
//  file.isEmpty = true;
//
//  for (uint16_t i = 0; i < filesAmount; i++) {
//    _fio->writeBytes(address + i * fileSize, (uint8_t*)&file, fileSize);
//  }
//
//  return address + fileSize * filesAmount;
//}
//
//void LogFS::clearPages(uint32_t address, uint16_t sectorsAmount) {
//  uint32_t zero = 0;
//  for (uint16_t i = 0; i < sectorsAmount; i++) {
//    _fio->writeInt(
//      address + i * _header.pageSize + _header.pageSize - sizeof(zero),
//      zero
//    );
//  }
//}
//
//int32_t LogFS::allocatePage() {
//  uint32_t address = _header.sectorsMapStartAddress;
//
//  for (uint16_t i = 0; i < _header.sectorsAmount; i++) {
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
//  uint8_t state = _fio->readByte(_header.sectorsMapStartAddress + pageBlockIndex);
//  state |= 1 << pageBlockBit;
//  _fio->writeByte(_header.sectorsMapStartAddress + pageBlockIndex, state);
//}
//
//uint32_t LogFS::fillTableFile(char* name, LogFSSectorFlags* tableFile) {
//  uint32_t tableFileAddress = 0;
//  uint32_t tableFileSize = sizeof(struct LogFSSectorFlags);
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

uint8_t LogFS::format() {
  uint32_t capacity = _fio->getCapacity();
  uint16_t sectorSize = _fio->getSectorSize();
  uint16_t pageSize = _fio->getPageSize();

  uint16_t headerSize = sectorSize;
  uint16_t sectorsMapSize = ceil(capacity / sectorSize / float (sectorSize)) * sectorSize;

  if (capacity < headerSize) return LOGFS_ERR_LOW_SPACE_HEADER;
  if (capacity < headerSize + sectorsMapSize) return LOGFS_ERR_LOW_SPACE_SECTORS_MAP;
  if (capacity < headerSize + sectorsMapSize + sectorSize) return LOGFS_ERR_LOW_SPACE_SECTORS;


  LogFSHeader header;
  strcpy(header.name, LogFS_NAME);
  header.version = LogFS_VERSION;
  header.sectorSize = sectorSize;
  header.pageSize = pageSize;
  header.sectorsMapStartAddress = headerSize;
  header.sectorsStartAddress = header.sectorsMapStartAddress + sectorsMapSize;
  header.sectorsAmount = (capacity - header.sectorsStartAddress) / sectorSize;

  if (header.sectorsMapStartAddress > _fio->getCapacity()) return LOGFS_ERR_LOW_SPACE_HEADER;
  header.sectorsMapStartAddress = header.sectorSize;

  // start format memory
  _fio->resetChip();
  // header
  _fio->writeBytes(0, 0, 0, (uint8_t*)&header, sizeof(struct LogFSHeader));
  // not need to write sectors map, it's already empty after chip reset

  return LOGFS_OK;
}

LogFSFile LogFS::createFile(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);

  LogFSSectorFlags sectorFlags;

  // look for free sector
  uint32_t sectorFlagsSize = sizeof(struct LogFSSectorFlags);
  uint16_t sectorsIndexOffset = _header.sectorsStartAddress / _header.sectorSize;

  uint8_t page[_header.pageSize];
  uint32_t readSectorIndex = 0;
  int32_t readPageIndex = 0;
  int32_t freeSectorIndex = 0;

  for (uint32_t i = 0; i < _header.sectorsAmount; i++) {
    uint32_t sectorIndex =
      i / _header.sectorSize + _header.sectorsMapStartAddress / _header.sectorSize;
    uint16_t pageIndex = i % _header.sectorSize / _header.pageSize;

    if (sectorIndex != readSectorIndex || pageIndex != readPageIndex) {
      _fio->readPage(sectorIndex, pageIndex, page);
      readSectorIndex = sectorIndex;
      readPageIndex = pageIndex;
    }

    uint16_t offset = i % _header.pageSize;
    sectorFlags.flags = page[i];

    if (sectorFlags.isEmpty()) {
      freeSectorIndex = i + sectorsIndexOffset;
      sectorFlags.removeFlags(LOGFS_EMPTY_SECTOR);
      sectorFlags.addFlags(LOGFS_FILE_START_SECTOR);
      _fio->writeBytes(sectorIndex, pageIndex, offset, &sectorFlags, sectorFlagsSize);

      break;
    }
  }

  if (!freeSectorIndex) {
    // TODO: clear used sectors map and check again
    return LogFSFile(LOGFS_ERR_LOW_SPACE_SECTORS);
  }

  // TODO: fill the file structure

  return LogFSFile(this, freeSectorIndex);
}

//LogFSFile LogFS::openFile(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);
//
//  LogFSSectorFlags tableFile;
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
//  LogFSSectorFlags tableFile;
//
//  // look for table file
//  uint32_t tableFileAddress = fillTableFile(name, &tableFile);
//  if (!tableFileAddress) return LOGFS_ERR_FILE_NOT_FOUND;
//
//  uint16_t pageIndex = tableFile.firstPageIndex;
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.sectorsAmount);
//
//  while (true) {
//    releasePage(pageIndex);
//    if (pageIndex == tableFile.lastPageIndex) break;
//
//    tableFile.firstPageIndex = pageIndex = _fio->readShort(getPageAddress(pageIndex) + pageBodySize);
//  }
//
//  tableFile.isEmpty = true;
//  _fio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, sizeof(struct LogFSSectorFlags));
//
//  return LOGFS_OK;
//}
//
//bool LogFS::exist(char* name) {
//  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return false;
//
//  LogFSSectorFlags tableFile;
//  // look for table file
//  if(!fillTableFile(name, &tableFile)) return false;
//
//  return true;
//}
//
//uint32_t LogFS::getTotalSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.sectorsAmount);
//  return _header.sectorsAmount * pageBodySize;
//}
//
//uint32_t LogFS::getAvailableSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.sectorsAmount);
//  uint32_t totalSize = getTotalSize();
//  return totalSize - _pagesUsed * pageBodySize;
//}
//
//uint32_t LogFS::getUsedSize() {
//  // TODO: check for init
//
//  uint16_t pageBodySize = _header.pageSize - sizeof(_header.sectorsAmount);
//  uint32_t totalSize = getTotalSize();
//  return totalSize - (_header.sectorsAmount - _pagesUsed) * pageBodySize;
//}
//
//LogFSDirectory LogFS::readFiles() {
//  // TODO: check for init
//  return LogFSDirectory(this);
//}

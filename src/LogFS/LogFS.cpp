#include <string.h>
#include <math.h>
#include "./LogFS.h"
#include "./SectorFlags.h" // ??
#include "./FileHeader.h" // ??
#include "./Header.h" // ??
#include "./config.h"

#include <iostream>

using namespace std;

LogFS::LogFS(FlashIO *fio, bool autoInit) {
  _fio = fio;
  if (autoInit) init();
}

uint32_t LogFS::getPageIndex(uint32_t offset) {
  return offset / _header.pageSize;
}

uint32_t LogFS::allocateSector(uint8_t flags) {
  LogFSSectorFlags sectorFlags;

  // look for free sector
  uint32_t sectorFlagsSize = sizeof(struct LogFSSectorFlags);
  uint16_t sectorsIndexOffset = _header.sectorsStartAddress / _header.sectorSize;

  uint8_t page[_header.pageSize];
  uint32_t readSectorIndex = 0;
  int32_t readPageIndex = 0;
  uint32_t freeSectorIndex = 0;

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
      sectorFlags.flags = flags;
      _sectorsUsed++;

//      cout << "allocate sector: " << freeSectorIndex << endl;

      _fio->writeBytes(sectorIndex, pageIndex, offset, &sectorFlags, sectorFlagsSize);
      break;
    }
  }

  return freeSectorIndex;
}

void LogFS::releaseSector(uint32_t sectorIndex) {
  if (sectorIndex < _header.sectorsStartAddress / _header.sectorSize) return;

  uint16_t sectorFlagsSize = sizeof(struct LogFSSectorFlags);
  uint16_t flagsPerSector = _header.sectorSize / sectorFlagsSize;
  uint16_t flagsPerPage = _header.pageSize / sectorFlagsSize;

  uint32_t sectorsMapSectorIndex = sectorIndex / flagsPerSector;
  uint32_t sectorsMapPageIndex =
    sectorIndex - (sectorsMapSectorIndex * flagsPerSector) / flagsPerPage;
  uint32_t sectorsMapOffset = (sectorIndex - (sectorsMapSectorIndex * flagsPerSector) -
                               (sectorsMapPageIndex * flagsPerPage)) % _header.pageSize *
                              sectorFlagsSize;

  LogFSSectorFlags sectorFlags;
  sectorFlags.flags = 0;

//  cout << "release sector: " << sectorIndex << endl;

  _sectorsUsed--;
  _fio->writeBytes(sectorsMapSectorIndex, sectorsMapPageIndex, sectorsMapOffset, &sectorFlags,
                   sectorFlagsSize);
  _fio->resetSector(sectorIndex);
}

uint32_t LogFS::readFileHeader(char *name, LogFSFileHeader *fileHeader) {
  LogFSFileHeader localFileHeader;

  uint32_t fileHeaderSize = sizeof(struct LogFSFileHeader);

  LogFSSectorFlags sectorFlags;

  // look for free sector
  uint16_t sectorsIndexOffset = _header.sectorsStartAddress / _header.sectorSize;

  uint8_t page[_header.pageSize];
  uint32_t readSectorIndex = 0;
  int32_t readPageIndex = 0;

  for (uint32_t i = 0; i < _header.sectorsAmount; i++) {
    uint32_t sectorIndex =
      i / _header.sectorSize + _header.sectorsMapStartAddress / _header.sectorSize;
    uint16_t pageIndex = i % _header.sectorSize / _header.pageSize;

    if (sectorIndex != readSectorIndex || pageIndex != readPageIndex) {
      _fio->readPage(sectorIndex, pageIndex, page);
      readSectorIndex = sectorIndex;
      readPageIndex = pageIndex;
    }

    sectorFlags.flags = page[i];

    if (!sectorFlags.isEmpty() && !sectorFlags.isDeleted() && sectorFlags.isFileStart()) {
      uint32_t fileSectorIndex = i + sectorsIndexOffset;

      _fio->readBytes(fileSectorIndex, 0, 0, &localFileHeader, fileHeaderSize);
      if (strcmp(name, localFileHeader.name) == 0) {
        memcpy(fileHeader, &localFileHeader, fileHeaderSize);
        return fileSectorIndex;
      }
    }
  }

  return 0;
}

uint16_t LogFS::getSectorAddressSize() {
  return sizeof(_header.sectorsAmount);
}

uint32_t LogFS::countSectorsUsed() {

  uint32_t sectorsUsed = 0;

  uint16_t pageSize = _header.pageSize;
  uint16_t sectorSize = _header.sectorSize;
  uint16_t sectorFlagsSize = sizeof(struct LogFSSectorFlags);

  uint32_t readSectorMapIndex = 0;
  uint16_t readSectorMapPageIndex = 0;
  uint8_t page[pageSize];
  LogFSSectorFlags sectorFlags;

  for (uint32_t sectorIndex = 0; sectorIndex < _header.sectorsAmount; sectorIndex++) {
    uint32_t sectorMapIndex =
      sectorIndex / (sectorSize / sectorFlagsSize) + _header.sectorsMapStartAddress / sectorSize;
    uint16_t sectorMapPageIndex = sectorIndex / pageSize;
    uint16_t offset = sectorIndex % pageSize;

    if (readSectorMapIndex != sectorMapIndex || readSectorMapPageIndex != sectorMapPageIndex) {
      _fio->readPage(sectorMapIndex, sectorMapPageIndex, page);
      readSectorMapIndex = sectorMapIndex;
      readSectorMapPageIndex = sectorMapPageIndex;
    }

    memcpy(&sectorFlags, &page[offset], sectorFlagsSize);

    if (!sectorFlags.isEmpty() && !sectorFlags.isDeleted()) {
      sectorsUsed++;
    }
  }

  return sectorsUsed;
}


// ======================= PUBLIC =======================


uint8_t LogFS::init() {
  _fio->readBytes(0, 0, 0, (uint8_t * ) & _header, sizeof(struct LogFSHeader));

  if (strcmp(_header.name, LogFS_NAME)) {
    return LOGFS_ERR_NOT_FORMATTED;
  }

  if (_header.version != LogFS_VERSION) {
    return LOGFS_ERR_DIFFERENT_VERSION;
  }

  _sectorsUsed = countSectorsUsed();

  return LOGFS_OK;
}

uint8_t LogFS::format() {
  uint32_t capacity = _fio->getCapacity();
  uint16_t sectorSize = _fio->getSectorSize();
  uint16_t pageSize = _fio->getPageSize();

  uint16_t headerSize = sectorSize;
  uint16_t sectorsMapSize = ceil(capacity / sectorSize / float(sectorSize)) * sectorSize;

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
  _fio->writeBytes(0, 0, 0, (uint8_t * ) & header, sizeof(struct LogFSHeader));
  // not need to write sectors map, it's already empty after chip reset

  return LOGFS_OK;
}

LogFSFile LogFS::createFile(char *name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);
  if (exist(name)) return LOGFS_ERR_FILE_ALREADY_EXIST;

  // look for free sector
  int32_t freeSectorIndex = allocateSector(LOGFS_ACTIVE_SECTOR | LOGFS_FILE_START_SECTOR);

  if (!freeSectorIndex) {
    // TODO: clear used sectors map and check again
    return LogFSFile(LOGFS_ERR_LOW_SPACE_SECTORS);
  }

  LogFSFileHeader fileHeader;
  uint16_t fileHeaderSize = sizeof(struct LogFSFileHeader);
  strcpy(fileHeader.name, name);

  _fio->writeBytes(freeSectorIndex, 0, 0, &fileHeader, fileHeaderSize);

  // TODO: fill the file structure

  return LogFSFile(this, freeSectorIndex);
}

LogFSFile LogFS::openFile(char *name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LogFSFile(LOGFS_ERR_LONG_FILE_NAME);

  LogFSFileHeader fileHeader;

  // look for table file
  uint32_t sectorIndex = readFileHeader(name, &fileHeader);
  if (!sectorIndex) return LogFSFile(LOGFS_ERR_FILE_NOT_FOUND);

  // fill the file structure

  return LogFSFile(this, sectorIndex);
}

uint8_t LogFS::deleteFile(char *name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return LOGFS_ERR_LONG_FILE_NAME;

  LogFSFileHeader fileHeader;

  // look for table file
  uint32_t sectorIndex = readFileHeader(name, &fileHeader);
  if (!sectorIndex) return LOGFS_ERR_FILE_NOT_FOUND;

  uint8_t sectorAddressSize = getSectorAddressSize();
  uint16_t sectorBodySize = _header.sectorSize - sectorAddressSize;

  uint16_t previousSectorIndex = sectorIndex;
  uint32_t nextSectorIndex;
  uint32_t maxAddressValue = -1;

  while (true) {
    _fio->readBytes(
      previousSectorIndex,
      sectorBodySize / _header.pageSize,
      sectorBodySize % _header.pageSize,
      &nextSectorIndex,
      sectorAddressSize
    );
    releaseSector(previousSectorIndex);

    if (nextSectorIndex == maxAddressValue) {
      break;
    }

    previousSectorIndex = nextSectorIndex;
  }

  return LOGFS_OK;
}

bool LogFS::exist(char *name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return false;

  LogFSFileHeader fileHeader;

  // look for table file
  uint32_t sectorIndex = readFileHeader(name, &fileHeader);

  return sectorIndex > 0;
}

uint32_t LogFS::getTotalSize() {
  // TODO: check for init

  return _header.sectorsAmount * (_header.sectorSize - getSectorAddressSize());
}

uint32_t LogFS::getAvailableSize() {
  // TODO: check for init

  return (_header.sectorsAmount - _sectorsUsed) * (_header.sectorSize - getSectorAddressSize());
}

uint32_t LogFS::getUsedSize() {
  // TODO: check for init

  return _sectorsUsed * (_header.sectorSize - getSectorAddressSize());
}

LogFSDirectory LogFS::readFiles() {
  // TODO: check for init
  return LogFSDirectory(this);
}

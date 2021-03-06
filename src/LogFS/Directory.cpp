#include <string.h>
#include "LogFS.h"
#include "Directory.h"


LogFSDirectory::LogFSDirectory(LogFS *fs) {
  _fs = fs;
  _status = LOGFS_OK;
  _lastSectorIndex = 0;
}

uint8_t LogFSDirectory::next(char *filename) {
  if (_status) return _status;

  LogFSHeader *header = &_fs->_header;

  uint16_t pageSize = header->pageSize;
  uint16_t sectorsAmount = header->sectorsAmount;
  uint16_t sectorsMapIndexOffset = header->sectorsMapStartAddress / header->sectorSize;
  uint16_t sectorsIndexOffset = header->sectorsStartAddress / header->sectorSize;
  uint16_t lastPageIndex = _fs->_header.sectorSize / _fs->_header.pageSize - 1;
  uint16_t sectorAddressSize = sizeof(struct LogFSSectorFlags);

  for (; _lastSectorIndex < sectorsAmount; _lastSectorIndex++) {
    LogFSSectorFlags sectorFlags;

    uint16_t pageOffset = _lastSectorIndex % pageSize;
    for (uint16_t pageIndex = _lastSectorIndex / pageSize; pageIndex < lastPageIndex; pageIndex++) {

      uint8_t page[pageSize];
      uint16_t sectorsMapSectorIndex = _lastSectorIndex / (sectorsAmount / sectorAddressSize);
      _fs->_fio->readPage(sectorsMapSectorIndex + sectorsMapIndexOffset, pageIndex, &page);

      for (; pageOffset < pageSize; pageOffset += sectorAddressSize) {
        memcpy(&sectorFlags, &page[pageOffset], sectorAddressSize);

        if (!sectorFlags.isEmpty() && !sectorFlags.isDeleted() && sectorFlags.isFileStart()) {
          LogFSFileHeader fileHeader;
          _fs->_fio->readBytes(_lastSectorIndex + sectorsIndexOffset, 0, 0, &fileHeader, sizeof(struct LogFSFileHeader));
          strcpy(filename, fileHeader.name);

          _lastSectorIndex++;

          return LOGFS_OK;
        }
      }

      pageOffset = 0;
    }
  }

  return LOGFS_ERR_FILE_NOT_FOUND;
}

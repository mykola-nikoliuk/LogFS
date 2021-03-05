#include "math.h"
#include "LogFS.h"
#include "File.h"
#include "config.h"


LogFSFile::LogFSFile(LogFS *fs, uint32_t sectorIndex) {
  _fs = fs;
  _status = LOGFS_OK;
  _startSectorIndex = sectorIndex;

// TODO: should be filled
  _lastSectorIndex = 0;
  _lastSectorOffset = _fs->_header.sectorSize - sizeof(_fs->_header.sectorsAmount);
  _readPageIndex = 0;
  _readPageOffset = 0;
}

void LogFSFile::setLastSectorOffset(uint16_t offset) {
  // TODO: implement
}

uint8_t LogFSFile::write(uint8_t *data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorSize = _fs->_header.sectorSize;
  uint16_t sectorAddressSize = sizeof(_fs->_header.sectorsAmount);
  uint16_t sectorBodySize = sectorSize - sectorAddressSize;

  // TODO: check memory is enough for write
  // return LOGFS_ERR_LOW_SPACE_PAGES;

  uint16_t lastPageIndex = _fs->_header.sectorSize / pageSize - 1;
  uint16_t startPageIndex = _fs->getPageIndex(_lastSectorOffset);
  uint16_t endPageIndex = _fs->getPageIndex(fmin(_lastSectorOffset + length, sectorBodySize));

  uint16_t pageOffset = _lastSectorOffset % pageSize;
  uint32_t totalWrote = 0;
  for (uint16_t pageIndex = startPageIndex; pageIndex <= endPageIndex; pageIndex++) {
    uint16_t pageAvailableSize = pageIndex == lastPageIndex
                                 ? pageSize - pageOffset - sectorAddressSize
                                 : pageSize - pageOffset;
    uint16_t writeLength = fmin(pageAvailableSize, length);

    setLastSectorOffset(_lastSectorOffset + totalWrote + writeLength);
    _fs->_fio->writeBytes(
      _lastSectorIndex,
      pageIndex,
      pageOffset,
      &data[totalWrote],
      writeLength
    );

    totalWrote += writeLength;
    pageOffset = 0;
  }

  // if data didn't fit current sector
  if (length > totalWrote) {
    setLastSectorOffset(0);
    uint32_t newSectorIndex = _fs->allocateSector(LOGFS_ACTIVE_SECTOR);
    _fs->_fio->writeBytes(
      _lastSectorIndex,
      lastPageIndex,
      pageSize - sectorAddressSize,
      &newSectorIndex,
      sectorAddressSize
    );
    _lastSectorIndex = newSectorIndex;

    write(&data[totalWrote], length - totalWrote);
  }

  return LOGFS_OK;
}

//uint8_t LogFSFile::read(uint8_t* data, uint32_t length) {
//  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;
//
//  uint16_t sectorBodySize = _fs->_header.pageSize - sizeof(_fs->_header.sectorsAmount);
//  uint16_t sizeToEndOfSector = sectorBodySize - readPageOffset;
//
//
//  uint32_t address = _fs->getPageAddress(readPageIndex) + readPageOffset;
//  _fs->_fio->readBytes(address, data, fmin(length, sizeToEndOfSector));
//
//  if (length > sizeToEndOfSector) {
//    readPageOffset = 0;
//    readPageIndex = _fs->_fio->readShort(_fs->getPageAddress(readPageIndex) + sectorBodySize);
//
//    read(data + sizeToEndOfSector, length - sizeToEndOfSector);
//  }
//  else {
//    readPageOffset = length;
//  }
//
//  return LOGFS_OK;
//}
//
//uint32_t LogFSFile::size() {
//  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;
//
//  uint16_t pagesCount = 0;
//  uint16_t pageIndex = _tableFile.firstPageIndex;
//  uint16_t sectorBodySize = _fs->_header.pageSize - sizeof(_fs->_header.sectorsAmount);
//
//  while (pageIndex != _tableFile.lastPageIndex) {
//    uint32_t address = _fs->getPageAddress(pageIndex) + sectorBodySize;
//    pageIndex = _fs->_fio->readShort(address);
//    pagesCount++;
//  }
//
//  return _tableFile.lastPageOffset + sectorBodySize * pagesCount;
//}
//
//uint8_t LogFSFile::close() {
//  _status = LOGFS_FILE_CLOSED;
//  _fs->_fio->writeBytes(_tableFileAddress, (uint8_t*)&_tableFile, sizeof(struct LogFSSectorFlags));
//
//  return LOGFS_OK;
//}

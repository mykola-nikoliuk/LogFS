#include "math.h"
#include "LogFS.h"
#include "File.h"
#include "FileHeader.h"
#include "config.h"

#include "iostream"

using namespace std;

void LogFSFile::setLastSectorOffset(uint16_t offset) {
  _lastSectorOffset = offset;

  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorSize = _fs->_header.sectorSize;
  uint16_t sectorOffsetSize = sizeof(_lastSectorOffset);
  uint16_t lastAvailableAddress = sectorSize - sectorSize - sectorOffsetSize;

  if (_EOFOffsetAddress > lastAvailableAddress) {
    _EOFOffsetAddress = sizeof(struct LogFSFileHeader);
    // TODO: clear addresses
  }

  _fs->_fio->writeBytes(
    _startSectorIndex,
    _EOFOffsetAddress / pageSize,
    _EOFOffsetAddress % pageSize,
    &_lastSectorOffset,
    sectorOffsetSize
  );
  _EOFOffsetAddress += sectorOffsetSize;
}

void LogFSFile::fillEOFOffset() {
  uint16_t offset = sizeof(struct LogFSFileHeader);
  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorOffsetSize = sizeof(_lastSectorOffset);
  uint16_t maxSectorOffset = -1;
  uint16_t sectorSize = _fs->_header.sectorSize;
  uint16_t sectorAddressSize = _fs->getSectorAddressSize();

  uint8_t page[pageSize];
  int16_t readPage = -1;
  uint16_t pageIndex = offset / _fs->_header.pageSize;

  for (; offset < sectorSize - sectorAddressSize; offset += sectorOffsetSize) {
    if (pageIndex != readPage) {
      _fs->_fio->readPage(_startSectorIndex, pageIndex, page);
      readPage = pageIndex;
    }

    uint16_t lastSectorOffset;
    memcpy(&lastSectorOffset, &page[offset], sectorOffsetSize);

    if (lastSectorOffset == maxSectorOffset) return;

    _EOFOffsetAddress = offset + sectorOffsetSize;
    _lastSectorOffset = lastSectorOffset;
  }
}

void LogFSFile::fillEOFSector() {
  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorAddressSize = _fs->getSectorAddressSize();

  uint32_t maxPageIndex = _fs->_header.sectorSize / pageSize - 1;
  uint32_t maxSectorIndex = -1;
  uint32_t sectorIndex = _startSectorIndex;

  while (sectorIndex != maxSectorIndex) {
    _lastSectorIndex = sectorIndex;
    _fs->_fio->readBytes(
      sectorIndex,
      maxPageIndex,
      pageSize - sectorAddressSize,
      &sectorIndex,
      sectorAddressSize
    );
  }
}

LogFSFile::LogFSFile(LogFS *fs, uint32_t sectorIndex) {
  _fs = fs;
  _status = LOGFS_OK;
  _startSectorIndex = sectorIndex;

  _lastSectorIndex = sectorIndex;
  _lastSectorOffset = _fs->_header.sectorSize - _fs->getSectorAddressSize();
  _readSectorIndex = sectorIndex;
  _readSectorOffset = _lastSectorOffset;
  _EOFOffsetAddress = sizeof(struct LogFSFileHeader);
  fillEOFOffset();
  fillEOFSector();
}

uint8_t LogFSFile::write(void *data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint8_t *pData = (uint8_t *) data;

  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorSize = _fs->_header.sectorSize;
  uint16_t sectorAddressSize = _fs->getSectorAddressSize();
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
    uint16_t writeLength = fmin(pageAvailableSize, length - totalWrote);

    if (writeLength > 0) {
      setLastSectorOffset(_lastSectorOffset + writeLength);

//      cout << endl << "write" << endl;
//      cout << "sector: " << _lastSectorIndex << endl;
//      cout << "page: " << pageIndex << endl;
//      cout << "offset: " << pageOffset << endl;
//      cout << "len: " << writeLength << endl << endl;

      _fs->_fio->writeBytes(
        _lastSectorIndex,
        pageIndex,
        pageOffset,
        &pData[totalWrote],
        writeLength
      );

      totalWrote += writeLength;
    }

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

//    cout << endl << "add new sector: " << newSectorIndex << endl;

    write(&pData[totalWrote], length - totalWrote);
  }

  return LOGFS_OK;
}

uint8_t LogFSFile::read(void *data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint8_t *pData = (uint8_t *) data;

  uint16_t pageSize = _fs->_header.pageSize;
  uint16_t sectorSize = _fs->_header.sectorSize;
  uint16_t sectorAddressSize = _fs->getSectorAddressSize();
  uint16_t sectorBodySize = sectorSize - sectorAddressSize;

  uint16_t lastPageIndex = _fs->_header.sectorSize / pageSize - 1;
  uint16_t startPageIndex = _fs->getPageIndex(_readSectorOffset);
  uint16_t endPageIndex = _fs->getPageIndex(fmin(_readSectorOffset + length, sectorBodySize));

  uint16_t pageOffset = _readSectorOffset % pageSize;
  uint32_t totalRead = 0;
  for (uint16_t pageIndex = startPageIndex; pageIndex <= endPageIndex; pageIndex++) {
    uint16_t pageAvailableSize = pageIndex == lastPageIndex
                                 ? pageSize - pageOffset - sectorAddressSize
                                 : pageSize - pageOffset;
    uint16_t readLength = fmin(pageAvailableSize, length - totalRead);

    if (readLength > 0) {
//      cout << endl << "read" << endl;
//      cout << "sector: " << _readSectorIndex << endl;
//      cout << "page: " << pageIndex << endl;
//      cout << "offset: " << pageOffset << endl;
//      cout << "len: " << readLength << endl << endl;

      _readSectorOffset += readLength;
      _fs->_fio->readBytes(
        _readSectorIndex,
        pageIndex,
        pageOffset,
        &pData[totalRead],
        readLength
      );
      totalRead += readLength;
    }

    pageOffset = 0;
  }

  if (length > totalRead) {
    _readSectorOffset = 0;

//    cout << endl << "read new sector index" << endl;
//    cout << "sector: " << _readSectorIndex << endl;
//    cout << "page: " << lastPageIndex << endl;
//    cout << "offset: " << pageSize - sectorAddressSize << endl;
//    cout << "len: " << sectorAddressSize << endl << endl;

    uint32_t nextSectorIndex;
    _fs->_fio->readBytes(
      _readSectorIndex,
      lastPageIndex,
      pageSize - sectorAddressSize,
      &nextSectorIndex,
      sectorAddressSize
    );
    _readSectorIndex = nextSectorIndex;

    read(&pData[totalRead], length - totalRead);
  }

  return LOGFS_OK;
}

uint32_t LogFSFile::size() {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint16_t sectorsCount = 0;
  uint16_t lastPageIndex = _fs->_header.sectorSize / _fs->_header.pageSize - 1;
  uint16_t sectorAddressSize = _fs->getSectorAddressSize();
  uint16_t sectorIndex = _startSectorIndex;
  uint16_t sectorBodySize = _fs->_header.sectorSize - sectorAddressSize;
  uint16_t pageBodySize = _fs->_header.pageSize - sectorAddressSize;

  while (sectorIndex != _lastSectorIndex) {
    uint32_t address;
    _fs->_fio->readBytes(sectorIndex, lastPageIndex, pageBodySize, &address, sectorAddressSize);
    sectorIndex = address;
    sectorsCount++;
  }

  return _lastSectorOffset + sectorBodySize * (sectorsCount - 1);
}

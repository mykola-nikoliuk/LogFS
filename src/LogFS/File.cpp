#include "math.h"
#include "LogFS.h"
#include "File.h"
#include "config.h"

using namespace std;

uint8_t LogFSFile::write(uint8_t* data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint16_t pageBodySize = _fs->_header.pageSize - sizeof(_fs->_header.pagesAmount);

  uint16_t sizeToEndOfPage = pageBodySize - _tableFile.lastPageOffset;

  uint16_t writeLength = fmin(sizeToEndOfPage, length);
  uint32_t address = _fs->getPageAddress(_tableFile.lastPageIndex) + _tableFile.lastPageOffset;
  _fs->_fsio->writeBytes(address, data, fmin(sizeToEndOfPage, writeLength));
  _tableFile.lastPageOffset += writeLength;


  if (length > sizeToEndOfPage) {
    uint16_t dataLength = length - sizeToEndOfPage;
    uint16_t pagesNeeded = ceil(dataLength / float(pageBodySize));
    uint16_t pageIndexes[pagesNeeded];

    for (uint16_t i = 0; i < pagesNeeded; i++) {
      int32_t pageIndex = _fs->allocatePage();

      if (pageIndex < 0) {
        for (uint16_t j = i - 1; j >= 0; j--) {
          _fs->releasePage(pageIndexes[j]);
        }
        return LOGFS_ERR_LOW_SPACE_PAGES;
      }

      pageIndexes[i] = pageIndex;
    }

    uint32_t offset = sizeToEndOfPage;
    for (uint16_t i = 0; i < pagesNeeded; i++) {
      _fs->_fsio->writeShort(_fs->getPageAddress(_tableFile.lastPageIndex) + pageBodySize, pageIndexes[i]);
      _tableFile.lastPageOffset = 0;
      _tableFile.lastPageIndex = pageIndexes[i];
      write(data + offset, fmin(pageBodySize, length - offset));
      offset += pageBodySize;
    }
  }

  return LOGFS_OK;
}

uint8_t LogFSFile::read(uint8_t* data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint16_t pageBodySize = _fs->_header.pageSize - sizeof(_fs->_header.pagesAmount);
  uint16_t sizeToEndOfPage = pageBodySize - readPageOffset;


  uint32_t address = _fs->getPageAddress(readPageIndex) + readPageOffset;
  _fs->_fsio->readBytes(address, data, fmin(length, sizeToEndOfPage));

  if (length > sizeToEndOfPage) {
    readPageOffset = 0;
    readPageIndex = _fs->_fsio->readShort(_fs->getPageAddress(readPageIndex) + pageBodySize);

    read(data + sizeToEndOfPage, length - sizeToEndOfPage);
  }
  else {
    readPageOffset = length;
  }

  return LOGFS_OK;
}

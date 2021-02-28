#include "LogFS.h"
#include "File.h"
#include "config.h"

uint8_t LogFSFile::write(uint8_t* data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint32_t address = _fs->getPageAddress(_tableFile.lastPageIndex) + _tableFile.lastPageOffset;
  _fs->_fsio->writeBytes(address, data, length);

  return LOGFS_OK;
}

uint8_t LogFSFile::read(uint8_t* data, uint32_t length) {
  if (_status) return LOGFS_ERR_FILE_NOT_OPENED;

  uint32_t address = _fs->getPageAddress(readPageIndex) + readPageOffset;
  _fs->_fsio->readBytes(address, data, length);

  return LOGFS_OK;
}

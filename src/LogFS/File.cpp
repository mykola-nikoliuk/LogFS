#include "File.h"
#include "config.h"

uint8_t LogFSFile::writeBytes(uint8_t* data, uint32_t length) {
  if (!_fsio) return LOGFS_ERR_FILE_NOT_OPENED;

  return LOGFS_OK;
}

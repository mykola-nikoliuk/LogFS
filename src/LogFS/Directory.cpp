#include <string.h>
#include "LogFS.h"
#include "Directory.h"

uint8_t LogFSDirectory::next(char* filename) {
  if (_status) return _status;

  uint16_t filesAmount = _fs->_header.filesAmount;


  for (uint16_t i = _lastFileIndex; i < filesAmount; i++) {
    uint32_t address = _fs->_header.filesStartAddress + i * sizeof(struct LogFSTableFile);
    uint8_t isEmpty = _fs->_fsio->readByte(address);

    if (!isEmpty) {
      LogFSTableFile tableFile;
      _fs->_fsio->readBytes(address, (uint8_t*)&tableFile, sizeof(struct LogFSTableFile));
      strcpy(filename, tableFile.name);
      _lastFileIndex = i + 1;

      return LOGFS_OK;
    }
  }

  return LOGFS_ERR_FILE_NOT_FOUND;
}

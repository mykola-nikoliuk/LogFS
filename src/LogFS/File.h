#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include <stdint.h>
#include "FSIO.h"
#include "TableFile.h"
#include "config.h"

class LogFS;

class LogFSFile {
  private:
    LogFS* _fs;
    uint8_t _status;
    LogFSTableFile _tableFile;

  public:
    LogFSFile(LogFS* fs, LogFSTableFile* tableFile) {
      _fs = fs;
      _tableFile = tableFile;
      _status = LOGFS_OK;
    }
    LogFSFile(uint8_t status) {
      _status = status;
    }
    uint8_t getStatus() { return _status; }
    uint8_t writeBytes(uint8_t* data, uint32_t length);
};

#endif

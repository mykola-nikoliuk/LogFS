#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include <stdint.h>
#include "FSIO.h"
#include "config.h"

class LogFS;

class LogFSFile {
  private:
    LogFS* _fs;
    uint8_t _status;

  public:
    LogFSFile(LogFS* fs) {
      _fs = fs;
      _status = LOGFS_OK;
    }
    LogFSFile(uint8_t status) {
      _status = status;
    }
    uint8_t getStatus() { return _status; }
    uint8_t writeBytes(uint8_t* data, uint32_t length);
};

#endif

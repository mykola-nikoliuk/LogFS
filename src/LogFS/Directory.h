#ifndef LOGFS_DIRECTORY_H
#define LOGFS_DIRECTORY_H

#include <stdint.h>
#include "FlashIO.h"
#include "SectorFlags.h"
#include "config.h"

class LogFS;

class LogFSDirectory {
  private:
    LogFS* _fs;
    uint8_t _status;
    uint16_t _lastFileIndex;

  public:
    LogFSDirectory(LogFS* fs) {
      _fs = fs;
      _status = LOGFS_OK;
      _lastFileIndex = 0;
    }
    LogFSDirectory(uint8_t status) {
      _status = status;
    }
    uint8_t getStatus() { return _status; }
    uint8_t next(char* filename);
};

#endif

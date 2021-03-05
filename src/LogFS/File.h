#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include <stdint.h>
#include "FlashIO.h"
#include "SectorFlags.h"
#include "config.h"

class LogFS;

class LogFSFile {
  private:
    LogFS* _fs;
    uint8_t _status;
    uint32_t _startSectorIndex;

    uint16_t readPageIndex;
    uint16_t readPageOffset;

  public:
    LogFSFile(LogFS* fs, uint32_t sectorIndex) {
      _fs = fs;
      _status = LOGFS_OK;
      _startSectorIndex = sectorIndex;
    }
    LogFSFile(uint8_t status) {
      _status = status;
    }
    uint8_t getStatus() { return _status; }
    uint8_t write(uint8_t* data, uint32_t length);
    uint8_t read(uint8_t* data, uint32_t length);
    uint32_t size();
    uint8_t close();
};

#endif

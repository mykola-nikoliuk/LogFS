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
    uint32_t _tableFileAddress;
    uint16_t readPageIndex;
    uint16_t readPageOffset;

  public:
    LogFSFile(LogFS* fs, LogFSTableFile* tableFile, uint32_t tableFileAddress) {
      _fs = fs;
      _tableFile = tableFile;
      _tableFileAddress = tableFileAddress;
      _status = LOGFS_OK;
      readPageIndex = _tableFile.firstPageIndex;
      readPageOffset = 0;
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

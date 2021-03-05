#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include <stdint.h>
#include "FlashIO.h"
#include "SectorFlags.h"
#include "config.h"

class LogFS;

class LogFSFile {
private:
  LogFS *_fs;
  uint8_t _status;
  uint32_t _startSectorIndex;

  uint32_t _lastSectorIndex;
  uint16_t _lastSectorOffset;

  uint16_t _readSectorIndex;
  uint16_t _readSectorOffset;

  void setLastSectorOffset(uint16_t offset);

public:
  LogFSFile(LogFS *fs, uint32_t sectorIndex);
  LogFSFile(uint8_t status) {
    _status = status;
  }
  uint8_t getStatus() { return _status; }
  uint8_t write(void* data, uint32_t length);
  uint8_t read(void* data, uint32_t length);
  uint32_t size();
  uint8_t close();
};

#endif

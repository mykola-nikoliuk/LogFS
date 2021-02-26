#ifndef LOGFS_H
#define LOGFS_H

#include "FSIO.h"
#include "Header.h"
#include <stdint.h>

class LogFS {

  private:
    LogFSHeader _header;
    FSIO* _fsio;

  public:
    LogFS(FSIO* fsio);

    uint8_t init();
    void format(uint32_t capacity);
    void format(uint32_t capacity, uint16_t pageSize);
    void format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount);

    uint16_t pageSize() {
      return _header.pageSize;
    }
    uint16_t pagesAmount() {
      return _header.pagesAmount;
    }
    uint16_t filesAmount() {
      return _header.filesAmount;
    }
};


enum {
  LOGFS_OK = 0,
  LOGFS_ERR_NOT_FORMATTED,
  LOGFS_ERR_DIFFERENT_VERSION
};

#endif

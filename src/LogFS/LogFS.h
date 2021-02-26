#ifndef LOGFS_H
#define LOGFS_H

#include "FSIO.h"
#include "Header.h"
#include <stdint.h>

class LogFS {

  private:
    LogFSHeader _header;
    FSIO* _fsio;
    uint32_t writeEmptyFileTable(uint32_t address, uint16_t filesAmount);
    void clearPages(uint32_t address, uint16_t pagesAmount);

  public:
    LogFS(FSIO* fsio);

    uint8_t init();
    uint8_t format(uint32_t capacity);
    uint8_t format(uint32_t capacity, uint16_t pageSize);
    uint8_t format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount);

    LogFSHeader* getHeader() {
      return &_header;
    }
};


enum {
  LOGFS_OK = 0,
  LOGFS_ERR_NOT_FORMATTED,
  LOGFS_ERR_DIFFERENT_VERSION,
  LOGFS_ERR_LOW_SPACE_FILE_TABLE,
  LOGFS_ERR_LOW_SPACE_PAGES
};

#endif

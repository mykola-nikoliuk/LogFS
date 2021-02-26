#ifndef LOGFS_H
#define LOGFS_H

#include "Header.h"
#include <stdint.h>

class LogFS {

  private:
    LogFSHeader _header;

  protected:
    virtual uint32_t writeByte(uint32_t address, uint8_t value) = 0;
    uint32_t writeInt(uint32_t address, uint32_t value);
    uint32_t writeShort(uint32_t address, uint16_t value);
    uint32_t writeBytes(uint32_t address, uint8_t* array, uint32_t length);

    virtual uint8_t readByte(uint32_t address) = 0;
    uint32_t readInt(uint32_t address);
    uint16_t readShort(uint32_t address);
    void readBytes(uint32_t address, uint8_t* array, uint32_t length);

  public:
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

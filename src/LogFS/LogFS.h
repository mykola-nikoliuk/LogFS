#ifndef LOGFS_H
#define LOGFS_H

//#include "Header.h"
#include <stdint.h>

class LogFS {

//  private:
//    LogFSHeader _header;

  protected:
    virtual uint32_t writeByte(uint32_t address, uint8_t value) = 0;
    uint32_t writeShort(uint32_t address, uint16_t value);
    uint32_t writeBytes(uint32_t address, uint8_t* array, uint32_t length);

  public:
    void format(uint32_t capacity);
    void format(uint32_t capacity, uint16_t pageSize);
    void format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount);
};

#endif

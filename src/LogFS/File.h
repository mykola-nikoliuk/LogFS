#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include <stdint.h>
#include "FSIO.h"

struct LogFSFile {
  private:
    FSIO* _fsio;

  public:
    LogFSFile() {
      _fsio = 0;
    }
    uint8_t writeBytes(uint8_t* data, uint32_t length);
};

#endif

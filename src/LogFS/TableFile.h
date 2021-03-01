#ifndef LOGFS_TABLE_FILE_H
#define LOGFS_TABLE_FILE_H

#include <string.h>
#include <stdint.h>
#include "config.h"

struct LogFSTableFile {
  bool isEmpty;
  char name[LogGS_FILE_NAME_LENGTH];
  uint16_t firstPageIndex;
  uint16_t lastPageIndex;
  uint16_t lastPageOffset;

  LogFSTableFile() {
    firstPageIndex = 0;
    lastPageIndex = 0;
    lastPageOffset = 0;
  }

  LogFSTableFile* operator=(LogFSTableFile* tableFile) {
    memcpy(this, tableFile, sizeof(struct LogFSTableFile));
    return tableFile;
  }
};

#endif

#ifndef LOGFS_TABLE_FILE_H
#define LOGFS_TABLE_FILE_H

#include <stdint.h>
#include "config.h"

struct LogFSTableFile {
  bool isEmpty;
  char name[LogGS_FILE_NAME_LENGTH];
  uint16_t firstPageIndex;
  uint16_t lastPageOffset;
};

#endif

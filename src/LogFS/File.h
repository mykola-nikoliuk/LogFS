#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

#include "stdint.h"

struct LogFSFile {
  char name[13];
  bool isEmpty;
  uint16_t firstPageIndex;
  uint16_t lastPageOffset;
};

#endif

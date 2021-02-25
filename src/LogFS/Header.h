#ifndef LOGFS_HEADER_H
#define LOGFS_HEADER_H

#include <stdint.h>

/* HEADER SIZE 32 bytes
  MAX CAPACITY: maxPages(32768) * maxPageSize(32768) = 1073741824 = 1Gb
*/

struct LogFSHeader {
  static char name[];
  static uint8_t version;
  uint16_t pageSize;    // min 512, max 32768
  uint16_t pagesAmount;
  uint16_t filesAmount;
};

#endif

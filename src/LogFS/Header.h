#ifndef LOGFS_HEADER_H
#define LOGFS_HEADER_H

#include <stdint.h>

/* HEADER SIZE 32 bytes
  MAX CAPACITY: maxPages(32768) * maxPageSize(32768) = 1073741824 = 1Gb
*/

struct LogFSHeader {
  char name[6];
  uint8_t version;
  uint16_t pageSize;            // 512, 1024, ..., 32768
  uint16_t pagesAmount;
  uint16_t filesAmount;
  uint32_t filesStartAddress;
  uint32_t pagesMapStartAddress;
  uint32_t pagesStartAddress;

  LogFSHeader();
};

#endif

#ifndef LOGFS_HEADER_H
#define LOGFS_HEADER_H

#include <stdint.h>

/* HEADER SIZE 32 bytes
  MAX CAPACITY: maxPages(32768) * maxPageSize(32768) = 1073741824 = 1Gb
*/

struct LogFSHeader {
  char name[6];
  uint8_t version;
  uint16_t sectorSize;
  uint16_t pageSize;
  uint16_t pagesAmount;
  uint32_t filesStartAddress;     // 2nd sector
  uint32_t pagesMapStartAddress;  // 3rd sector
  uint32_t sectorsStartAddress;

  LogFSHeader();
};

#endif

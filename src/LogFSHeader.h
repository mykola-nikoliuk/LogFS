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
  uint32_t sectorsAmount;
  uint32_t sectorsMapStartAddress;
  uint32_t sectorsStartAddress;

  LogFSHeader();
};

#endif

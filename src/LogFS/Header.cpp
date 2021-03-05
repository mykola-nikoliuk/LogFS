#include "./Header.h"

LogFSHeader::LogFSHeader() {
  version = 0;
  pageSize = 0;
  sectorSize = 0;
  sectorsAmount = 0;
  sectorsMapStartAddress = 0;
  sectorsStartAddress = 0;
}

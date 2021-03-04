#include "./Header.h"

LogFSHeader::LogFSHeader() {
  version = 0;
  pageSize = 0;
  sectorSize = 0;
  pagesAmount = 0;
  filesStartAddress = 0;
  pagesMapStartAddress = 0;
  sectorsStartAddress = 0;
}

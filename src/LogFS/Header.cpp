#include "./Header.h"

LogFSHeader::LogFSHeader() {
  version = 0;
  pageSize = 0;
  pagesAmount = 0;
  filesAmount = 0;
  filesStartAddress = 0;
  pagesMapStartAddress = 0;
  pagesStartAddress = 0;
}

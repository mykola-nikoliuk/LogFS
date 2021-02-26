#include "./Header.h"

LogFSHeader::LogFSHeader() {
  pageSize = 0;
  pagesAmount = 0;
  filesAmount = 0;
}

char LogFSHeader::name[] = "LOGFS";
uint8_t LogFSHeader::version = 1;

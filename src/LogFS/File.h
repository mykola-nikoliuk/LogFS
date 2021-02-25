#ifndef LOGFS_FILE_H
#define LOGFS_FILE_H

struct LogFSFile {
  char name[12];
  uint16 firstPageIndex;
  uint16 lastPageOffset;
};

#endif

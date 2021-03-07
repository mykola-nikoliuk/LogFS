#ifndef LOGFS_FILEHEADER_H
#define LOGFS_FILEHEADER_H

#include "config.h"

struct LogFSFileHeader {
  char name[LogGS_FILE_NAME_LENGTH + 1];
};

#endif
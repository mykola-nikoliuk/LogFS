#ifndef LOGFS_H
#define LOGFS_H

#include "FSIO.h"
#include "Header.h"
#include "TableFile.h"
#include "File.h"
#include <stdint.h>

class LogFS {

  private:
    LogFSHeader _header;
    FSIO* _fsio;
    uint32_t writeEmptyFileTable(uint32_t address, uint16_t filesAmount);
    void clearPages(uint32_t address, uint16_t pagesAmount);
    int32_t allocatePage();
    uint32_t fillTableFile(char* name, LogFSTableFile* tableFile);
    uint32_t getPageAddress(uint16_t pageIndex);
    friend class LogFSFile;

  public:
    LogFS(FSIO* fsio);

    uint8_t init();
    uint8_t format(uint32_t capacity);
    uint8_t format(uint32_t capacity, uint16_t pageSize);
    uint8_t format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount);

    LogFSFile createFile(char* name);
    LogFSFile openFile(char* name);
    uint8_t deleteFile(char* name);
    bool exist(char* name);

    LogFSHeader* getHeader() { return &_header; }
};

#endif

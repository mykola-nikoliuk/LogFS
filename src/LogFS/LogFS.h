#ifndef LOGFS_H
#define LOGFS_H

#include "FlashIO.h"
#include "Header.h"
#include "FileHeader.h"
#include "SectorFlags.h"
#include "Directory.h"
#include "File.h"
#include <stdint.h>

class LogFS {

  private:
    LogFSHeader _header;
    FlashIO* _fio;
    uint16_t _pagesUsed;

//    uint32_t writeEmptyFileTable(uint32_t address, uint16_t filesAmount);
//    void clearPages(uint32_t address, uint16_t sectorsAmount);
    uint32_t allocateSector(uint8_t flags);
//    void releasePage(uint16_t pageIndex);
    uint32_t readFileHeader(char* name, LogFSFileHeader* fileHeader);
//    uint32_t getPageAddress(uint16_t pageIndex);
    uint16_t getPagesUsed();
//    friend class LogFSFile;
//    friend class LogFSDirectory;

  public:
    LogFS(FlashIO* fio);

    uint8_t init();
    uint8_t format();

    LogFSFile createFile(char* name);
    LogFSFile openFile(char* name);
//    uint8_t deleteFile(char* name);
//    bool exist(char* name);
//
//    uint32_t getTotalSize();
//    uint32_t getAvailableSize();
//    uint32_t getUsedSize();
//
//    LogFSDirectory readFiles();

    LogFSHeader* getHeader() { return &_header; }
};

#endif

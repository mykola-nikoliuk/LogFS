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
    uint32_t _sectorsUsed;

    uint32_t getPageIndex(uint32_t offset);
    uint32_t allocateSector(uint8_t flags);
    void releaseSector(uint32_t sectorIndex);
    void cleanSelectorsMap();
    uint32_t readFileHeader(char* name, LogFSFileHeader* fileHeader);
    uint16_t getSectorAddressSize();
    uint32_t countSectorsUsed();
    friend class LogFSFile;
    friend class LogFSDirectory;

  public:
    LogFS(FlashIO* fio, bool runInit = false);

    uint8_t init();
    uint8_t format();

    LogFSFile createFile(char* name);
    LogFSFile openFile(char* name);
    uint8_t deleteFile(char* name);
    bool exist(char* name);

    uint32_t getTotalSize();
    uint32_t getAvailableSize();
    uint32_t getUsedSize();

    LogFSDirectory readFiles();

    LogFSHeader* getHeader() { return &_header; }
};

#endif

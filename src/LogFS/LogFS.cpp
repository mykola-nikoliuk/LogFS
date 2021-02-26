#include <string.h>
#include <math.h>
#include "./LogFS.h"
#include "./TableFile.h"
#include "./Header.h"
#include "./config.h"

LogFS::LogFS(FSIO* fsio) {
  _fsio = fsio;
}

uint32_t LogFS::writeEmptyFileTable(uint32_t address, uint16_t filesAmount) {
  uint16_t fileSize = sizeof(struct LogFSTableFile);

  LogFSTableFile file;
  file.isEmpty = true;

  for (uint16_t i = 0; i < filesAmount; i++) {
    _fsio->writeBytes(address + i * fileSize, (uint8_t*)&file, fileSize);
  }

  return address + fileSize * filesAmount;
}

void LogFS::clearPages(uint32_t address, uint16_t pagesAmount) {
  uint32_t zero = 0;
  for (uint16_t i = 0; i < pagesAmount; i++) {
    _fsio->writeInt(
      address + i * _header.pageSize + _header.pageSize - sizeof(zero),
      zero
    );
  }
}


uint8_t LogFS::init() {
  _fsio->readBytes(0, (uint8_t*)&_header, sizeof(struct LogFSHeader));

  if (strcmp(_header.name, LogFS_NAME)) {
    return LOGFS_ERR_NOT_FORMATTED;
  }

  if (_header.version != LogFS_VERSION) {
    return LOGFS_ERR_DIFFERENT_VERSION;
  }

  return LOGFS_OK;
}

uint8_t LogFS::format(uint32_t capacity) {
  return LogFS::format(capacity, DEFAULT_PAGE_SIZE, DEFAULT_MAX_FILES_AMOUNT);
}

uint8_t LogFS::format(uint32_t capacity, uint16_t pageSize) {
  return LogFS::format(capacity, pageSize, DEFAULT_MAX_FILES_AMOUNT);
}

uint8_t LogFS::format(uint32_t capacity, uint16_t pageSize, uint16_t maxFilesAmount) {
  LogFSHeader header;
  strcpy(header.name, LogFS_NAME);
  header.version = LogFS_VERSION;
  header.pageSize = pageSize;
  header.filesAmount = maxFilesAmount;
  header.filesStartAddress = sizeof(struct LogFSHeader);

  header.pagesMapStartAddress = header.filesStartAddress + sizeof(struct LogFSTableFile) * uint32_t(maxFilesAmount);
  if (header.pagesMapStartAddress > capacity) return LOGFS_ERR_LOW_SPACE_FILE_TABLE;

  int64_t memoryForPages = capacity - header.pagesMapStartAddress;

  // 9th bit to count pagesMap size;
  int16_t pagesAmount = (uint64_t(memoryForPages) * 8) / (uint64_t(pageSize) * 9);
  if (pagesAmount == 0) return LOGFS_ERR_LOW_SPACE_PAGES;

  header.pagesAmount = pagesAmount;
  header.pagesStartAddress = ceil(pagesAmount / 8.f) + header.pagesMapStartAddress;

  // start format memory
  uint32_t address = 0;
  address = _fsio->writeBytes(address, (uint8_t*)&header, sizeof(struct LogFSHeader));
  address = writeEmptyFileTable(address, maxFilesAmount);
  uint8_t zero = 0;
  for (uint16_t i = 0; i < pagesAmount; i++) {
    _fsio->writeByte(address + i, zero);
  }
  address += pagesAmount;


  return LOGFS_OK;
}

LogFSFile* LogFS::createFile(char* name) {
  if (strlen(name) > LogGS_FILE_NAME_LENGTH - 1) return NULL;

  LogFSTableFile tableFile;

  // look for free table file
  uint32_t tableFileAddress = 0;
  uint32_t tableFileSize = sizeof(struct LogFSTableFile);
  for (uint16_t i = 0; i < _header.filesAmount; i++) {
    uint32_t address = _header.filesStartAddress + i * tableFileSize;
    if (_fsio->readByte(address)) {
      _fsio->readBytes(address, (uint8_t*)&tableFile, tableFileSize);

      tableFile.isEmpty = false;
      strcpy(tableFile.name, name);

      tableFileAddress = address;
      break;
    }
  }
  if (!tableFileAddress) return NULL;

  // look for free page


  _fsio->writeBytes(tableFileAddress, (uint8_t*)&tableFile, tableFileSize);

  LogFSFile *file = new LogFSFile();
  // fill the file structure

  return file;
}

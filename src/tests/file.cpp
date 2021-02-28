#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./file.h"

using namespace std;

char defaultName[] = "file.txt";
char longName[] = "12345678901234567890.txt";
char defaultLogs[] = "some logs";

class LogFSRAMTest : public LogFSRAM {
  public:
    LogFSRAMTest(uint16_t filesAmount) : LogFSRAM() {
      formatAndInit(filesAmount);
    }
    LogFSRAMTest() : LogFSRAM() {
      formatAndInit(64);
    }

    void formatAndInit(uint8_t filesAmount) {
      format(MEMORY_SIZE, 512, filesAmount);
      init();
    }
};

// --- TESTS ---

bool createLongNameFile() {
  LogFSRAMTest fs;

  return fs.createFile(longName).getStatus() == LOGFS_ERR_LONG_FILE_NAME;
}

bool reachMaxFilesLimit() {
  LogFSRAMTest fs(2);

  fs.createFile(defaultName);
  fs.createFile(defaultName);

  return fs.createFile(defaultName).getStatus() == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
}

bool openFileNotExist() {
  LogFSRAMTest fs;

  return fs.openFile(defaultName).getStatus() == LOGFS_ERR_FILE_NOT_FOUND;
}

bool createAndOpenFile() {
  LogFSRAMTest fs;

  fs.createFile(defaultName);
  return fs.openFile(defaultName).getStatus() == LOGFS_OK;
}

bool createAndDeleteFile() {
  LogFSRAMTest fs;

  fs.createFile(defaultName);
  return fs.deleteFile(defaultName) == LOGFS_OK;
}

bool deleteFileNotExist() {
  LogFSRAMTest fs;

  return fs.deleteFile(defaultName) == LOGFS_ERR_FILE_NOT_FOUND;
}

bool deleteLongNameFile() {
  LogFSRAMTest fs;

  return fs.deleteFile(longName) == LOGFS_ERR_LONG_FILE_NAME;
}

bool openDeletedFile() {
  LogFSRAMTest fs;

  fs.createFile(defaultName);
  fs.deleteFile(defaultName);

  return fs.openFile(defaultName).getStatus() == LOGFS_ERR_FILE_NOT_FOUND;
}

bool fileNotExist() {
  LogFSRAMTest fs;

  return !fs.exist(defaultName);
}

bool fileExist() {
  LogFSRAMTest fs;

  fs.createFile(defaultName);
  return fs.exist(defaultName);
}

bool writeFile() {
  LogFSRAMTest fs;

  LogFSFile file = fs.createFile(defaultName);
  file.writeBytes((uint8_t*)defaultLogs, strlen(defaultLogs));

  char* pagesStart = (char*)&fs.fsio.data + fs.getHeader()->pagesStartAddress;

  return strcmp(pagesStart, defaultLogs) == 0;
}

bool fileNotOpened() {
  LogFSRAMTest fs;

  LogFSFile file = fs.createFile(longName);

  return file.writeBytes((uint8_t*)defaultLogs, strlen(defaultLogs)) == LOGFS_ERR_FILE_NOT_OPENED;
}

void testFile() {
  cout << "File:" << endl;

  test("create long name file", createLongNameFile());
  test("reach max files limit", reachMaxFilesLimit());
  test("open file not exist", openFileNotExist());
  test("create and open", createAndOpenFile());
  test("create and delete file", createAndDeleteFile());
  test("delete file not exist", deleteFileNotExist());
  test("delete long name file", deleteLongNameFile());
  test("open deleted file", openDeletedFile());
  test("file not exist", fileNotExist());
  test("file exist", fileExist());
  test("write file", writeFile());
  test("file not opened", fileNotOpened());
}

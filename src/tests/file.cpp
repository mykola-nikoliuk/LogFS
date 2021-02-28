#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./file.h"

using namespace std;

char defaultName[] = "file.txt";
char defaultLogs[] = "some logs";

void initFS(LogFSRAM* fs, uint16_t filesAmount) {
  fs->format(MEMORY_SIZE, 512, filesAmount);
  fs->init();
}

void initFS(LogFSRAM* fs) {
  return initFS(fs, 64);
}

// --- TESTS ---

bool createLongNameFile() {
  LogFSRAM fs;
  initFS(&fs);
  char name[] = "12345678901234567890.txt";

  LogFSFile file;
  return fs.createFile(name, &file) == LOGFS_ERR_LONG_FILE_NAME;
}

bool reachMaxFilesLimit() {
  LogFSRAM fs;
  initFS(&fs, 2);
  LogFSFile file;

  fs.createFile(defaultName, &file);
  fs.createFile(defaultName, &file);

  return fs.createFile(defaultName, &file) == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
}

bool openFileNotExist() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  return fs.openFile(defaultName, &file) == LOGFS_ERR_FILE_NOT_FOUND;
}

bool createAndOpenFile() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  fs.createFile(defaultName, &file);

  return fs.openFile(defaultName, &file) == LOGFS_OK;
}

bool createAndDeleteFile() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  fs.createFile(defaultName, &file);
  return fs.deleteFile(defaultName) == LOGFS_OK;
}

bool deleteFileNotExist() {
  LogFSRAM fs;
  initFS(&fs);

  return fs.deleteFile(defaultName) == LOGFS_ERR_FILE_NOT_FOUND;
}

bool deleteLongNameFile() {
  LogFSRAM fs;
  initFS(&fs);
  char name[] = "12345678901234567890.txt";

  return fs.deleteFile(name) == LOGFS_ERR_LONG_FILE_NAME;
}

bool openDeletedFile() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  fs.createFile(defaultName, &file);
  fs.deleteFile(defaultName);

  return fs.openFile(defaultName, &file) == LOGFS_ERR_FILE_NOT_FOUND;
}

bool fileNotExist() {
  LogFSRAM fs;
  initFS(&fs);

  return !fs.exist(defaultName);
}

bool fileExist() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  fs.createFile(defaultName, &file);
  return fs.exist(defaultName);
}

bool writeFile() {
  LogFSRAM fs;
  initFS(&fs);
  LogFSFile file;

  fs.createFile(defaultName, &file);
  file.writeBytes((uint8_t*)defaultLogs, strlen(defaultLogs));

  char* pagesStart = (char*)&fs.fsio.data + fs.getHeader()->pagesStartAddress;

  cout << defaultLogs << endl;

//  return strcmp(pagesStart, defaultLogs) == 0;
  // finish test
  return true;
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
}

#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./file.h"

using namespace std;

char defaultName[] = "file.txt";

LogFS* createFS(uint16_t filesAmount) {
  MiniFSIO fsio;
  LogFS *fs = new LogFS(&fsio);

  fs->format(MEMORY_SIZE, 512, filesAmount);
  fs->init();
  return fs;
}

LogFS* createFS() {
  return createFS(64);
}


bool createLongNameFile() {
  LogFS *fs = createFS();
  char name[] = "12345678901234567890.txt";

  LogFSFile file;
  bool result = fs->createFile(name, &file) == LOGFS_ERR_LONG_FILE_NAME;

  delete fs;
  return result;
}

bool reachMaxFilesLimit() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  fs->createFile(defaultName, &file);
  fs->createFile(defaultName, &file);

  bool result = fs->createFile(defaultName, &file) == LOGFS_ERR_LOW_SPACE_FILE_TABLE;

  delete fs;
  return result;
}

bool openFileNotExist() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  bool result = fs->openFile(defaultName, &file) == LOGFS_ERR_FILE_NOT_FOUND;

  delete fs;
  return result;
}

bool createAndOpenFile() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  fs->createFile(defaultName, &file);

  bool result = fs->openFile(defaultName, &file) == LOGFS_OK;

  delete fs;
  return result;
}

bool createAndDeleteFile() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  fs->createFile(defaultName, &file);
  bool result = fs->deleteFile(defaultName) == LOGFS_OK;

  delete fs;
  return result;
}

bool deleteFileNotExist() {
  LogFS *fs = createFS(2);

  bool result = fs->deleteFile(defaultName) == LOGFS_ERR_FILE_NOT_FOUND;

  delete fs;
  return result;
}

bool deleteLongNameFile() {
  LogFS *fs = createFS(2);
  char name[] = "12345678901234567890.txt";

  bool result = fs->deleteFile(name) == LOGFS_ERR_LONG_FILE_NAME;

  delete fs;
  return result;
}

bool openDeletedFile() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  fs->createFile(defaultName, &file);
  fs->deleteFile(defaultName);

  bool result = fs->openFile(defaultName, &file) == LOGFS_OK;

  delete fs;
  return result;
}

bool fileNotExist() {
  LogFS *fs = createFS(2);

  bool result = !fs->exist(defaultName);

  delete fs;
  return result;
}

bool fileExist() {
  LogFS *fs = createFS(2);
  LogFSFile file;

  fs->createFile(defaultName, &file);
  bool result = fs->exist(defaultName);

  delete fs;
  return result;
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
}

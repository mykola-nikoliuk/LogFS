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

  bool result = fs->createFile(name) == NULL;

  delete fs;
  return result;
}

bool reachMaxFilesLimit() {
  LogFS *fs = createFS(2);
  delete fs->createFile(defaultName);
  delete fs->createFile(defaultName);

  bool result = fs->createFile(defaultName) == NULL;

  delete fs;
  return result;

}

bool openFileNotExist() {
  LogFS *fs = createFS(2);

  bool result = fs->openFile(defaultName) == NULL;

  delete fs;
  return result;
}

bool createAndOpenFile() {
  LogFS *fs = createFS(2);

  delete fs->createFile(defaultName);
  LogFSFile* file = fs->openFile(defaultName);

  bool result = file;

  delete file;
  delete fs;
  return result;
}

bool createAndDeleteFile() {
  LogFS *fs = createFS(2);

  delete fs->createFile(defaultName);
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

  delete fs->createFile(defaultName);
  fs->deleteFile(defaultName);

  bool result = fs->openFile(defaultName) == NULL;

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
}

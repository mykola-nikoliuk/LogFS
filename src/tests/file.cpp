#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./file.h"

using namespace std;

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
  char name[] = "1.txt";
  delete fs->createFile(name);
  delete fs->createFile(name);

  bool result = fs->createFile(name) == NULL;

  delete fs;
  return result;

}

bool openFileNotExist() {
  LogFS *fs = createFS(2);
  char name[] = "notexist.txt";

  bool result = fs->openFile(name) == NULL;

  delete fs;
  return result;
}

bool createAndOpenFile() {
  LogFS *fs = createFS(2);
  char name[] = "file.txt";

  delete fs->createFile(name);
  bool result = fs->openFile(name);

  delete fs;
  return result;
}

void testFile() {
  cout << "File:" << endl;

  test("create long name file", createLongNameFile());
  test("reach max files limit", reachMaxFilesLimit());
  test("open file not exist", openFileNotExist());
  test("create and open", createAndOpenFile());
}

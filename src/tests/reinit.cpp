#include <iostream>
#include "test.h"
#include "reinit.h"

char filename[] = "filename.txt";
char filename2[] = "anotherFile.txt";
char body[] = "file content can be any but we are using string for this tests";

namespace ReInitNS {

  LogFS getSecondFS(LogFSRAM* fs) {
    return LogFS(&fs->fio, true);
  }

  bool createFile() {
    LogFSRAMTest fs;
    fs.createFile(filename);
    return getSecondFS(&fs).exist(filename);
  }

  bool createTwoFiles() {
    LogFSRAMTest fs;
    fs.createFile(filename);
    fs.createFile(filename2);

    LogFS fs2 = getSecondFS(&fs);

    return fs2.exist(filename) && fs.exist(filename2);
  }

  bool deleteFile() {
    LogFSRAMTest fs;
    fs.createFile(filename);
    fs.deleteFile(filename);
    return !getSecondFS(&fs).exist(filename);
  }

  bool openFile() {
    LogFSRAMTest fs;
    fs.createFile(filename);
    return getSecondFS(&fs).openFile(filename).getStatus() == LOGFS_OK;
  }

  bool readFile() {
    LogFSRAMTest fs;
    fs.createFile(filename).write(body, strlen(body) + 1);
    LogFSFile file = getSecondFS(&fs).openFile(filename);

    char buffer[64];
    file.read(buffer, file.size());

    return strcmp(body, buffer) == 0;
  }

  bool fileSize() {
    LogFSRAMTest fs;
    LogFSFile file = fs.createFile(filename);
    file.write(body, strlen(body) + 1);
    uint16_t fileSize = file.size();

    LogFS fs2 = getSecondFS(&fs);

    LogFSFile file2 = fs2.openFile(filename);
    return file2.size() == fileSize;
  }

  bool usedSize() {
    LogFSRAMTest fs;

    fs.createFile(filename).write(body, strlen(body) + 1);
    uint32_t usedSize = fs.getUsedSize();

    return getSecondFS(&fs).getUsedSize() == usedSize;
  }

  bool writeAfter() {
    LogFSRAMTest fs;

    uint8_t firstPartSize = 4;
    uint8_t secondPartSize = 6;
    uint8_t totalSize = firstPartSize + secondPartSize + 1;

    char trimmedBody[totalSize];
    memcpy(trimmedBody, body, firstPartSize + secondPartSize);
    trimmedBody[totalSize - 1] = '\0';

    fs.createFile(filename).write(trimmedBody, firstPartSize);

    LogFS fs2 = getSecondFS(&fs);
    LogFSFile file2 = fs2.openFile(filename);
    file2.write(trimmedBody + firstPartSize, secondPartSize + 1);

    char buffer[totalSize];
    LogFS fs3 = getSecondFS(&fs);
    LogFSFile file3 = fs3.openFile(filename);
    file3.read(buffer, file3.size());

    return file3.size() == totalSize && strcmp(buffer, trimmedBody) == 0;

  }

  bool writeManyTimes() {
    LogFSRAMTest fs;

    char log[SECTOR_SIZE];

    LogFSFile file = fs.createFile(filename);
    for (uint16_t i = 0; i < SECTOR_SIZE; i++) {
      log[i] = (i % 10) + 48;
      if (i == SECTOR_SIZE - 1) {
        log[i] = '\0';
      }
      file.write(log + i, 1);
    }

    LogFS fs2 = getSecondFS(&fs);

    char result[SECTOR_SIZE];
    LogFSFile file2 = fs2.openFile(filename);
    file2.read(result, file.size());

    return strcmp(result, log) == 0;
  }

  void runTests() {
    cout << "Re Init:" << endl;
    test("create file", createFile());
    test("create two files", createTwoFiles());
    test("delete files", deleteFile());
    test("open file", openFile());
    test("read file", readFile());
    test("file size", fileSize());
    test("used size", usedSize());
    test("write after", writeAfter());
    test("write many times", writeManyTimes());
  }

}
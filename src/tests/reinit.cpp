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

  bool fileSize() {
    LogFSRAMTest fs;
    LogFSFile file = fs.createFile(filename);
    file.write(body, strlen(body) + 1);
    uint16_t fileSize = file.size();

    LogFS fs2 = getSecondFS(&fs);

    LogFSFile file2 = fs2.openFile(filename);
    return file2.size() == fileSize;
  }

  void runTests() {
    cout << "Re Init:" << endl;
    test("create file", createFile());
    test("create two files", createTwoFiles());
    test("delete files", deleteFile());
    test("open file", openFile());
    test("file size", fileSize());
  }

}
#include <stdint.h>
#include <iostream>
#include <string.h>
#include "../LogFS/LogFS.h"
#include "./test.h"
#include "./file.h"

using namespace std;

char defaultName[] = "file.txt";
char secondFileName[] = "file2.txt";
char longName[] = "1234567890123456789001234567890.txt";
char defaultLogs[] = "some logs";
char secondLogs[] = "another logs line";

// --- TESTS ---

bool createFile() {
  LogFSRAMTest fs;

  return fs.createFile(defaultName).getStatus() == LOGFS_OK;
}

bool createLongNameFile() {
  LogFSRAMTest fs;

  return fs.createFile(longName).getStatus() == LOGFS_ERR_LONG_FILE_NAME;
}

//bool reachMaxFilesLimit() {
//  LogFSRAMTest fs(2);
//
//  fs.createFile(defaultName);
//  fs.createFile(defaultName);
//
//  return fs.createFile(defaultName).getStatus() == LOGFS_ERR_LOW_SPACE_FILE_TABLE;
//}
//
//bool openFileNotExist() {
//  LogFSRAMTest fs;
//
//  return fs.openFile(defaultName).getStatus() == LOGFS_ERR_FILE_NOT_FOUND;
//}
//
//bool createAndOpenFile() {
//  LogFSRAMTest fs;
//
//  fs.createFile(defaultName);
//  return fs.openFile(defaultName).getStatus() == LOGFS_OK;
//}
//
//bool createAndDeleteFile() {
//  LogFSRAMTest fs;
//
//  fs.createFile(defaultName);
//  return fs.deleteFile(defaultName) == LOGFS_OK;
//}
//
//bool deleteFileNotExist() {
//  LogFSRAMTest fs;
//
//  return fs.deleteFile(defaultName) == LOGFS_ERR_FILE_NOT_FOUND;
//}
//
//bool deleteLongNameFile() {
//  LogFSRAMTest fs;
//
//  return fs.deleteFile(longName) == LOGFS_ERR_LONG_FILE_NAME;
//}
//
//bool openDeletedFile() {
//  LogFSRAMTest fs;
//
//  fs.createFile(defaultName);
//  fs.deleteFile(defaultName);
//
//  return fs.openFile(defaultName).getStatus() == LOGFS_ERR_FILE_NOT_FOUND;
//}
//
//bool fileNotExist() {
//  LogFSRAMTest fs;
//
//  return !fs.exist(defaultName);
//}
//
//bool fileExist() {
//  LogFSRAMTest fs;
//
//  fs.createFile(defaultName);
//  return fs.exist(defaultName);
//}
//
//bool writeFile() {
//  LogFSRAMTest fs;
//
//  LogFSFile file = fs.createFile(defaultName);
//  file.write((uint8_t*)defaultLogs, strlen(defaultLogs));
//
//  char* pagesStart = (char*)&fs.fio.data + fs.getHeader()->sectorsStartAddress;
//
//  return strcmp(pagesStart, defaultLogs) == 0;
//}
//
//bool fileNotOpened() {
//  LogFSRAMTest fs;
//
//  LogFSFile file = fs.createFile(longName);
//
//  return file.write((uint8_t*)defaultLogs, strlen(defaultLogs)) == LOGFS_ERR_FILE_NOT_OPENED;
//}
//
//bool readFile() {
//  LogFSRAMTest fs;
//
//  uint8_t logsLength = strlen(defaultLogs) + 1;
//  LogFSFile file = fs.createFile(defaultName);
//  file.write((uint8_t*)defaultLogs, logsLength);
//
//  char buffer[logsLength];
//  file.read((uint8_t*)buffer, logsLength);
//
//  return strcmp(buffer, defaultLogs) == 0;
//}
//
//bool readTwoFile() {
//  LogFSRAMTest fs;
//
//  uint8_t logsLength1 = strlen(defaultLogs) + 1;
//  uint8_t logsLength2 = strlen(secondLogs) + 1;
//  LogFSFile file1 = fs.createFile(defaultName);
//  LogFSFile file2 = fs.createFile(secondFileName);
//  file1.write((uint8_t*)defaultLogs, logsLength1);
//  file2.write((uint8_t*)secondLogs, logsLength2);
//
//  char buffer1[logsLength1];
//  char buffer2[logsLength2];
//  file1.read((uint8_t*)buffer1, logsLength1);
//  file2.read((uint8_t*)buffer2, logsLength2);
//
//  return strcmp(buffer1, defaultLogs) == 0 && strcmp(buffer2, secondLogs) == 0;
//}
//
//bool writeAndReadMoreThanPageSize() {
//  LogFSRAMTest fs(1024);
//
//  char log[] = "log that longer then page size";
//  uint16_t logSize = strlen(log) + 1;
//
//  LogFSFile file = fs.createFile(defaultName);
//  file.write((uint8_t*)log, logSize);
//
//  char buffer[logSize];
//  file.read((uint8_t*)buffer, logSize);
//
//  return strcmp(buffer, log) == 0;
//}
//
//bool writeByChunks() {
//  LogFSRAMTest fs;
//
//  char logs[] = "12345678\n";
//  LogFSFile file = fs.createFile(defaultName);
//  uint8_t chunkSize = 3;
//  for (uint8_t i = 0; i < strlen(logs); i += chunkSize) {
//    file.write((uint8_t*)(logs + i), chunkSize);
//  }
//  char buffer[strlen(logs)];
//  file.read((uint8_t*)buffer, strlen(logs));
//
//  return strcmp(buffer, logs);
//}
//
//bool readByChunks() {
//  LogFSRAMTest fs;
//
//  char logs[] = "12345678\n";
//  LogFSFile file = fs.createFile(defaultName);
//  uint8_t chunkSize = 3;
//  file.write((uint8_t*)logs, strlen(logs));
//  char buffer[strlen(logs)];
//
//  for (uint8_t i = 0; i < strlen(logs); i += chunkSize) {
//    file.read((uint8_t*)(buffer + i), chunkSize);
//  }
//
//  return strcmp(buffer, logs);
//}
//
//bool getTotalSize() {
//  LogFSRAMTest fs;
//  return fs.getTotalSize() > 0;
//}
//
//bool getUsedSize() {
//  uint16_t pageSize = 256;
//  LogFSRAMTest fs;
//  fs.createFile(defaultName);
//
//  return fs.getUsedSize() == pageSize - sizeof(fs.getHeader()->sectorsAmount);
//}
//
//bool getAvailableSize() {
//  LogFSRAMTest fs;
//  fs.createFile(defaultName);
//
//  return fs.getAvailableSize() == fs.getTotalSize() - fs.getUsedSize();
//}
//
//bool releasePages() {
//  LogFSRAMTest fs;
//
//  uint32_t sizeBefore = fs.getAvailableSize();
//
//  fs.createFile(defaultName);
//
//  uint32_t sizeInTheMiddle = fs.getAvailableSize();
//
//
//  fs.deleteFile(defaultName);
//
//  uint32_t sizeAfter = fs.getAvailableSize();
//  return sizeBefore == sizeAfter && sizeInTheMiddle != sizeAfter;
//}
//
//bool readEmptyFiles() {
//  LogFSRAMTest fs;
//
//  char buffer[16];
//
//  return fs.readFiles().next(buffer) == LOGFS_ERR_FILE_NOT_FOUND;
//}
//
//bool readCreatedFiles() {
//  LogFSRAMTest fs;
//
//  fs.createFile(defaultName);
//  fs.createFile(secondFileName);
//
//  char buffer1[16];
//  char buffer2[16];
//
//  LogFSDirectory dir = fs.readFiles();
//  dir.next(buffer1);
//  dir.next(buffer2);
//  return strcmp(buffer1, defaultName) == 0 && strcmp(buffer2, secondFileName) == 0;
//}
//
//bool checkWriteSize() {
//
//  LogFSRAMTest fs;
//  LogFSFile file = fs.createFile(defaultName);
//  uint8_t logLength = strlen(defaultLogs) + 1;
//  file.write((uint8_t*)defaultLogs, logLength);
//  file.write((uint8_t*)defaultLogs, logLength);
//
//  return file.size() == logLength * 2;
//}

void testFile() {
  cout << "File:" << endl;

  test("create file", createFile());
  test("create long name file", createLongNameFile());
//  test("reach max files limit", reachMaxFilesLimit());
//  test("open file not exist", openFileNotExist());
//  test("create and open", createAndOpenFile());
//  test("create and delete file", createAndDeleteFile());
//  test("delete file not exist", deleteFileNotExist());
//  test("delete long name file", deleteLongNameFile());
//  test("open deleted file", openDeletedFile());
//  test("file not exist", fileNotExist());
//  test("file exist", fileExist());
//  test("write file", writeFile());
//  test("file not opened", fileNotOpened());
//  test("write and read file", readFile());
//  test("write and read two files", readTwoFile());
//  test("write and read more than page size", writeAndReadMoreThanPageSize());
//  test("write by chunks", writeByChunks());
//  test("read by chunks", readByChunks());
//  test("get total size", getTotalSize());
//  test("get used size", getUsedSize());
//  test("get available size", getAvailableSize());
//  test("release pages", releasePages());
//  test("read empty files list", readEmptyFiles());
//  test("read created files list", readCreatedFiles());
//  test("check write size", checkWriteSize());

  // page release after write with full storage
  // read files list
}

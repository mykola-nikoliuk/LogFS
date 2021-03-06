#include <iostream>
#include "test.h"

using namespace std;

LogFSRAMTest fs;

void createFile(LogFSRAMTest* fs) {
  char buffer[32];

  cout << "Enter filename: ";
  cin >> buffer;

  LogFSFile file = fs->createFile(buffer);
  if (file.getStatus() == LOGFS_OK) {
    cout << "File created successfully" << endl;
  } else {
    cout << "Error: " << int(file.getStatus()) << endl;
  }
}

void deleteFile(LogFSRAMTest* fs) {
  char buffer[32];

  cout << "Enter filename: ";
  cin >> buffer;

  LogFSFile file = fs->deleteFile(buffer);
  if (file.getStatus() == LOGFS_OK) {
    cout << "File successfully deleted" << endl;
  } else {
    cout << "Error: " << int(file.getStatus()) << endl;
  }
}

void writeFile(LogFSRAMTest* fs) {
  char buffer[32];

  cout << "Enter filename: ";
  cin >> buffer;

  LogFSFile file = fs->openFile(buffer);
  if (file.getStatus() == LOGFS_OK) {
    char content[1024];
    cout << "Enter content: ";
    cin >> content;
    uint8_t writeResult = file.write((uint8_t*)&content, strlen(content));
    if (writeResult == LOGFS_OK) {
      cout << "File wrote successfully" << endl;
    } else {
      cout << "Error: " << int(writeResult) << endl;
    }
  } else {
    cout << "Error: " << int(file.getStatus()) << endl;
  }
}

void readFile(LogFSRAMTest* fs) {
  char buffer[32];

  cout << "Enter filename: ";
  cin >> buffer;

  LogFSFile file = fs->openFile(buffer);
  if (file.getStatus() == LOGFS_OK) {
    char content[1024];
    uint8_t readResult = file.read((uint8_t*)&content, file.size());
    if (readResult == LOGFS_OK) {
      cout << content << endl;
    } else {
      cout << "Error: " << int(readResult) << endl;
    }
  } else {
    cout << "Error: " << int(file.getStatus()) << endl;
  }
}

void readFiles(LogFSRAMTest* fs) {
  char buffer[32];

  uint8_t counter = 0;
  LogFSDirectory dir = fs->readFiles();

  while (true) {
    if (dir.next(buffer) != LOGFS_OK) break;
    counter++;
    cout << "file: " << buffer << endl;
  }

  if (!counter) {
    cout << "There is no files" << endl;
  }
}

void readSizes(LogFSRAMTest* fs) {
  cout << "Used: " << fs->getUsedSize() << endl;
  cout << "Available: " << fs->getAvailableSize() << endl;
  cout << "Total: " << fs->getTotalSize() << endl;
}

void menu() {
  cout << endl << "--== Menu ==--" << endl;
  cout << "1. Create file" << endl;
  cout << "2. Delete file" << endl;
  cout << "3. Write file" << endl;
  cout << "4. Read file" << endl;
  cout << "9. Read sizes" << endl;
  cout << "0. Read files" << endl;
  cout << endl << "Enter an option: ";
}

void loop() {
  char command[16];

  menu();
  cin >> command;

  cout << endl;

  switch(command[0]) {
    case 49:
      createFile(&fs);
      break;
    case 50:
      deleteFile(&fs);
      break;
    case 51:
      writeFile(&fs);
      break;
    case 52:
      readFile(&fs);
      break;
    case 57:
      readSizes(&fs);
      break;
    case 48:
      readFiles(&fs);
      break;
  }
}

void testInteractive() {
  while (true) loop();
}

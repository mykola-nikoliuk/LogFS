#include <LogFS.h>
#include <string.h>

#define SECTOR_SIZE 4096
#define PAGE_SIZE 256
#define MAX_MEMORY_SIZE 4 * SECTOR_SIZE

char filename[] = "file.txt";
char text[] = "content of the file\0";

struct RAMIO : public LogFSStorageIO {
  RAMIO(uint32_t capacity, uint16_t sectorSize, uint16_t pageSize) :
    LogFSStorageIO(capacity, sectorSize, pageSize) {};

  uint8_t data[MAX_MEMORY_SIZE];

  uint8_t readByte(uint32_t address) {
    return data[address];
  }

  void resetChip() {
    for (uint32_t i = 0; i < getCapacity(); i++) {
      data[i] = 0xFF;
    }
  };
  void resetSector(uint32_t sectorIndex) {
    uint32_t address = sectorIndex * getSectorSize();
    uint16_t sectorSize = getSectorSize();
    for (uint32_t i = 0; i < sectorSize; i++) {
      data[address + i] = 0xFF;
    }
  };
  void writePage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    uint8_t *pData = (uint8_t *) pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      data[address + i] &= pData[i];
    }
  };
  void readPage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    uint8_t *pData = (uint8_t *) pageData;
    uint16_t pageSize = getPageSize();
    uint32_t address = sectorIndex * getSectorSize() + pageIndex * pageSize;
    for (uint32_t i = 0; i < pageSize; i++) {
      pData[i] = data[address + i];
    }
  };
};

RAMIO ramio(MAX_MEMORY_SIZE, SECTOR_SIZE, PAGE_SIZE);
LogFS fs(&ramio);

void setup() {
  Serial.begin(9600);

  fs.format();
  fs.init();

  LogFSFile writeFile = fs.createFile(filename);
  writeFile.write(text, strlen(text));

  char buffer[strlen(text)];
  LogFSFile readFile = fs.openFile(filename);
  readFile.read(buffer, readFile.size());

  Serial.print("Write: ");
  Serial.println(text);
  Serial.print("Read: ");
  Serial.println(buffer);
}

void loop() {

}
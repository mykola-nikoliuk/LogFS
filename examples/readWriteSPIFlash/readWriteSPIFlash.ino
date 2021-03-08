#include <SPI.h>
#include <LogFS.h>
#include <string.h>

#define SECTOR_SIZE 4096
#define PAGE_SIZE 256
#define MAX_MEMORY_SIZE 4 * SECTOR_SIZE

#define WB_WRITE_ENABLE       0x06
#define WB_WRITE_DISABLE      0x04
#define WB_CHIP_ERASE         0xc7
#define WB_READ_STATUS_REG_1  0x05
#define WB_READ_DATA          0x03
#define WB_PAGE_PROGRAM       0x02
#define WB_JEDEC_ID           0x9f
#define WB_SECTOR_ERASE           0x20

char filename[] = "file.txt";
char text[] = "content of the file\0";

void not_busy(void) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_STATUS_REG_1);
  while (SPI.transfer(0) & 1) {};
  digitalWrite(SS, HIGH);
}

struct FlashIO : public LogFSStorageIO {
  FlashIO(uint32_t capacity, uint16_t sectorSize, uint16_t pageSize) :
    LogFSStorageIO(capacity, sectorSize, pageSize) {};

  void resetChip() {
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_WRITE_ENABLE);
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_CHIP_ERASE);
    digitalWrite(SS, HIGH);
    not_busy();
  };
  void resetSector(uint32_t sectorIndex) {
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_WRITE_ENABLE);
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_SECTOR_ERASE);
    SPI.transfer((sectorIndex >> 8) & 0xFF);
    SPI.transfer((sectorIndex >> 0) & 0xFF);
    SPI.transfer(0);
    digitalWrite(SS, HIGH);
    not_busy();
  };
  void writePage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    uint16_t absolutePageIndex = sectorIndex * (SECTOR_SIZE / PAGE_SIZE) + pageIndex;
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_WRITE_ENABLE);
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_PAGE_PROGRAM);
    SPI.transfer((absolutePageIndex >>  8) & 0xFF);
    SPI.transfer((absolutePageIndex >>  0) & 0xFF);
    SPI.transfer(0);
    for (int i = 0; i < PAGE_SIZE; ++i) {
      SPI.transfer(((uint8_t*)pageData)[i]);
    }
    digitalWrite(SS, HIGH);
    not_busy();
  };
  void readPage(uint32_t sectorIndex, uint16_t pageIndex, void *pageData) {
    uint16_t absolutePageIndex = sectorIndex * (SECTOR_SIZE / PAGE_SIZE) + pageIndex;
    digitalWrite(SS, HIGH);
    digitalWrite(SS, LOW);
    SPI.transfer(WB_READ_DATA);
    SPI.transfer((absolutePageIndex >> 8) & 0xFF);
    SPI.transfer((absolutePageIndex >> 0) & 0xFF);
    SPI.transfer(0);
    for (int i = 0; i < PAGE_SIZE; ++i) {
      ((uint8_t*)pageData)[i] = SPI.transfer(0);
    }
    digitalWrite(SS, HIGH);
    not_busy();
  };
};

FlashIO flasio(MAX_MEMORY_SIZE, SECTOR_SIZE, PAGE_SIZE);
LogFS fs(&flasio);

void setup() {
  SPI.begin();
  SPI.setDataMode(0);
  SPI.setBitOrder(MSBFIRST);
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
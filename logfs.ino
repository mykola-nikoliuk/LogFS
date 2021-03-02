#include "src/LogFS/LogFS.h"

#define MEMORY_SIZE 1024

struct RAMFSIO : public FSIO {
  public:
    uint8_t data[MEMORY_SIZE];
    uint32_t writeByte(uint32_t address, uint8_t value) {
      data[address] = value;
      return address + 1;
    }
    uint8_t readByte(uint32_t address) {
      return data[address];
    }
};

class LogFSRAM : public LogFS {
  public:
      RAMFSIO fsio;
      LogFSRAM() : LogFS(&fsio) {};
};


void setup() {
  LogFSRAM fs;

  uint32_t formatStartTime = millis();
  fs.format(MEMORY_SIZE, 16, 16);
  uint32_t initStartTime = millis();
  fs.init();

  uint32_t createStartTime = millis();
  LogFSFile file = fs.createFile("1.txt");
  uint32_t writeStartTime = millis();
  file.write((uint8_t*)"1234567890", 11);
  file.close();
  uint32_t readStartTime = millis();
  char buffer[16];
  file.read((uint8_t*)buffer, 11);
  uint32_t endTime = millis();

  Serial.print("format: ");
  Serial.println(writeStartTime - createStartTime);
  Serial.print("write: ");
  Serial.println(readStartTime - writeStartTime);
  Serial.print("read: ");
  Serial.println(endTime - readStartTime);
}

void loop () {
}

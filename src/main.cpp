#include <stdint.h>
#include <iostream>
#include "./LogFS/LogFS.h"

uint8_t data[100];

class MiniSPIFFS : public LogFS {
  uint32_t writeByte(uint32_t address, uint8_t value) {
    data[address] = value;
    return address + 1;
  }
  uint8_t readByte(uint32_t address) {
    return data[address];
  }
};

MiniSPIFFS fs;

int main() {
  std::cout << "Start!\n";


  fs.format(16 * 1024 * 1024);
  fs.init();

  for (int i = 0; i < 100; i++) {
    std::cout << int(data[i]) << ',';
  }
  std::cout << "\n";
}

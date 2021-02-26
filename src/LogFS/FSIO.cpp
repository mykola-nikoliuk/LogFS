#include "FSIO.h"

uint32_t FSIO::writeInt(uint32_t address, uint32_t value) {
  writeByte(address, value >> 24);
  writeByte(address + 1, (value >> 16) & 0x00ff);
  writeByte(address + 2, (value >> 8) & 0x0000ff);
  writeByte(address + 3, value & 0x000000ff);
  return address + 4;
}

uint32_t FSIO::writeShort(uint32_t address, uint16_t value) {
  writeByte(address, value >> 8);
  writeByte(address + 1, value & 0x00ff);
  return address + 2;
}

uint32_t FSIO::writeBytes(uint32_t address, uint8_t* array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    writeByte(address + i, array[i]);
  }
  return address + length;
}

uint32_t FSIO::readInt(uint32_t address) {
  return
    (readByte(address) << 24) +
    (readByte(address + 1) << 16) +
    (readByte(address + 1) << 8) +
    readByte(address + 1);
}

uint16_t FSIO::readShort(uint32_t address) {
  uint8_t h = readByte(address);
  uint16_t l = readByte(address + 1);
  return (h << 8) | l;
}

void FSIO::readBytes(uint32_t address, uint8_t* array, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    array[i] = readByte(address + i);
  }
}

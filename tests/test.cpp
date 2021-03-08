#include <iostream>
#include "./test.h"

using namespace std;

uint16_t totalTests = 0;
uint16_t totalFailed = 0;

uint32_t RAMIO::resetChipCounter = 0;
uint32_t RAMIO::resetSectorCounter = 0;
uint32_t RAMIO::writePageCounter = 0;
uint32_t RAMIO::readPageCounter = 0;

void testStatus() {
  cout << endl;
  if (!totalFailed) {
    cout << "\033[32mAll test successful!\033[0m" << endl << endl;
  } else {
    cout << "\033[31mFailed: " << totalFailed << "\033[0m" << endl;
    cout << "\033[32mPassed: " << totalTests - totalFailed << "\033[0m" << endl << endl;
  }
}

void test(const char *name, bool result) {
  totalTests++;
  if (!result) totalFailed++;
  cout << (result ? "\033[32mOK    " : "\033[31mFAILED") << "\033[0m - " << name << endl;
}
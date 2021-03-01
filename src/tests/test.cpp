#include <iostream>
#include "./test.h"

using namespace std;

void test(const char* name, bool result) {
  cout << (result ? "\033[32mOK    " : "\033[31mFAILED") << "\033[0m - " << name << endl;
}

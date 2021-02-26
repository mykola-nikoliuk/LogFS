#include <iostream>
#include "./test.h"

using namespace std;

void test(const char* name, bool result) {
  cout << (result ? "OK    " : "FAILED") << " - " << name << endl;
}

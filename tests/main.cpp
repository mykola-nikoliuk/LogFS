#include "./format.h"
#include "./file.h"
#include "./interactive.h"
#include "./reinit.h"
#include "./test.h"

int main() {
  testFormat();
  testFile();
  ReInitNS::runTests();
  testStatus();
  testInteractive();
}


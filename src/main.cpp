#include "./tests/format.h"
#include "./tests/file.h"
//#include "./tests/interactive.h"
#include "./tests/reinit.h"

int main() {
  testFormat();
  testFile();
  ReInitNS::runTests();
//  testInteractive();
}


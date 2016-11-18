#include "se/stream_executor/lib/demangle.h"
#include <iostream>

using namespace std;

using perftools::gputools::port::Demangle;

int main() {
  std::cout << Demangle("St13bad_exception") << std::endl;
  std::cout << "Hello World!" << std::endl;
  return 0;
}

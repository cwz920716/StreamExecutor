#include "se/stream_executor/lib/demangle.h"
#include "se/stream_executor/blas.h"
#include <iostream>

using namespace std;

using perftools::gputools::port::Demangle;
using namespace perftools::gputools::blas;

int main() {
  std::cout << Demangle("St13bad_exception") << std::endl;
  std::cout << TransposeString(Transpose::kTranspose) << std::endl;
  std::cout << "Hello World!" << std::endl;
  return 0;
}

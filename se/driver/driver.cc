#include "se/stream_executor/lib/demangle.h"
#include "se/stream_executor/blas.h"
#include "se/stream_executor/stream_executor.h"
#include <iostream>

using namespace std;
using namespace perftools::gputools;

using perftools::gputools::port::Demangle;
using namespace perftools::gputools::blas;

// A PTX string defining a CUDA kernel.
//
// This PTX string represents a kernel that takes two arguments: an input value
// and an output pointer. The input value is a floating point number. The output
// value is a pointer to a floating point value in device memory. The output
// pointer is where the output from the kernel will be written.
//
// The kernel adds a fixed floating point value to the input and writes the
// result to the output location.
static constexpr const char *KERNEL_PTX = R"(
    .version 3.1
    .target sm_20
    .address_size 64
    .visible .entry add_mystery_value(
        .param .f32 float_literal,
        .param .u64 result_loc
        ) {
      .reg .u64 %rl<2>;
      .reg .f32 %f<2>;
      ld.param.f32 %f1, [float_literal];
      ld.param.u64 %rl1, [result_loc];
      add.f32 %f1, %f1, 123.0;
      st.f32 [%rl1], %f1;
      ret;
    }
    )";

// The number of arguments expected by the kernel described in
// KERNEL_PTX_TEMPLATE.
static constexpr int KERNEL_ARITY = 2;

// The name of the kernel described in KERNEL_PTX.
static constexpr const char *KERNEL_NAME = "add_mystery_value";

// The value added to the input in the kernel described in KERNEL_PTX.
static constexpr float MYSTERY_VALUE = 123.0f;


int main() {
  std::cout << Demangle("St13bad_exception") << std::endl;
  std::cout << TransposeString(Transpose::kTranspose) << std::endl;
  std::cout << "Hello World!" << std::endl;

  auto platform =
      MultiPlatformManager::PlatformWithName("cuda").ValueOrDie();

  return 0;
}

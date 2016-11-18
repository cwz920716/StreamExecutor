#include "se/stream_executor/lib/demangle.h"
#include "se/stream_executor/blas.h"
#include "se/driver/stream_executor.h"
#include <iostream>

using namespace std;
using namespace perftools::gputools;
using namespace perftools::gputools::cuda;

using perftools::gputools::port::Demangle;
using namespace perftools::gputools::blas;

void InitializeCudaPlatform() {
  // Disabling leak checking, MultiPlatformManager does not destroy its
  // registered platforms.
  
  std::unique_ptr<cuda::CudaPlatform> platform(new cuda::CudaPlatform);
  SE_CHECK_OK(MultiPlatformManager::RegisterPlatform(std::move(platform)));
}


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
      add.f32 %f1, %f1, 153.0;
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
static constexpr float MYSTERY_VALUE = 153.0f;


int main() {
  std::cout << Demangle("St13bad_exception") << std::endl;
  std::cout << TransposeString(Transpose::kTranspose) << std::endl;
  std::cout << "Hello World!" << std::endl;

  auto platform =
      MultiPlatformManager::PlatformWithName("cuda").ValueOrDie();

  
  // Get a StreamExecutor for the chosen Platform. Multiple devices are
  // supported, we indicate here that we want to run on device 0.
  const int device_ordinal = 0;
  StreamExecutor *executor =
      platform->ExecutorForDevice(device_ordinal).ValueOrDie();

  // Create a MultiKernelLoaderSpec, which knows where to find the code for our
  // kernel. In this case, the code is stored in memory as a PTX string.
  //
  // Note that the "arity" and name specified here must match  "arity" and name
  // of the kernel defined in the PTX string.
  MultiKernelLoaderSpec kernel_loader_spec(KERNEL_ARITY);
  kernel_loader_spec.AddCudaPtxInMemory(KERNEL_PTX, KERNEL_NAME);

  // Next create a kernel handle, which we will associate with our kernel code
  // (i.e., the PTX string).  The type of this handle is a bit verbose, so we
  // create an alias for it.
  //
  // This specific type represents a kernel that takes two arguments: a floating
  // point value and a pointer to a floating point value in device memory.
  //
  // A type like this is nice to have because it enables static type checking of
  // kernel arguments when we enqueue work on a stream.
  using KernelType = TypedKernel<float, DeviceMemory<float> *>;

  // Now instantiate an object of the specific kernel type we declared above.
  // The kernel object is not yet connected with the device code that we want it
  // to run (that happens with the call to GetKernel below), so it cannot be
  // used to execute work on the device yet.
  //
  // However, the kernel object is not completely empty when it is created. From
  // the StreamExecutor passed into its constructor it knows which platform it
  // is targeted for, and it also knows which device it will run on.
  KernelType kernel(executor);

  // Use the MultiKernelLoaderSpec defined above to load the kernel code onto
  // the device pointed to by the kernel object and to make that kernel object a
  // handle to the kernel code loaded on that device.
  //
  // The MultiKernelLoaderSpec may contain code for several different platforms,
  // but the kernel object has an associated platform, so there is no confusion
  // about which code should be loaded.
  //
  // After this call the kernel object can be used to launch its kernel on its
  // device.
  executor->GetKernel(kernel_loader_spec, &kernel);

  // Allocate memory in the device memory space to hold the result of the kernel
  // call. This memory will be freed when this object goes out of scope.
  ScopedDeviceMemory<float> result = executor->AllocateOwnedScalar<float>();

  // Create a stream on which to schedule device operations.
  Stream stream(executor);

  // Schedule a kernel launch on the new stream and block until the kernel
  // completes. The kernel call executes asynchronously on the device, so we
  // could do more work on the host before calling BlockHostUntilDone.
  const float kernel_input_argument = 42.5f;
  stream.Init()
      .ThenLaunch(ThreadDim(), BlockDim(), kernel,
                  kernel_input_argument, result.ptr())
      .BlockHostUntilDone();

  // Copy the result of the kernel call from device back to the host.
  float host_result = 0.0f;
  executor->SynchronousMemcpyD2H(result.cref(), sizeof(host_result),
                                 &host_result);

  // Verify that the correct result was computed.
  assert((kernel_input_argument + MYSTERY_VALUE) == host_result);

  std::cout << "host_result= " << host_result << std::endl;

  return 0;
}

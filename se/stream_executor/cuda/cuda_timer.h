// Defines the CUDATimer type - the CUDA-specific implementation of the generic
// StreamExecutor Timer interface.

#ifndef TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_TIMER_H_
#define TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_TIMER_H_

#include "se/stream_executor/stream_executor_internal.h"
#include "se/stream_executor/cuda/cuda_driver.h"
#include "se/stream_executor/cuda/cuda_gpu_executor.h"

namespace perftools {
namespace gputools {
namespace cuda {

class CUDAExecutor;
class CUDAStream;

// Wraps a pair of CUevents in order to satisfy the platform-independent
// TimerInferface -- both a start and a stop event are present which may be
// recorded in a stream.
class CUDATimer : public internal::TimerInterface {
 public:
  explicit CUDATimer(CUDAExecutor *parent)
      : parent_(parent), start_event_(nullptr), stop_event_(nullptr) {}

  // Note: teardown is explicitly handled in this API by a call to
  // StreamExecutor::DeallocateTimer(), which invokes Destroy().
  ~CUDATimer() override {}

  // Allocates the platform-specific pieces of the timer, called as part of
  // StreamExecutor::AllocateTimer().
  bool Init();

  // Deallocates the platform-specific pieces of the timer, called as part of
  // StreamExecutor::DeallocateTimer().
  void Destroy();

  // Records the "timer start" event at the current point in the stream.
  bool Start(CUDAStream *stream);

  // Records the "timer stop" event at the current point in the stream.
  bool Stop(CUDAStream *stream);

  // Returns the elapsed time, in milliseconds, between the start and stop
  // events.
  float GetElapsedMilliseconds() const;

  // See perftools::gputools::Timer::Microseconds().
  // TODO(leary) make this into an error code interface...
  uint64 Microseconds() const override {
    return GetElapsedMilliseconds() * 1e3;
  }

  // See perftools::GPUTools::Timer::Nanoseconds().
  uint64 Nanoseconds() const override { return GetElapsedMilliseconds() * 1e6; }

 private:
  CUDAExecutor *parent_;
  CUevent start_event_;  // Event recorded to indicate the "start" timestamp
                         // executing in a stream.
  CUevent stop_event_;   // Event recorded to indicate the "stop" timestamp
                         // executing in a stream.
};

}  // namespace cuda
}  // namespace gputools
}  // namespace perftools

#endif  // TENSORFLOW_STREAM_EXECUTOR_CUDA_CUDA_TIMER_H_

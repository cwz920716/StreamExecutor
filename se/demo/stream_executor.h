/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_PLATFORM_DEFAULT_STREAM_EXECUTOR_H_
#define TENSORFLOW_PLATFORM_DEFAULT_STREAM_EXECUTOR_H_

// IWYU pragma: private, include "third_party/tensorflow/core/platform/stream_executor.h"
// IWYU pragma: friend third_party/tensorflow/core/platform/stream_executor.h

#include "se/stream_executor/device_memory.h"
#include "se/stream_executor/dnn.h"
#include "se/stream_executor/dso_loader.h"
#include "se/stream_executor/event.h"
#include "se/stream_executor/lib/status.h"
#include "se/stream_executor/lib/initialize.h"
#include "se/stream_executor/multi_platform_manager.h"
#include "se/stream_executor/platform.h"
#include "se/stream_executor/scratch_allocator.h"
#include "se/stream_executor/stream.h"
#include "se/stream_executor/stream_executor.h"

#include "se/stream_executor/cuda/cuda_platform.h"

void InitializeCudaPlatform() {
  // Disabling leak checking, MultiPlatformManager does not destroy its
  // registered platforms.
  
  std::unique_ptr<perftools::gputools::cuda::CudaPlatform> platform(new perftools::gputools::cuda::CudaPlatform);
  SE_CHECK_OK(perftools::gputools::MultiPlatformManager::RegisterPlatform(std::move(platform)));
}


namespace tensorflow {

namespace gpu = ::perftools::gputools;

// On the open-source platform, stream_executor currently uses
// tensorflow::Status
inline Status FromStreamExecutorStatus(
    const perftools::gputools::port::Status& s) {
  return s;
}

}  // namespace tensorflow

#endif  // TENSORFLOW_PLATFORM_DEFAULT_STREAM_EXECUTOR_H_

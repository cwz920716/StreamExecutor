#ifndef TENSORFLOW_STREAM_EXECUTOR_PLATFORM_MUTEX_H_
#define TENSORFLOW_STREAM_EXECUTOR_PLATFORM_MUTEX_H_

#include "se/stream_executor/platform/port.h"

#if defined(PLATFORM_GOOGLE)
#include "tensorflow/stream_executor/platform/google/mutex.h"
#else
#include "default/mutex.h"
#endif

#endif  // TENSORFLOW_STREAM_EXECUTOR_PLATFORM_MUTEX_H_

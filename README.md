# StreamExecutor

I start this project since I need a standalone StreamExecutor runtime library (https://github.com/henline/streamexecutordoc). However, at the time of writing, StreamExecutor has not been open-sourced (or I haven't found it) so I decide to make my own library. The code will be mostly ported form tensorflow/stream_executor but I want to make it a standalone library for my own purpose.

To build this project, it assumes you have bazel (https://bazel.build/versions/master/docs/install.html) and Google Protocol Buffer (https://github.com/google/protobuf/blob/master/src/README.md) installed system wide. 

This project will stay sync with Tensorflow tagged releases to make sure it is up-to-date. For now, it is with Tensorflow v0.11.0. However, it may be deprecated as soon as a better alternative (e.g., Google's official StreamExecutor release) is available. 

Note: This project is only for in-house research purpose and all code/licenses are from Google's Tensorflow project unless stated otherwise. 

Changes in tensorflow 0.12.0.rc0:

stream_executor:

  device_memory.h

  dso_loader.cc

  lib/env.h

  lib/process_state.cc

  lib/static_threadlocal.h

  cuda/cuda_blas.cc

  cuda/cuda_diagnostics.cc

  cuda/cuda_dnn.cc

  cuda/cuda_driver.cc

  cuda/cuda_fft.cc

  cuda/cuda_rng.cc

  core/platform/env.cc

  core/platform/env.h

  core/platform/file_statistics.h

  core/platform/file_system.cc

  core/platform/file_system.h

  core/platform/mem.h

  core/platform/mutex.h

  core/platform/notification.h

  core/platform/platform.h

  core/platform/protobuf.h

  core/platform/protobuf_util.cc

  core/platform/stacktrace.h

  core/platform/thread_annotations.h

  core/platform/types.h

  core/platform/default/logging.cc

  core/platform/default/dynamic_annotations.h

  core/lib/core/status.h

  core/lib/core/threadpool.cc

  core/lib/gtl/inlined_vector.h

  core/lib/hash/hash.h

  core/lib/io/path.cc

  core/lib/io/path.h

  core/lib/strings/numbers.cc

  core/lib/strings/str_util.cc

  core/lib/strings/str_util.h

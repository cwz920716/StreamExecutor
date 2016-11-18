#ifndef TENSORFLOW_STREAM_EXECUTOR_LIB_DEMANGLE_H_
#define TENSORFLOW_STREAM_EXECUTOR_LIB_DEMANGLE_H_

#include "se/stream_executor/platform/port.h"

namespace perftools {
namespace gputools {
namespace port {

string Demangle(const char* mangled);

}  // namespace port
}  // namespace gputools
}  // namespace perftools

#endif  // TENSORFLOW_STREAM_EXECUTOR_LIB_DEMANGLE_H_

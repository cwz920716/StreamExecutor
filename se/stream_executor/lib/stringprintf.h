#ifndef TENSORFLOW_STREAM_EXECUTOR_LIB_STRINGPRINTF_H_
#define TENSORFLOW_STREAM_EXECUTOR_LIB_STRINGPRINTF_H_

namespace perftools {
namespace gputools {
namespace port {

// Return a C++ string
extern string Printf(const char* format, ...)
    // Tell the compiler to do printf format string checking.
    TF_PRINTF_ATTRIBUTE(1, 2);

// Append result to a supplied string
extern void Appendf(string* dst, const char* format, ...)
    // Tell the compiler to do printf format string checking.
    TF_PRINTF_ATTRIBUTE(2, 3);

// Lower-level routine that takes a va_list and appends to a specified
// string.  All other routines are just convenience wrappers around it.
extern void Appendv(string* dst, const char* format, va_list ap);

}  // namespace port
}  // namespace gputools
}  // namespace perftools

#endif  // TENSORFLOW_STREAM_EXECUTOR_LIB_STRINGPRINTF_H_

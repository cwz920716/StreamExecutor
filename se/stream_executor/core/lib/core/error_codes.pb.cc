// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: error_codes.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "error_codes.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace tensorflow {
namespace error {

namespace {

const ::google::protobuf::EnumDescriptor* Code_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_error_5fcodes_2eproto() GOOGLE_ATTRIBUTE_COLD;
void protobuf_AssignDesc_error_5fcodes_2eproto() {
  protobuf_AddDesc_error_5fcodes_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "error_codes.proto");
  GOOGLE_CHECK(file != NULL);
  Code_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_error_5fcodes_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_error_5fcodes_2eproto() {
}

void protobuf_InitDefaults_error_5fcodes_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

}

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_InitDefaults_error_5fcodes_2eproto_once_);
void protobuf_InitDefaults_error_5fcodes_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_InitDefaults_error_5fcodes_2eproto_once_,
                 &protobuf_InitDefaults_error_5fcodes_2eproto_impl);
}
void protobuf_AddDesc_error_5fcodes_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  protobuf_InitDefaults_error_5fcodes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021error_codes.proto\022\020tensorflow.error*\204\003"
    "\n\004Code\022\006\n\002OK\020\000\022\r\n\tCANCELLED\020\001\022\013\n\007UNKNOWN"
    "\020\002\022\024\n\020INVALID_ARGUMENT\020\003\022\025\n\021DEADLINE_EXC"
    "EEDED\020\004\022\r\n\tNOT_FOUND\020\005\022\022\n\016ALREADY_EXISTS"
    "\020\006\022\025\n\021PERMISSION_DENIED\020\007\022\023\n\017UNAUTHENTIC"
    "ATED\020\020\022\026\n\022RESOURCE_EXHAUSTED\020\010\022\027\n\023FAILED"
    "_PRECONDITION\020\t\022\013\n\007ABORTED\020\n\022\020\n\014OUT_OF_R"
    "ANGE\020\013\022\021\n\rUNIMPLEMENTED\020\014\022\014\n\010INTERNAL\020\r\022"
    "\017\n\013UNAVAILABLE\020\016\022\r\n\tDATA_LOSS\020\017\022K\nGDO_NO"
    "T_USE_RESERVED_FOR_FUTURE_EXPANSION_USE_"
    "DEFAULT_IN_SWITCH_INSTEAD_\020\024B1\n\030org.tens"
    "orflow.frameworkB\020ErrorCodesProtosP\001\370\001\001b"
    "\006proto3", 487);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "error_codes.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_error_5fcodes_2eproto);
}

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_error_5fcodes_2eproto_once_);
void protobuf_AddDesc_error_5fcodes_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_error_5fcodes_2eproto_once_,
                 &protobuf_AddDesc_error_5fcodes_2eproto_impl);
}
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_error_5fcodes_2eproto {
  StaticDescriptorInitializer_error_5fcodes_2eproto() {
    protobuf_AddDesc_error_5fcodes_2eproto();
  }
} static_descriptor_initializer_error_5fcodes_2eproto_;
const ::google::protobuf::EnumDescriptor* Code_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Code_descriptor_;
}
bool Code_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 20:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace error
}  // namespace tensorflow

// @@protoc_insertion_point(global_scope)

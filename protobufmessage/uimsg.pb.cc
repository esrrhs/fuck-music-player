// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "uimsg.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ui {

namespace {

const ::google::protobuf::Descriptor* uimsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  uimsg_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* uimsg_type_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_uimsg_2eproto() {
  protobuf_AddDesc_uimsg_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "uimsg.proto");
  GOOGLE_CHECK(file != NULL);
  uimsg_descriptor_ = file->message_type(0);
  static const int uimsg_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, required_type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, optional_x_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, optional_y_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, optional_key_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, optional_wheel_),
  };
  uimsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      uimsg_descriptor_,
      uimsg::default_instance_,
      uimsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(uimsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(uimsg));
  uimsg_type_descriptor_ = uimsg_descriptor_->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_uimsg_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    uimsg_descriptor_, &uimsg::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_uimsg_2eproto() {
  delete uimsg::default_instance_;
  delete uimsg_reflection_;
}

void protobuf_AddDesc_uimsg_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\013uimsg.proto\022\002ui\"\254\002\n\005uimsg\022%\n\rrequired_"
    "type\030\001 \002(\0162\016.ui.uimsg.type\022\022\n\noptional_x"
    "\030\002 \001(\002\022\022\n\noptional_y\030\003 \001(\002\022\024\n\014optional_k"
    "ey\030\004 \001(\005\022\026\n\016optional_wheel\030\005 \001(\005\"\245\001\n\004typ"
    "e\022\r\n\tleft_down\020\001\022\013\n\007left_up\020\002\022\016\n\nright_d"
    "own\020\003\022\014\n\010right_up\020\004\022\016\n\nmouse_move\020\005\022\014\n\010k"
    "ey_down\020\006\022\n\n\006key_up\020\007\022\014\n\010key_char\020\010\022\020\n\014c"
    "lose_window\020\t\022\017\n\013mouse_wheel\020\n\022\010\n\004size\020\013"
    "B\002H\001", 324);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "uimsg.proto", &protobuf_RegisterTypes);
  uimsg::default_instance_ = new uimsg();
  uimsg::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_uimsg_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_uimsg_2eproto {
  StaticDescriptorInitializer_uimsg_2eproto() {
    protobuf_AddDesc_uimsg_2eproto();
  }
} static_descriptor_initializer_uimsg_2eproto_;


// ===================================================================

const ::google::protobuf::EnumDescriptor* uimsg_type_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return uimsg_type_descriptor_;
}
bool uimsg_type_IsValid(int value) {
  switch(value) {
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
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const uimsg_type uimsg::left_down;
const uimsg_type uimsg::left_up;
const uimsg_type uimsg::right_down;
const uimsg_type uimsg::right_up;
const uimsg_type uimsg::mouse_move;
const uimsg_type uimsg::key_down;
const uimsg_type uimsg::key_up;
const uimsg_type uimsg::key_char;
const uimsg_type uimsg::close_window;
const uimsg_type uimsg::mouse_wheel;
const uimsg_type uimsg::size;
const uimsg_type uimsg::type_MIN;
const uimsg_type uimsg::type_MAX;
const int uimsg::type_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int uimsg::kRequiredTypeFieldNumber;
const int uimsg::kOptionalXFieldNumber;
const int uimsg::kOptionalYFieldNumber;
const int uimsg::kOptionalKeyFieldNumber;
const int uimsg::kOptionalWheelFieldNumber;
#endif  // !_MSC_VER

uimsg::uimsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void uimsg::InitAsDefaultInstance() {
}

uimsg::uimsg(const uimsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void uimsg::SharedCtor() {
  _cached_size_ = 0;
  required_type_ = 1;
  optional_x_ = 0;
  optional_y_ = 0;
  optional_key_ = 0;
  optional_wheel_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

uimsg::~uimsg() {
  SharedDtor();
}

void uimsg::SharedDtor() {
  if (this != default_instance_) {
  }
}

void uimsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* uimsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return uimsg_descriptor_;
}

const uimsg& uimsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_uimsg_2eproto();  return *default_instance_;
}

uimsg* uimsg::default_instance_ = NULL;

uimsg* uimsg::New() const {
  return new uimsg;
}

void uimsg::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    required_type_ = 1;
    optional_x_ = 0;
    optional_y_ = 0;
    optional_key_ = 0;
    optional_wheel_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool uimsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .ui.uimsg.type required_type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::ui::uimsg_type_IsValid(value)) {
            set_required_type(static_cast< ::ui::uimsg_type >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(21)) goto parse_optional_x;
        break;
      }
      
      // optional float optional_x = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED32) {
         parse_optional_x:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &optional_x_)));
          set_has_optional_x();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(29)) goto parse_optional_y;
        break;
      }
      
      // optional float optional_y = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED32) {
         parse_optional_y:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &optional_y_)));
          set_has_optional_y();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_optional_key;
        break;
      }
      
      // optional int32 optional_key = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_optional_key:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &optional_key_)));
          set_has_optional_key();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_optional_wheel;
        break;
      }
      
      // optional int32 optional_wheel = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_optional_wheel:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &optional_wheel_)));
          set_has_optional_wheel();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void uimsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .ui.uimsg.type required_type = 1;
  if (has_required_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->required_type(), output);
  }
  
  // optional float optional_x = 2;
  if (has_optional_x()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(2, this->optional_x(), output);
  }
  
  // optional float optional_y = 3;
  if (has_optional_y()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(3, this->optional_y(), output);
  }
  
  // optional int32 optional_key = 4;
  if (has_optional_key()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->optional_key(), output);
  }
  
  // optional int32 optional_wheel = 5;
  if (has_optional_wheel()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(5, this->optional_wheel(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* uimsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .ui.uimsg.type required_type = 1;
  if (has_required_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->required_type(), target);
  }
  
  // optional float optional_x = 2;
  if (has_optional_x()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(2, this->optional_x(), target);
  }
  
  // optional float optional_y = 3;
  if (has_optional_y()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(3, this->optional_y(), target);
  }
  
  // optional int32 optional_key = 4;
  if (has_optional_key()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->optional_key(), target);
  }
  
  // optional int32 optional_wheel = 5;
  if (has_optional_wheel()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(5, this->optional_wheel(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int uimsg::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .ui.uimsg.type required_type = 1;
    if (has_required_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->required_type());
    }
    
    // optional float optional_x = 2;
    if (has_optional_x()) {
      total_size += 1 + 4;
    }
    
    // optional float optional_y = 3;
    if (has_optional_y()) {
      total_size += 1 + 4;
    }
    
    // optional int32 optional_key = 4;
    if (has_optional_key()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->optional_key());
    }
    
    // optional int32 optional_wheel = 5;
    if (has_optional_wheel()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->optional_wheel());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void uimsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const uimsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const uimsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void uimsg::MergeFrom(const uimsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_required_type()) {
      set_required_type(from.required_type());
    }
    if (from.has_optional_x()) {
      set_optional_x(from.optional_x());
    }
    if (from.has_optional_y()) {
      set_optional_y(from.optional_y());
    }
    if (from.has_optional_key()) {
      set_optional_key(from.optional_key());
    }
    if (from.has_optional_wheel()) {
      set_optional_wheel(from.optional_wheel());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void uimsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void uimsg::CopyFrom(const uimsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool uimsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  return true;
}

void uimsg::Swap(uimsg* other) {
  if (other != this) {
    std::swap(required_type_, other->required_type_);
    std::swap(optional_x_, other->optional_x_);
    std::swap(optional_y_, other->optional_y_);
    std::swap(optional_key_, other->optional_key_);
    std::swap(optional_wheel_, other->optional_wheel_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata uimsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = uimsg_descriptor_;
  metadata.reflection = uimsg_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace ui

// @@protoc_insertion_point(global_scope)

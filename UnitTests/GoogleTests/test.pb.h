// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_test_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_test_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3007000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3007001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h> // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_test_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal
{
class AnyMetadata;
} // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_test_2eproto
{
    static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField
        entries[] PROTOBUF_SECTION_VARIABLE(protodesc_cold);
    static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField
        aux[] PROTOBUF_SECTION_VARIABLE(protodesc_cold);
    static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable
                                                                       schema[1] PROTOBUF_SECTION_VARIABLE(protodesc_cold);
    static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata      field_metadata[];
    static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
    static const ::PROTOBUF_NAMESPACE_ID::uint32                       offsets[];
};
void AddDescriptors_test_2eproto();
namespace core_lib_test
{
class TestMessage;
class TestMessageDefaultTypeInternal;
extern TestMessageDefaultTypeInternal _TestMessage_default_instance_;
} // namespace core_lib_test
PROTOBUF_NAMESPACE_OPEN
template <>
::core_lib_test::TestMessage* Arena::CreateMaybeMessage<::core_lib_test::TestMessage>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace core_lib_test
{

// ===================================================================

class TestMessage
    : public ::PROTOBUF_NAMESPACE_ID::
          Message /* @@protoc_insertion_point(class_definition:core_lib_test.TestMessage)
                   */
{
public:
    TestMessage();
    virtual ~TestMessage();

    TestMessage(const TestMessage& from);
    TestMessage(TestMessage&& from) noexcept
        : TestMessage()
    {
        *this = ::std::move(from);
    }

    inline TestMessage& operator=(const TestMessage& from)
    {
        CopyFrom(from);
        return *this;
    }
    inline TestMessage& operator=(TestMessage&& from) noexcept
    {
        if (GetArenaNoVirtual() == from.GetArenaNoVirtual())
        {
            if (this != &from)
                InternalSwap(&from);
        }
        else
        {
            CopyFrom(from);
        }
        return *this;
    }

    static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor()
    {
        return default_instance().GetDescriptor();
    }
    static const TestMessage& default_instance();

    static void                      InitAsDefaultInstance(); // FOR INTERNAL USE ONLY
    static inline const TestMessage* internal_default_instance()
    {
        return reinterpret_cast<const TestMessage*>(&_TestMessage_default_instance_);
    }
    static constexpr int kIndexInFileMessages = 0;

    void        Swap(TestMessage* other);
    friend void swap(TestMessage& a, TestMessage& b)
    {
        a.Swap(&b);
    }

    // implements Message ----------------------------------------------

    inline TestMessage* New() const final
    {
        return CreateMaybeMessage<TestMessage>(nullptr);
    }

    TestMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final
    {
        return CreateMaybeMessage<TestMessage>(arena);
    }
    void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
    void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
    void CopyFrom(const TestMessage& from);
    void MergeFrom(const TestMessage& from);
    PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
    bool                                  IsInitialized() const final;

    size_t ByteSizeLong() const final;
#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
    const char* _InternalParse(const char*                                      ptr,
                               ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
#else
    bool MergePartialFromCodedStream(::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
#endif // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
    void
    SerializeWithCachedSizes(::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
    ::PROTOBUF_NAMESPACE_ID::uint8*
        InternalSerializeWithCachedSizesToArray(::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
    int GetCachedSize() const final
    {
        return _cached_size_.Get();
    }

private:
    inline void SharedCtor();
    inline void SharedDtor();
    void        SetCachedSize(int size) const final;
    void        InternalSwap(TestMessage* other);
    friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
    static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName()
    {
        return "core_lib_test.TestMessage";
    }

private:
    inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const
    {
        return nullptr;
    }
    inline void* MaybeArenaPtr() const
    {
        return nullptr;
    }

public:
    ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // repeated double values = 3;
    int                                                   values_size() const;
    void                                                  clear_values();
    static const int                                      kValuesFieldNumber = 3;
    double                                                values(int index) const;
    void                                                  set_values(int index, double value);
    void                                                  add_values(double value);
    const ::PROTOBUF_NAMESPACE_ID::RepeatedField<double>& values() const;
    ::PROTOBUF_NAMESPACE_ID::RepeatedField<double>*       mutable_values();

    // string name = 1;
    void               clear_name();
    static const int   kNameFieldNumber = 1;
    const std::string& name() const;
    void               set_name(const std::string& value);
    void               set_name(std::string&& value);
    void               set_name(const char* value);
    void               set_name(const char* value, size_t size);
    std::string*       mutable_name();
    std::string*       release_name();
    void               set_allocated_name(std::string* name);

    // uint64 counter = 2;
    void                            clear_counter();
    static const int                kCounterFieldNumber = 2;
    ::PROTOBUF_NAMESPACE_ID::uint64 counter() const;
    void                            set_counter(::PROTOBUF_NAMESPACE_ID::uint64 value);

    // @@protoc_insertion_point(class_scope:core_lib_test.TestMessage)
private:
    class HasBitSetters;

    ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
    ::PROTOBUF_NAMESPACE_ID::RepeatedField<double>               values_;
    mutable std::atomic<int>                                     _values_cached_byte_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr            name_;
    ::PROTOBUF_NAMESPACE_ID::uint64                              counter_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize        _cached_size_;
    friend struct ::TableStruct_test_2eproto;
};
// ===================================================================

// ===================================================================

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif // __GNUC__
// TestMessage

// string name = 1;
inline void TestMessage::clear_name()
{
    name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& TestMessage::name() const
{
    // @@protoc_insertion_point(field_get:core_lib_test.TestMessage.name)
    return name_.GetNoArena();
}
inline void TestMessage::set_name(const std::string& value)
{

    name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
    // @@protoc_insertion_point(field_set:core_lib_test.TestMessage.name)
}
inline void TestMessage::set_name(std::string&& value)
{

    name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
                     ::std::move(value));
    // @@protoc_insertion_point(field_set_rvalue:core_lib_test.TestMessage.name)
}
inline void TestMessage::set_name(const char* value)
{
    GOOGLE_DCHECK(value != nullptr);

    name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
                     ::std::string(value));
    // @@protoc_insertion_point(field_set_char:core_lib_test.TestMessage.name)
}
inline void TestMessage::set_name(const char* value, size_t size)
{

    name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
                     ::std::string(reinterpret_cast<const char*>(value), size));
    // @@protoc_insertion_point(field_set_pointer:core_lib_test.TestMessage.name)
}
inline std::string* TestMessage::mutable_name()
{

    // @@protoc_insertion_point(field_mutable:core_lib_test.TestMessage.name)
    return name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* TestMessage::release_name()
{
    // @@protoc_insertion_point(field_release:core_lib_test.TestMessage.name)

    return name_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void TestMessage::set_allocated_name(std::string* name)
{
    if (name != nullptr)
    {
    }
    else
    {
    }
    name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
                              name);
    // @@protoc_insertion_point(field_set_allocated:core_lib_test.TestMessage.name)
}

// uint64 counter = 2;
inline void TestMessage::clear_counter()
{
    counter_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 TestMessage::counter() const
{
    // @@protoc_insertion_point(field_get:core_lib_test.TestMessage.counter)
    return counter_;
}
inline void TestMessage::set_counter(::PROTOBUF_NAMESPACE_ID::uint64 value)
{

    counter_ = value;
    // @@protoc_insertion_point(field_set:core_lib_test.TestMessage.counter)
}

// repeated double values = 3;
inline int TestMessage::values_size() const
{
    return values_.size();
}
inline void TestMessage::clear_values()
{
    values_.Clear();
}
inline double TestMessage::values(int index) const
{
    // @@protoc_insertion_point(field_get:core_lib_test.TestMessage.values)
    return values_.Get(index);
}
inline void TestMessage::set_values(int index, double value)
{
    values_.Set(index, value);
    // @@protoc_insertion_point(field_set:core_lib_test.TestMessage.values)
}
inline void TestMessage::add_values(double value)
{
    values_.Add(value);
    // @@protoc_insertion_point(field_add:core_lib_test.TestMessage.values)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedField<double>& TestMessage::values() const
{
    // @@protoc_insertion_point(field_list:core_lib_test.TestMessage.values)
    return values_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedField<double>* TestMessage::mutable_values()
{
    // @@protoc_insertion_point(field_mutable_list:core_lib_test.TestMessage.values)
    return &values_;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

// @@protoc_insertion_point(namespace_scope)

} // namespace core_lib_test

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_test_2eproto
#pragma once
#include "BasicTypeAliases.h"
#include <string>
#include <meta>

#include "TypeInfo.h"
#include "TypeRegister.h"


namespace KE
{

class IReflectedType
{
  private:
    inline static KE::TypeInfo const* const m_TypeInfo = nullptr;

  public:
    IReflectedType()                  = default;
    virtual ~IReflectedType()         = default;
    virtual u64         GetTypeID()   = 0;
    virtual std::string GetTypeName() = 0;
};

} // namespace KE

#define KE_REFLECT(TypeName)                                                                                           \
  private:                                                                                                             \
    static KE::TypeInfo const* GetTypeInfo()                                                                           \
    {                                                                                                                  \
        static KE::TypeInfo const* typeInfo =                                                                          \
            KE::TypeRegister::RegisterType(std::define_static_string(std::meta::display_string_of(^^TypeName)));       \
        return typeInfo;                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
  public:                                                                                                              \
    virtual u64 GetTypeID() override                                                                                   \
    {                                                                                                                  \
        return GetTypeInfo()->typeID;                                                                                  \
    }                                                                                                                  \
    static u64 GetStaticTypeID()                                                                                       \
    {                                                                                                                  \
        return GetTypeInfo()->typeID;                                                                                  \
    }                                                                                                                  \
    virtual std::string GetTypeName() override                                                                         \
    {                                                                                                                  \
        return #TypeName;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
  private:

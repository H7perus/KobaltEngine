#pragma once
#include "BasicTypeAliases.h"
#include <memory>
#include <string>


#include "TypeInfo.h"
#include "TypeRegister.h"


namespace KE
{

class IReflectedType
{
  private:
    inline static KE::TypeInfo const *const m_TypeInfo = nullptr;

  public:
    IReflectedType()                  = default;
    virtual ~IReflectedType()         = default;
    virtual u32         GetTypeID()   = 0;                                                                                                                 \
    virtual std::string GetTypeName() = 0;
};

} // namespace KE

#define KE_REFLECT(TypeName)                                                                                           \
    inline static KE::TypeInfo const *const m_TypeInfo = KE::TypeRegister::RegisterType(#TypeName);                          \
                                                                                                                       \
  public:                                                                                                              \
    virtual u32 GetTypeID() override                                                                                   \
    {                                                                                                                  \
        return m_TypeInfo->typeID;                                                                                     \
    }                                                                                                                  \
    static u32 GetStaticTypeID()                                                                              \
    {                                                                                                                  \
        return m_TypeInfo->typeID;                                                                                     \
    }                                                                                                                  \
    virtual std::string GetTypeName() override                                                                         \
    {                                                                                                                  \
        return #TypeName;                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
  private:\

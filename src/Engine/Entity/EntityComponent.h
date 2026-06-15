#pragma once


#include "TypeSystem/ReflectedType.h"

#include <functional>
#include <memory>

#include "BasicTypeAliases.h"

namespace KE
{
class IEntityComponent : public IReflectedType
{
    u8 loadState_ = 0;


  public:
    IEntityComponent() = default;

    virtual void Initialise() = 0;
    virtual void Destroy()    = 0;
};

struct ComponentKey
{
    u64 componentTypeID;

    ComponentKey(u64 componentTypeID) : componentTypeID(componentTypeID)
    {
    }
};

template<std::derived_from<IEntityComponent> T>
ComponentKey GetKey()
{
    return ComponentKey(T::GetStaticTypeID());
}

// template <std::derived_from<IEntityComponent> T> constexpr ComponentInfo MakeComponentInfo(u64 id, std::string name)
// {
//     return ComponentInfo{
//         .componentTypeID = id,
//         .componentTypeName = std::move(name),
//         .Create = []() { return std::make_shared<T>(); }};
// }

} // namespace KE

#define KE_ENTITYCOMPONENT(entityComponent) KE_REFLECT(entityComponent)
#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"
#include <concepts>
#include <functional>
#include <stdexcept>
#include <string>
#include <cstring>

#include "EngineDLL.h"
#include "TypeSystem/ReflectedType.h"
#include "Util/FNV1a.h"

#include <memory>

namespace KE
{
using ComponentCreator = std::function<std::shared_ptr<IEntityComponent>()>;


struct EntityTypeDescriptor
{
    u64                       classID; // FNV1aTo64(identifier_of(^^T)), persisted / networked
    const char*               className;
    std::unique_ptr<Entity::Entity> (*create)();
};

class ENGINE_DLL_API EntityFactory
{
  public:
    template<std::derived_from<KE::Entity::Entity> T>
    void Register()
    {
        constexpr auto hash = FNV1aTo64(std::meta::identifier_of(^^T));

        EntityTypeDescriptor descriptor
        {
            .classID = hash,
            .className = std::define_static_string(std::meta::identifier_of(^^T)),
            .create = []() -> std::unique_ptr<KE::Entity::Entity> 
            { 
                std::unique_ptr<Entity::Entity> ent = std::make_unique<T>();
                ent->classID_ = hash;

                return ent;
            }
        };

        auto [it, inserted] = entityDescriptors.emplace(hash, descriptor);
        if (!inserted && std::strcmp(it->second.className, descriptor.className) != 0)
            throw std::invalid_argument("Entity Name already exists!!!");
    }

    template<std::derived_from<KE::Entity::Entity> T>
    std::unique_ptr<Entity::Entity> Create()
    {
        constexpr auto hash = FNV1aTo64(std::meta::identifier_of(^^T));

        return Create(hash);
    }

    std::unique_ptr<Entity::Entity> Create(u64 hash)
    {
        if (!entityDescriptors.contains(hash))
        {
            throw std::logic_error("Entity does not exist!");
        }
        return entityDescriptors.at(hash).create();
    }

  private:
    std::unordered_map<u64, EntityTypeDescriptor, IdentityHash>  entityDescriptors;
};
} // namespace KE

#define KE_ENTITYARCHETYPE(entityType, components)                                                                     \
    KE::EntityKey entityType##Key = KE::EntityFactory::RegisterEntityArchetype(#entityType, components);

#define KE_REGISTERCOMPONENT(componentType) \
    KE::ComponentKey componentType##Key = KE::EntityFactory::RegisterComponentType<componentType>(#componentType);

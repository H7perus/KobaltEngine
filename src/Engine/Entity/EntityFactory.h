#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"
#include "Engine/Entity/TypedEntity.h"
#include <functional>
#include <stdexcept>
#include <string>


#include "EngineDLL.h"
#include "Util/FNV1a.h"

#include <memory>

namespace KE
{
using ComponentCreator = std::function<std::shared_ptr<IEntityComponent>()>;

class ENGINE_DLL_API EntityFactory
{
  public:
    static EntityKey RegisterEntityArchetype(const char* EntityTypeName, std::vector<ComponentKey> components)
    {
        u64 hashedName = FNV1aTo64(EntityTypeName);

        EntityArchetype archetype{hashedName, EntityTypeName, components};

        if (entityArchetypes_.find(hashedName) == entityArchetypes_.end())
        {
            entityArchetypes_[hashedName] = archetype;
        }
        else
        {
            throw std::logic_error("Entity name already exists!");
        };

        return EntityKey(hashedName);
    }

    template <std::derived_from<IEntityComponent> T>
    static ComponentKey RegisterComponentType(const char* EntityComponentTypeName)
    {
        u64 hashedName                 = FNV1aTo64(EntityComponentTypeName);
        componentCreators_[hashedName] = [] { return std::make_shared<T>(); };
        return ComponentKey(hashedName);
    }

    static std::shared_ptr<Entity> Create(const char* EntityTypeName);

    static std::shared_ptr<Entity> Create(u64& hashedName);

  private:
    static std::unordered_map<u64, EntityArchetype>  entityArchetypes_;
    static std::unordered_map<u64, ComponentCreator> componentCreators_;
};
} // namespace KE

#define KE_ENTITYARCHETYPE(entityType, components)                                                                     \
    KE::EntityKey entityType##Key = KE::EntityFactory::RegisterEntityArchetype(#entityType, components);

#define KE_REGISTERCOMPONENT(componentType) \
    KE::ComponentKey componentType##Key = KE::EntityFactory::RegisterComponentType<componentType>(#componentType);

#include "Engine/Entity/EntityFactory.h"

#include "Util/FNV1a.h"
#include <stdexcept>

namespace KE
{
using Creator = std::function<std::shared_ptr<Entity>()>;

std::unordered_map<u64, EntityArchetype>  EntityFactory::entityArchetypes_;
std::unordered_map<u64, ComponentCreator> EntityFactory::componentCreators_;

std::shared_ptr<Entity> EntityFactory::Create(const char* EntityTypeName)
{
    u64 hashedName = FNV1aTo64(EntityTypeName);
    
    return Create(hashedName);
}

std::shared_ptr<Entity> EntityFactory::Create(u64& hashedName)
{
    if (!entityArchetypes_.contains(hashedName))
    {
        throw std::logic_error("Entity does not exist!");
    }

    auto& entityArchetype = entityArchetypes_[hashedName];

    std::shared_ptr<Entity> entity =
        std::make_shared<Entity>(entityArchetype.entityTypeName, entityArchetype.entityTypeID);

    for (auto& componentKey : entityArchetype.components)
    {

        auto localVar = componentKey.componentTypeID;
        
        if (!componentCreators_.contains(componentKey.componentTypeID))
        {
            throw std::logic_error("Entity does not exist!");
        }
        entity->components_.push_back(componentCreators_[componentKey.componentTypeID]());
    }
    return entity;
}


} // namespace KE
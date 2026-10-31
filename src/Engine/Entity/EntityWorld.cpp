#include "EntityWorld.h"
#include "Engine/Entity/Entity.h"
#include "EntityWorldSystem.h"
#include <memory>


KE::EntityWorld::EntityWorld()
{
}

// std::shared_ptr<KE::Entity::Entity> KE::EntityWorld::AddEntity(const char* entityTypeName)
// {
//     return AddEntity(FNV1aTo64(entityTypeName));
// }

KE::Entity::Entity& KE::EntityWorld::AddEntity(u64 entityTypeID)
{
    return AddEntityWithID(entityTypeID, incrementingID_++);
}

KE::Entity::Entity& KE::EntityWorld::AddEntityWithID(u64 entityTypeID, u64 entityID) 
{
    std::unique_ptr<KE::Entity::Entity> entity = entityFactory.Create(entityTypeID);

    entity->entityID_ = entityID;
    entities.push_back(std::move(entity));

    return *entities.back();
}


// void KE::EntityWorld::RegisterEntity(std::shared_ptr<Entity>& entity)
// {
//     entity->entityID_ = incrementingID_++;
//     entities.push_back(entity);
// };

// void KE::EntityWorld::RegisterEntityWithSystems(std::shared_ptr<Entity>& entity)
// {
//     for(std::shared_ptr<EntityWorldSystem> worldSystem : worldSystems)
//     {
//         for(std::shared_ptr<IEntityComponent> comp : entity->components_)
//         {
//             worldSystem->RegisterComponent(entity, comp);
//         }
        
//     }
// };
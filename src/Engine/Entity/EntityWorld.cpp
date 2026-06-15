#include "EntityWorld.h"
#include "Engine/Entity/Entity.h"
#include "EntityWorldSystem.h"
#include <memory>


KE::EntityWorld::EntityWorld()
{
}

void KE::EntityWorld::AddEntity(const char* entityTypeName)
{
    AddEntity(FNV1aTo64(entityTypeName));
}

void KE::EntityWorld::AddEntity(u64 entityTypeID)
{
    AddEntityWithID(entityTypeID, incrementingID_);
    incrementingID_++;
}
void KE::EntityWorld::AddEntityWithID(u64 entityTypeID, u64 entityID) 
{
    std::shared_ptr<Entity> entity = EntityFactory::Create(entityTypeID);

    entity->entityID_ = entityID;
    entities.push_back(entity);
}


void KE::EntityWorld::RegisterEntity(std::shared_ptr<Entity>& entity)
{
    entity->entityID_ = incrementingID_++;
    entities.push_back(entity);
};

void KE::EntityWorld::RegisterEntityWithSystems(std::shared_ptr<Entity>& entity)
{
    for(std::shared_ptr<EntityWorldSystem> worldSystem : worldSystems)
    {
        for(std::shared_ptr<IEntityComponent> comp : entity->components_)
        {
            worldSystem->RegisterComponent(entity.get(), comp.get());
        }
        
    }
};
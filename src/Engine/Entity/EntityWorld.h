#pragma once

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"
#include "Engine/Entity/EntityFactory.h"
#include "Engine/Entity/EntityWorldSystem.h"
#include "EngineDLL.h"
#include "Util/FNV1a.h"
#include <concepts>


namespace KE
{
class ENGINE_DLL_API EntityWorld : public IReflectedType
{
    KE_REFLECT(EntityWorld)
    u32 incrementingID_ = 1;

  public:
    EntityWorld();
    ~EntityWorld() = default;


    std::shared_ptr<Entity> AddEntity(const char* entityTypeName);

    std::shared_ptr<Entity> AddEntity(u64 entityTypeID);

    std::shared_ptr<Entity> AddEntityWithID(u64 entityTypeID, u64 entityID);

    void RegisterEntity(std::shared_ptr<Entity>& entity);

    void RegisterEntityWithSystems(std::shared_ptr<Entity>& entity);

    template <std::derived_from<IEntityComponent> T> 
    void AddComponentToEntity(std::shared_ptr<Entity>& entity)
    {
        entity->components_.push_back(std::make_shared<T>());

        for (std::shared_ptr<EntityWorldSystem> worldSystem : worldSystems)
        {
            worldSystem->RegisterComponent(entity, entity->components_.back());
        }
    }

    template <std::derived_from<EntityWorldSystem> T> void AddWorldSystem(auto&&... args)
    {
        worldSystems.push_back(std::make_shared<T>(args...));
    }

    std::vector<std::shared_ptr<Entity>>            entities;
    std::vector<std::shared_ptr<EntityWorldSystem>> worldSystems;
};
} // namespace KE
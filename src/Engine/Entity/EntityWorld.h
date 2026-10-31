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
    u64 incrementingID_ = 1;

    
  public:
    KE::EntityFactory entityFactory;

    EntityWorld();
    ~EntityWorld() = default;

    template<std::derived_from<Entity::Entity> T>
    T& AddEntity()
    {
        return static_cast<T&>(AddEntity(FNV1aTo64(std::meta::identifier_of(^^T))));
    }

    Entity::Entity& AddEntity(u64 entityTypeID);

    Entity::Entity& AddEntityWithID(u64 entityTypeID, u64 entityID);

    //void RegisterEntity(std::shared_ptr<Entity::Entity>& entity);

    //void RegisterEntityWithSystems(std::shared_ptr<Entity::Entity>& entity);

    template <std::derived_from<IEntityComponent> T> 
    void AddComponentToEntity(std::shared_ptr<Entity::Entity>& entity)
    {
        // entity->components_.push_back(std::make_shared<T>());

        // for (std::shared_ptr<EntityWorldSystem> worldSystem : worldSystems)
        // {
        //     worldSystem->RegisterComponent(entity, entity->components_.back());
        // }
    }

    // template <std::derived_from<EntityWorldSystem> T> void AddWorldSystem(auto&&... args)
    // {
    //     worldSystems.push_back(std::make_shared<T>(args...));
    // }

    std::vector<std::unique_ptr<Entity::Entity>>            entities;
    std::vector<std::unique_ptr<EntityWorldSystem>> worldSystems;
};
} // namespace KE
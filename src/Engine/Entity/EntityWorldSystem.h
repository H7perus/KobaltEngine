#pragma once

#include "Base/TypeSystem/ReflectedType.h"

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"

#include "EngineDLL.h"

namespace KE
{
class EntityWorldSystem : public IReflectedType
{

  public:
    EntityWorldSystem()                                    = default;
    virtual ~EntityWorldSystem()                           = default;
    EntityWorldSystem(const EntityWorldSystem&)            = delete;
    EntityWorldSystem& operator=(const EntityWorldSystem&) = delete;
    EntityWorldSystem(EntityWorldSystem&&)                 = default;
    EntityWorldSystem& operator=(EntityWorldSystem&&)
    {
        return *this;
    }

    virtual void RegisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component)   = 0;
    virtual void UnregisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component) = 0;
};
} // namespace KE

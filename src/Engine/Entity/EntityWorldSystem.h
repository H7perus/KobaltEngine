#pragma once

#include "Base/TypeSystem/ReflectedType.h"

#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityComponent.h"

#include "EngineDLL.h"

namespace KE
{
class ENGINE_DLL_API EntityWorldSystem : public IReflectedType
{

  public:
    EntityWorldSystem();
    virtual ~EntityWorldSystem();
    EntityWorldSystem(const EntityWorldSystem&)            = delete;
    EntityWorldSystem& operator=(const EntityWorldSystem&) = delete;
    EntityWorldSystem(EntityWorldSystem&&);
    EntityWorldSystem& operator=(EntityWorldSystem&&);

    virtual void RegisterComponent(Entity* entity, IEntityComponent* component)   = 0;
    virtual void UnregisterComponent(Entity* entity, IEntityComponent* component) = 0;
};
} // namespace KE

#pragma once
#include "Base/TypeSystem/ReflectedType.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Entity/EntityWorld.h"
#include "TypeSystem/TypeInfo.h"
#include "Base/Util/FNV1a.h"


namespace KE
{

// Idea: Entities are data only internally, but allow to have a class that holds the
class TypedEntity : public IReflectedType
{
  protected:
    std::shared_ptr<Entity> entity;

    // TypedEntity(const char* name) : entity(std::make_shared<Entity>(name))
    // {
    // }

  public:
    std::weak_ptr<Entity> GetEntity()
    {
        return entity;
    }
};


struct EntityArchetype
{
  u64 entityTypeID;
  std::string entityTypeName;
  std::vector<ComponentKey> components;
};

} // namespace KE


#define KE_ENTITY(entityClass) TypedEntity(#entityClass)
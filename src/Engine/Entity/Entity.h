#pragma once
#include "BasicTypeAliases.h"
#include "Engine/General/Transform.h"
#include "Engine/General/View.h"
#include "TypeSystem/ReflectedType.h"


#include <memory>
#include <string>


//------------------------------------------------------------
// Entity base class
//------------------------------------------------------------


namespace KE { class EntityFactory; class EntityWorld; }
namespace KE::Entity
{

struct EntityHandle
{
    u64 id = 0;   // 0 = invalid

    constexpr bool IsValid() const { return id != 0; }
    constexpr bool operator==(const EntityHandle&) const = default;
};

enum class EntityState : u8
{
    Unloaded,
    Loading,
    Loaded,
    Destroyed
};

enum class EntityFlags : u8
{
};

class Entity : IReflectedType
{
    friend class KE::EntityWorld;
    friend class KE::EntityFactory;
    EntityState state_ = EntityState::Unloaded;
    EntityFlags flags_{};
    // entity class id
    u64 classID_ = 0;
    // unique entity id
    u64 entityID_ = 0;

  public:
    // HACK: This should be hidden to anything but Entity Systems and Entity World Systems, but alas, for testing
    Transform transform_;

    glm::mat3x4 GetRelativeTransform(KE::View view)
    {
        return transform_.GetRelativeTransform(view.origin);
    }

    const bool IsValid()
    {
        return state_ == EntityState::Loaded;
    };
};
} // namespace KE::Entity

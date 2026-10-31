#pragma once

#include "Generated/ClientEntityHeaders.h"
#include "Engine/Entity/EntityScan.h"

namespace KE
{
class EntityFactory;
inline void RegisterClientEntities(EntityFactory& factory)
{
    RegisterEntitiesIn<^^KE::Entity>(factory);
};
}
#pragma once


#include "Engine/Entity/Entity.h"
#include "Renderer/Types/EntityComponents/StaticMeshComponent.h"

namespace KE::Entity
{
class TestDecorationEntity : public Entity
{
    KE_REFLECT(TestDecorationEntity)
  public:
    TestDecorationEntity()
    {
        mesh.meshPath = "../../../../../Engine/assets/error.gltf";
    }

    R::StaticMeshComponent mesh;
};

} // namespace KE::Entity

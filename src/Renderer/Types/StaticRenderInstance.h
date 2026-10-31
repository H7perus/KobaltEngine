#pragma once

#include "Renderer/Types/Mesh.h"

#include "Engine/Entity/Entity.h"
#include "Base/Include/BasicTypeAliases.h"

namespace KE::R
{

//StaticRenderInstances are tied to one transform.

class StaticRenderInstance
{
        Entity::Entity* entity;
        u32 gpuTransformIndex = 0;
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
};

}
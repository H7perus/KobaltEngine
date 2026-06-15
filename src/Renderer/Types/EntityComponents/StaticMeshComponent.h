#pragma once
#include "Engine/Entity/EntityComponent.h"
#include "Renderer/Types/Mesh.h"
#include "Renderer/Types/Submesh.h"

#include "RendererDLL.h"

namespace KE::R
{
    class RENDERER_DLL_API StaticMeshComponent : public IEntityComponent
    {
        KE_ENTITYCOMPONENT(StaticMeshComponent);
        
    public: 
        StaticMeshComponent() = default;

        
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();


        virtual void Initialise() {};

        virtual void Destroy() {};
    };
}
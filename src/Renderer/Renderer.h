#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "GpuInterface/Types/TaskGraph.h"

#include "Engine/Entity/EntityWorldSystem.h"

#include "SDL3/SDL.h"
#include "RendererDLL.h"

namespace KE::R
{
class RENDERER_DLL_API Renderer : public EntityWorldSystem
{
    KE_REFLECT(Renderer)
    VK::TaskGraph taskGraph_;
  public:
    Renderer();
    ~Renderer() = default;
    
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&)                 = default;
    Renderer& operator=(Renderer&&)      = default;


    virtual void RegisterComponent(Entity* entity, IEntityComponent* component);

    virtual void UnregisterComponent(Entity* entity, IEntityComponent* component);

    // Build up the task graph
    void Init(SDL_Window*& windowPtr);

    // Execute the task graph
    void Draw();
};
} // namespace KE
#pragma once
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "GpuInterface/Types/TaskGraph.h"
#include "GpuInterface/Types/Texture.h"

#include "Engine/Interfaces/IRenderer.h"

#include "SDL3/SDL.h"
#include "RendererDLL.h"

namespace KE::R
{
class RENDERER_DLL_API Renderer : public IRenderer
{
    KE_REFLECT(Renderer)
    VK::TaskGraph taskGraph_;

    KE::VK::Texture testTexture;
  public:
    Renderer();
    ~Renderer() = default;
    
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&)                 = default;
    Renderer& operator=(Renderer&&)      = default;


    virtual void RegisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component) override;

    virtual void UnregisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component) override;


    virtual void Init(SDL_Window*& windowPtr) override;

    virtual void Draw() override;
};
} // namespace KE
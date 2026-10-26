
#include "Renderer.h"
#include "GpuInterface/Types/TaskNodes/StageBarrier.h"
#include "Renderer/Types/RenderContext.h"
#include "Renderer/Types/TaskNodes/RenderTriangle.h"
#include "Renderer/Types/TaskNodes/TonemapAndPresent.h"
#include "Types/TaskNodes/UpdateBuffers.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;
// #include "Types/TaskNodes/OpaquePass.h"
#include "GpuInterface/Types/TaskNodes/SubmitAndPresent.h"

#include "Renderer/Types/EntityComponents/StaticMeshComponent.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_include.h"

#include "GpuInterface/Types/DeviceManager.h"
#include "GpuInterface/VkHelpers.h"

#include "Engine/Load/ModelLoad.h"

#include "vulkan/vulkan.hpp"

KE::R::Renderer::Renderer()
{
}

void KE::R::Renderer::RegisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component)
{
    auto& renderContext = R::RenderContext::GetInstance();

    if (component->GetTypeID() == R::StaticMeshComponent::GetStaticTypeID())
    {
        std::shared_ptr<R::StaticMeshComponent> comp = std::static_pointer_cast<R::StaticMeshComponent>(component);

        
        R::RenderContext::GetInstance().meshes_.try_emplace("test", comp->mesh);
        // TODO: Add mesh to render context
        if(renderContext.meshes_.count(comp->meshPath))
        {
            comp->mesh = renderContext.meshes_.at(comp->meshPath);
        }
        else 
        {
            RenderMesh renderMeshLoaded = LoadGLTFAsRenderMesh(comp->meshPath).value();

            comp->mesh = renderContext.AddMesh(renderMeshLoaded, comp->meshPath);
        }

        comp->gpuTransformIndex = renderContext.transforms.size();
        renderContext.transforms.push_back(glm::mat3x4());

        renderContext.staticOpaqueRenderables.push_back(std::pair(entity, comp));
    }
}

void KE::R::Renderer::UnregisterComponent(std::shared_ptr<Entity> entity, std::shared_ptr<IEntityComponent> component)
{
}

void KE::R::Renderer::Init(SDL_Window*& windowPtr)
{
    auto vkboot_inst_ = createInstance();
    InitDispatcherForDLL(vkboot_inst_.instance);

    VK::ContextManager::Init();
    VK::ContextManager::SetVkInstance(vkboot_inst_);

    auto inst = VK::ContextManager::GetVkInstance();

    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(inst);
    
    vk::SurfaceKHR surface;
    bool           test = SDL_Vulkan_CreateSurface(windowPtr, inst, nullptr, (VkSurfaceKHR*)&surface);

    VK::ContextManager::GetInstance().AddDevice(createDevice(vkboot_inst_, surface));

    VK::Device& device = VK::ContextManager::GetInstance().GetDevice(0);

    R::RenderContext& renderContext = R::RenderContext::GetInstance();

    renderContext.window = &windowPtr;

    VK::ContextManager::AddSwapchain(0, surface, 800, 600, renderContext.numFramesInFlight);

    renderContext.graphics_queue_ = device.GetGraphicsQueue();
    renderContext.compute_queue_  = device.GetComputeQueue();

    


    taskGraph_.AddNamedResource("SwapchainOutput", VK::ContextManager::GetSwapchain(0).GetSwapchainOutputHandle());
    taskGraph_.AddNamedResource("HDRFramebuffer", renderContext.HDRFramebuffer.GetResourceHandle());

    taskGraph_.AddTask<VK::StageBarrier>(vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    taskGraph_.AddTask<KE::R::UpdateBuffers>();
    taskGraph_.AddTask<VK::RenderTriangle>();
    taskGraph_.AddTask<R::TonemapAndPresent>();

    taskGraph_.Compile();

    
}

void KE::R::Renderer::Draw()
{
    VK::ContextManager::GetSwapchain(0).BeginNextFrame();

    vk::CommandBuffer cmd = VK::ContextManager::GetSwapchain(0).GetCurrentCommandBuffer();
    
    taskGraph_.Execute(cmd);
}

#include "Client.h"
#include "Engine/Entity/EntityFactory.h"
#include "Entities/TestDecorationEntity.h"
#include "Frame/Frame.h"

#include "Base/Time/LoopRateLimiter.h"

#include "../GpuInterface/Types/Swapchain.h"

#include "../GpuInterface/Functions/TransitionBarrier.h"

#include "../GpuInterface/Types/DeviceManager.h"

#include "../GpuInterface/Types/SwapchainOutput.h"
#include "../GpuInterface/Types/TaskGraph.h"
#include "../GpuInterface/Types/TaskNodes/StageBarrier.h"
#include "../GpuInterface/Types/TaskNodes/SubmitAndPresent.h"
#include "Renderer/Renderer.h"
#include "../Renderer/Types/TaskNodes/RenderTriangle.h"
#include "GpuInterface/Types/ResourceSet.h"
#include "GpuInterface/VkGpuInterface.h"
#include "Renderer/Types/EntityComponents/StaticMeshComponent.h"


VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;


namespace KE
{
int Client::Initialise()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("My SDL Window",  // Window title
                              800,              // Width
                              600,              // Height
                              SDL_WINDOW_VULKAN // Flags
    );

    if (!window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // SDL_SetWindowFullscreen(window, true);
    

    ((VkGpuInterface*)GpuInterface)->window_ = window;

    GpuInterface->Init();
    renderer.Init(window);

    // commandPool = ((VkGpuInterface*)GpuInterface)->deviceManager_.CreateGraphicsCommandPool();

    // vk::CommandBufferAllocateInfo allocInfo;
    // allocInfo.commandPool = commandPool;
    // allocInfo.level = vk::CommandBufferLevel::ePrimary;
    // allocInfo.commandBufferCount = 1;

    // commandBuffer =
    // vk::Device(((VkGpuInterface*)GpuInterface)->deviceManager_.GetDevice()).allocateCommandBuffers(allocInfo)[0];

    return 0;
}
void Client::Render()
{
}

f32 vertexInfo[] = {0.0, -1.0, 0.5, 1.0, 0.0, 0.0, 0.5, 0.0, -1.0, 1.0, 0.5, 0.0,
                    1.0, 0.0,  0.0, 1.0, 1.0, 1.0, 0.5, 0.0, 0.0,  1.0, 1.0, 1.0};


f32 vertexPos[] = {0.0f, -1.0f, 0.5f, -1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f};

f32 vertexAttribs[] = {1.0, 0.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};

int vertexIndices[] = {0, 1, 2};

void Client::Main()
{
    LoopRateLimiter fps_limiter(100.f);
    bool            isRunning = true;

    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(((VkGpuInterface*)GpuInterface)->vk_inst_));

    VK::ResourceSet swapchainOutputSet =
        VK::ResourceSet::Create<VK::SwapchainOutput>({VK::ResourceSetUsage::FRAMEALIGNED, 3});

    VK::TaskGraph taskGraph;

    


    VK::ResourceSet posBufferSet = VK::ResourceSet::Create<VK::Buffer>({}, 0, sizeof(vertexPos), vk::BufferUsageFlagBits::eVertexBuffer,
                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    VK::Buffer& posBuffer = posBufferSet.GetResourceHandle().GetResourceRef<VK::Buffer>();

    f32* mappedPosBuffer = (float*)posBuffer.map();
    memcpy(mappedPosBuffer, vertexPos, sizeof(vertexPos));
    posBuffer.unmap();


    taskGraph.AddTask<VK::StageBarrier>(vk::PipelineStageFlagBits2::eTopOfPipe,
                                        vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    taskGraph.AddTask<VK::RenderTriangle>(0, (VkGpuInterface*)GpuInterface);
    
    taskGraph.AddTask<VK::SubmitAndPresent>(0);


    taskGraph.Compile();

    entityWorld.AddWorldSystem<R::Renderer>();

    entityWorld.AddEntity("TestDecorationEntity");

    std::shared_ptr<Entity> testEnt = entityWorld.entities[0];


    std::shared_ptr<KE::R::StaticMeshComponent> testComp = testEnt->GetComponentPtr<KE::R::StaticMeshComponent>();

    
    testComp->mesh->submeshes_.resize(1);

    KE::R::Submesh &testSubmesh = testComp->mesh->submeshes_[0];

    testSubmesh.vertexAttributes_ = VK::Buffer(0, sizeof(vertexAttribs), vk::BufferUsageFlagBits::eVertexBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    f32* mappedAttribBuffer = (float*)testSubmesh.vertexAttributes_.map();
    memcpy(mappedAttribBuffer, vertexAttribs, sizeof(vertexAttribs));
    testSubmesh.vertexAttributes_.unmap();
    

    testSubmesh.binding_.stride = 20;
    testSubmesh.binding_.binding = 1;

    vk::VertexInputAttributeDescription2EXT attribDesc;
    attribDesc.binding = 0;
    attribDesc.location = 0;
    attribDesc.format = vk::Format::eR32G32B32Sfloat;
    testSubmesh.attributeDescs_.push_back(attribDesc);

    attribDesc.binding = 1;
    attribDesc.location = 1;
    attribDesc.offset = 0;
    attribDesc.format = vk::Format::eR32G32B32Sfloat;
    testSubmesh.attributeDescs_.push_back(attribDesc);

    attribDesc.offset = 12;
    attribDesc.location = 2;
    attribDesc.format = vk::Format::eR32G32Sfloat;

    testSubmesh.attributeDescs_.push_back(attribDesc);

    renderer.RegisterComponent(testEnt, testComp);


    while (isRunning)
    {
        fps_limiter.Sleep();
        Frame::RunClientFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                isRunning = false;
            }
        }

        renderer.Draw();
    }
}
} // namespace KE
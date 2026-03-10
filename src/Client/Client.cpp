#include "Client.h"
#include "Frame/Frame.h"

#include "Base/Time/LoopRateLimiter.h"

#include "../GpuInterface/Types/Swapchain.h"

#include "../GpuInterface/Functions/TransitionBarrier.h"

#include "../GpuInterface/Types/DeviceManager.h"

#include "../GpuInterface/Types/TaskGraph.h"
#include "../GpuInterface/Types/TaskNodes/RenderTriangle.h"
#include "../GpuInterface/Types/TaskNodes/SubmitAndPresent.h"
#include "../GpuInterface/Types/TaskNodes/StageBarrier.h"   
#include "../GpuInterface/Types/SwapchainOutput.h"
#include "GpuInterface/Types/ResourceSet.h"
#include "GpuInterface/VkGpuInterface.h"

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

    ((VkGpuInterface *)GpuInterface)->window_ = window;

    GpuInterface->Init();


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

void Client::Main()
{
    LoopRateLimiter fps_limiter(100.f);
    bool            isRunning = true;

    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(((VkGpuInterface *)GpuInterface)->vk_inst_));


    VK::ResourceSet swapchainOutputSet = VK::ResourceSet::Create<VK::SwapchainOutput>(VK::ResourceSetUsage::FRAMEALIGNED, 3);

    VK::TaskGraph taskGraph;

    taskGraph.AddNamedResource("SwapchainOutput", VK::ContextManager::GetSwapchain(0).GetSwapchainOutputHandle());
    
    taskGraph.AddTask<VK::StageBarrier>(vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    taskGraph.AddTask<VK::RenderTriangle>(0, (VkGpuInterface*)GpuInterface);
    taskGraph.AddTask<VK::SubmitAndPresent>(0);



    taskGraph.Compile();

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

        VK::ContextManager::GetSwapchain(0).BeginNextFrame();

        vk::CommandBuffer          cmd = VK::ContextManager::GetSwapchain(0).GetCurrentCommandBuffer();

        taskGraph.Execute(cmd);

    }
}
} // namespace KE
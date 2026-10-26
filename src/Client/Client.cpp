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
#include "Time/Clocks.h"


VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;


namespace KE
{
int Client::Initialise()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) //SDL3 returns bool, not an int error code
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
    
    renderer.Init(window);

    return 0;
}
void Client::Render()
{
}

void Client::Main()
{
    LoopRateLimiter fps_limiter(100.f);
    bool            isRunning = true;

    //GpuInterface is never Init()'d anymore (Renderer owns the instance), so vk_inst_ is null and this nulls every
    //instance/device function. On Linux the dispatcher copies of all .so's get merged, so it wiped Renderer's too.
    //VULKAN_HPP_DEFAULT_DISPATCHER.init();
    //VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(((VkGpuInterface*)GpuInterface)->vk_inst_));

    VK::TaskGraph taskGraph;

    entityWorld.AddWorldSystem<R::Renderer>();

    

    std::shared_ptr<Entity> testEnt2 = entityWorld.AddEntity("TestDecorationEntity");

    std::shared_ptr<KE::R::StaticMeshComponent> testComp2 = testEnt2->GetComponentPtr<KE::R::StaticMeshComponent>();

    testComp2->meshPath = "../../../../../Engine/assets/error.gltf";

    renderer.RegisterComponent(testEnt2, testComp2);

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

        testEnt2.get()->transform_.position_ = glm::vec3(0, 0, sin(float(GlobalClock::QueryGlobalTime())));

        renderer.Draw();
    }
}
} // namespace KE
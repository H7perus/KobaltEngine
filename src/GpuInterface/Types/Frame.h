#pragma once 

#include "vulkan/vulkan.hpp"

namespace KE::VK
{
    //This class represents the Vulkan objects that belong to one frame-in-flight. The number is equal to the swapchain images
    class Frame
    {
    public:
        void Begin();
        void End();

    private:
        //Command pool, required per frame, because we want to reset the pool after submission
        vk::CommandPool commandPool_;
        //Self explanatory. We could also write into multiple buffers, might become necessary for certain applications
        vk::CommandBuffer commandBuffer_;

        vk::Semaphore imageAvailableSemaphore_;
        vk::Semaphore renderFinishedSemaphore_;
        vk::Fence inFlightFence_;
    };
}
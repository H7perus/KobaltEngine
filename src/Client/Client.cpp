#pragma once
#include "Client.h"
#include "Frame/Frame.h"

#include "Base/Time/LoopRateLimiter.h"

#include "../GpuInterface/Types/Swapchain.h"
#include "../GpuInterface/Functions/TransitionBarrier.h"
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
    LoopRateLimiter fps_limiter(0.f);
    bool            isRunning = true;

    VULKAN_HPP_DEFAULT_DISPATCHER.init();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(((VkGpuInterface *)GpuInterface)->vk_inst_));

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

        ((VkGpuInterface *)GpuInterface)->swapchain_.BeginNextFrame();

        vk::CommandBuffer          cmd = ((VkGpuInterface *)GpuInterface)->swapchain_.GetCurrentCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo{};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		cmd.reset();
        cmd.begin(beginInfo);

        transitionBarrier(cmd, ((VkGpuInterface *)GpuInterface)->swapchain_.GetCurrentImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

        vk::RenderingAttachmentInfo colorAttachment{};
        colorAttachment.imageView   = ((VkGpuInterface *)GpuInterface)->swapchain_.GetCurrentImageView();
        colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachment.loadOp      = vk::AttachmentLoadOp::eClear;
        colorAttachment.storeOp     = vk::AttachmentStoreOp::eStore;
        colorAttachment.clearValue  = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};

        vk::RenderingInfo renderingInfo{};
        renderingInfo.renderArea           = vk::Rect2D{{0, 0}, ((VkGpuInterface *)GpuInterface)->swapchain_.extent_};
        renderingInfo.layerCount           = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachment;


        cmd.beginRendering(renderingInfo);

        vk::Viewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = (float)((VkGpuInterface *)GpuInterface)->swapchain_.extent_.width;
        viewport.height   = (float)((VkGpuInterface *)GpuInterface)->swapchain_.extent_.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        cmd.setViewport(0, viewport);

        vk::Rect2D scissor{{0, 0}, ((VkGpuInterface *)GpuInterface)->swapchain_.extent_};
        cmd.setScissor(0, scissor);

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
                         ((VkGpuInterface *)GpuInterface)->testPipeline.GetPipeline());
        vk::DeviceSize offset = 0;
        cmd.bindVertexBuffers(0, 1, &((VkGpuInterface *)GpuInterface)->vertexBuffer.buffer, &offset);
        cmd.draw(3, 1, 0, 0);


        cmd.endRendering();
        transitionBarrier(cmd, ((VkGpuInterface *)GpuInterface)->swapchain_.GetCurrentImage(), vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
        cmd.end();
        ((VkGpuInterface *)GpuInterface)->swapchain_.EndFrame();
    }
}
} // namespace KE
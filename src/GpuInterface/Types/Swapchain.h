#pragma once

#include "Device.h"
#include "Frame.h"

#include "vulkan/vulkan.hpp"
#include "VkBootstrap.h"

#include <vector>

#include "RendererDLL.h"

#include "BasicTypeAliases.h"

namespace KE::VK
{
    class GPUI_DLL_API Swapchain
    {
      public:
        vk::SwapchainKHR swapchain_;
        vk::Format       format_;
        vk::Extent2D     extent_;
        vk::SurfaceKHR   surface_;
        vkb::Swapchain   vkbSwapchain_;
        KE::VK::Device  *device_;
        vk::Queue        graphicsQueue_;
        // refers to Frame In Flight!
        u32                        currentFrame_;
        u32                        currentImage_;
        std::vector<Frame>         frames_;
        std::vector<vk::Image>     images_;
        std::vector<vk::ImageView> imageViews_;

      public:
        void Init(KE::VK::Device &device, vk::SurfaceKHR surface, uint32_t width, uint32_t height);

        Frame &BeginNextFrame();

        vk::Image GetCurrentImage()
        {
            return images_[currentImage_];
        }
        vk::ImageView GetCurrentImageView()
        {
            return imageViews_[currentImage_];
        }

        vk::CommandBuffer GetCurrentCommandBuffer()
        {
            return frames_[currentFrame_].commandBuffer_;
        }

        void EndFrame();

        void BuildSwapchain(u32 width, u32 height);

        void Recreate(u32 width, u32 height);

        void Destroy();
    };
}
#pragma once

#include "Device.h"

#include "vulkan/vulkan.hpp"
#include "VkBootstrap.h"

#include <vector>

#include "RendererDLL.h"

namespace KE::VK
{
    GPUI_DLL_API class Swapchain
    {
        public:
        vk::SwapchainKHR swapchain_;
        vk::Format format_;
        vk::Extent2D extent_;
        vk::SurfaceKHR surface_;
        vkb::Swapchain vkbSwapchain_;
        KE::VK::Device *device_;

        std::vector<vk::Image> images_;
        std::vector<vk::ImageView> imageViews_;
    public:
        void Init(KE::VK::Device &device, vk::SurfaceKHR surface, uint32_t width, uint32_t height)
        {
            device_ = &device;
            surface_ = surface;
            BuildSwapchain(width, height);
        }

        void BuildSwapchain(uint32_t width, uint32_t height)
        {
            vkb::SwapchainBuilder builder{vkb::Device(*device_)};

            auto result = builder
                              .set_desired_extent(width, height)
                              .set_desired_format({VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                              .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                              .set_old_swapchain(vkbSwapchain_)
                              .build();

            if (!result)
            {
                // handle error
                printf("Swapchain build failed: %s\n", result.error().message().c_str());
                return;
            }

            vkbSwapchain_ = result.value();

            swapchain_ = (vk::SwapchainKHR)(vkbSwapchain_.swapchain);
            format_ = vk::Format(vkbSwapchain_.image_format);
            extent_ = vk::Extent2D{vkbSwapchain_.extent.width, vkbSwapchain_.extent.height};

            // grab images
            auto images = vkbSwapchain_.get_images().value();
            images_.assign(images.begin(), images.end());

            // grab image views
            auto imageViews = vkbSwapchain_.get_image_views().value();
            imageViews_.assign(imageViews.begin(), imageViews.end());
        }

        void Recreate(uint32_t width, uint32_t height)
        {
            // destroy old image views before rebuilding
            Destroy();
            BuildSwapchain(width, height);
        }

        void Destroy()
        {
            vk::Device device = (vk::Device)(*device_);

            for (auto &iv : imageViews_)
                device.destroyImageView(iv);

            imageViews_.clear();
            images_.clear();

            device.destroySwapchainKHR(swapchain_);
        }
    };
}
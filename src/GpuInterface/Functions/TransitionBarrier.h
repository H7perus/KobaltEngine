#pragma once

#include "../Types/SwapchainOutput.h"
#include "vulkan/vulkan.hpp"
#include "RendererDLL.h"

GPUI_DLL_API void transitionBarrier(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);


GPUI_DLL_API void transitionBarrier(vk::CommandBuffer cmd, KE::VK::SwapchainOutput swapchainOutput, vk::ImageLayout newLayout);
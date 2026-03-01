#pragma once
#include "vulkan/vulkan.hpp"
#include "RendererDLL.h"

GPUI_DLL_API void transitionBarrier(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
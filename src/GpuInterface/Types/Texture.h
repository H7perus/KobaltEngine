#pragma once

#include "Device.h"
#include "Resource.h"


#include "vulkan/vulkan.hpp"


namespace KE::VK
{
class Texture : public IResource
{
    KE_REFLECT(Texture)

    // Texture properties and Vulkan handles would go here
    vk::Image                  image_;
    vk::ImageView              imageView_;
    vk::DeviceMemory           memory_;
    vk::ImageDescriptorInfoEXT imageDescriptorInfo_;
    vk::ImageCreateInfo        imageInfo_;
    vk::ImageViewCreateInfo    viewInfo_;
    // Index of the device
    u32 deviceIndex_;
    // Index of the resource in the resource heap
    u32 resourceHeapIndex_;


  public:
    Texture() = default;

    Texture(u32 deviceIndex, uint32_t width, uint32_t height, vk::Format format, vk::ImageUsageFlags usage,
            vk::MemoryPropertyFlags properties, bool sendToHeap = false);
    ~Texture() = default;


    void UploadPixels(const void* pixelData);

    // Methods for creating, managing, and destroying the texture would go here
};

} // namespace KE::VK
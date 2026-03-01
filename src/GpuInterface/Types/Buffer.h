#pragma once

#include "Device.h"

#include "VkBootstrap.h"
#include "memHelper.h"
#include <iostream>
#include <vulkan/vulkan.hpp>


#include "BasicTypeAliases.h"


namespace KE::VK
{
struct Buffer
{
    vk::Buffer           buffer     = VK_NULL_HANDLE;
    vk::DeviceMemory     memory     = VK_NULL_HANDLE;
    bool                 isMappable = false;
    vk::BufferCreateInfo bufferCreateInfo{};

  protected:
    const KE::VK::Device     *device_ = nullptr;
    vk::DeviceAddressRangeEXT addressRange_;

  public:
    ~Buffer();

    Buffer() = default;
    Buffer(KE::VK::Device const &device_in, vk::DeviceSize size, vk::BufferUsageFlags usage,
           vk::MemoryPropertyFlags properties);

    Buffer(Buffer &&other);

    Buffer &operator=(Buffer &&other) noexcept;

    vk::DescriptorBufferInfo GetDescriptorBufferInfo(u32 offset, u64 range);

  protected:
    vk::DeviceAddress GetBufferAddress();

  public:
    vk::DeviceAddressRangeEXT *GetBufferAddressRangePtr()
    {
        return &addressRange_;
    }

    void *map()
    {
        if (!isMappable)
        {
            std::cerr << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT was not requested for this piece of memory";
            std::terminate();
        }
        void *data;
        data = ((vk::Device)(*device_)).mapMemory(memory, 0, bufferCreateInfo.size, vk::MemoryMapFlags(0));

        return data;
    }
    void unmap()
    {
        ((vk::Device)(*device_)).unmapMemory(memory);
    }
    void destroy()
    {
        if (buffer != VK_NULL_HANDLE)
        {
            ((vk::Device)(*device_)).destroyBuffer(buffer, nullptr);
            if (memory == VK_NULL_HANDLE)
                throw("literally HOW?");
            ((vk::Device)(*device_)).freeMemory(memory, nullptr);

            buffer = VK_NULL_HANDLE;
        }
    }
};
} // namespace KE::VK

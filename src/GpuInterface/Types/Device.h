#pragma once

// #include "../VkHelpers.h"

#include "VkBootstrap.h"
#include "vulkan/vulkan.hpp"


#include <iostream>
#include <map>
#include <optional>

// TODO: Think about where to store named buffer references etc. Its a bit of an annoyance.


#include "BasicTypeAliases.h"

using std::shared_ptr;

namespace KE::VK
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value() && graphicsFamily.has_value();
    }
};

class Device
{
    friend class DeviceManager;
    // reserved ranges and element size for sampler and resource heaps

  protected:
    vk::Device                                    device_;
    vk::PhysicalDevice                            physicalDevice_;
    vkb::Device                                   vkboot_device_;
    vk::PhysicalDeviceDescriptorHeapPropertiesEXT heapProperties_;
    QueueFamilyIndices                            queueFamilyIndices_;

  public:
    Device() {};

    Device(vkb::Device vkboot_device)
    {
        device_         = vk::Device(vkboot_device.device);
        physicalDevice_ = vk::PhysicalDevice(vkboot_device.physical_device);
        vkboot_device_  = vkboot_device;

        vk::PhysicalDeviceProperties2 props2;
        props2.pNext = &heapProperties_;

        physicalDevice_.getProperties2(&props2);

        queueFamilyIndices_ = FindQueueFamilies();
    }

    vk::Device GetVkDevice()
    {
        return device_;
    }

	
    u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties) const
    {

        vk::PhysicalDevice                 physicalDevice = vk::PhysicalDevice(physicalDevice_);
        vk::PhysicalDeviceMemoryProperties memProperties;

        physicalDevice.getMemoryProperties(&memProperties);

        for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }


    vk::Queue GetGraphicsQueue() const
    {
        auto graphics_queue_ret = device_.getQueue(queueFamilyIndices_.graphicsFamily.value(), 0);
        if (!graphics_queue_ret)
        { /* report */
        }
        return graphics_queue_ret;
    }


    vk::CommandPool CreateGraphicsCommandPool()
    {
        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.flags            = vk::CommandPoolCreateFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        poolInfo.queueFamilyIndex = queueFamilyIndices_.graphicsFamily.value();

        vk::CommandPool pool;

        try
        {
            pool = device_.createCommandPool(poolInfo, nullptr);
        }
        catch (const vk::SystemError &err)
        {
            std::cerr << "Failed to create command pool: " << err.what() << std::endl;
        }

        return pool;
    }

    i64 GetResourceHeapReservedRange() const
    {
        return heapProperties_.minResourceHeapReservedRange;
    }

    i64 GetResourceDescriptorSize() const
    {
        return heapProperties_.bufferDescriptorSize;
    }

    i64 GetSamplerHeapReservedRange() const
    {
        return heapProperties_.minSamplerHeapReservedRange;
    }

    i64 GetSamplerDescriptorSize() const
    {
        return heapProperties_.samplerDescriptorSize;
    }


    operator vk::Device() const
    {
        return device_;
    }

    operator vk::Device()
    {
        return device_;
    }

    operator vk::PhysicalDevice() const
    {
        return physicalDevice_;
    }

    operator vk::PhysicalDevice()
    {
        return physicalDevice_;
    }

    operator vkb::Device() const
    {
        return vkboot_device_;
    }

  private:
    QueueFamilyIndices FindQueueFamilies()
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        physicalDevice_.getQueueFamilyProperties(&queueFamilyCount, nullptr);

        std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
        physicalDevice_.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            // TODO: Present family must also be supported.

            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                indices.graphicsFamily = i;
            }

            if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
            {
                indices.computeFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }
};

} // namespace KE::VK
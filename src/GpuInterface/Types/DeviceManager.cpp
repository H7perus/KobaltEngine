#include "DeviceManager.h"
using namespace KE::VK;

DeviceManager::DeviceManager(Device device) : device_(device)
{
    ResourceHeap_ = DescriptorHeapBuffer(device_, 2048, ResourceHeap);
    SamplerHeap_ = DescriptorHeapBuffer(device_, 2048, SamplerHeap);
}


Device& DeviceManager::GetDevice()
{
    return device_;
}

vk::Queue DeviceManager::GetGraphicsQueue() const
{
    auto graphics_queue_ret = ((vkb::Device)device_).get_queue(vkb::QueueType::graphics);
    if (!graphics_queue_ret)
    { /* report */
    }
    return vk::Queue(graphics_queue_ret.value());
}

vk::Queue DeviceManager::GetComputeQueue() const
{
    auto compute_queue_ret = ((vkb::Device)device_).get_queue(vkb::QueueType::compute);
    if (!compute_queue_ret)
    { /* report */
    }
    return vk::Queue(compute_queue_ret.value());
}

vk::CommandPool DeviceManager::CreateComputeCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = device_.queueFamilyIndices_;

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();

    vk::CommandPool pool;

    try
    {
        pool = device_.device_.createCommandPool(poolInfo, nullptr);
    }
    catch (const vk::SystemError &err)
    {
        std::cerr << "Failed to create command pool: " << err.what() << std::endl;
    }

    return pool;
}

vk::CommandPool DeviceManager::CreateGraphicsCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = device_.queueFamilyIndices_;

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    vk::CommandPool pool;

    try
    {
        pool = device_.device_.createCommandPool(poolInfo, nullptr);
    }
    catch (const vk::SystemError &err)
    {
        std::cerr << "Failed to create command pool: " << err.what() << std::endl;
    }

    return pool;
}

shared_ptr<KE::VK::BufferSet> DeviceManager::RequestNamedBufferSet(std::string name, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
    auto ret = namedBuffers_.try_emplace(name, std::make_shared<KE::VK::BufferSet>(device_, size, usage, properties));
    if (ret.second)
    {
        return ret.first->second;
    }
}

shared_ptr<KE::VK::BufferSet> DeviceManager::GetBufferSetByName(std::string name)
{
    auto ret = *namedBuffers_.find(name);
    if (ret.second)
    {
        return ret.second;
    }
}
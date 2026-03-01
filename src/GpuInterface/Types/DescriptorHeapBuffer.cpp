#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "DescriptorHeapBuffer.h"

using namespace KE::VK;

DescriptorHeapBuffer::DescriptorHeapBuffer(const KE::VK::Device &device_in, i32 descriptorCount, DescriptorHeapType heapType)
    : Buffer(device_in, calculateSize(device_in, descriptorCount, heapType), vk::BufferUsageFlagBits::eDescriptorHeapEXT | vk::BufferUsageFlagBits::eShaderDeviceAddress, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
{

    if (heapType == ResourceHeap)
    {
        descriptorSize_ = device_in.GetResourceDescriptorSize();
        lowestIndex_ = device_in.GetResourceHeapReservedRange() / device_in.GetResourceDescriptorSize();
    }
    else if (heapType == SamplerHeap)
    {
        descriptorSize_ = device_in.GetSamplerDescriptorSize();
        lowestIndex_ = device_in.GetSamplerHeapReservedRange() / device_in.GetSamplerDescriptorSize();
    }
    else
    {
        throw new std::exception("There are only two types of descriptor heaps and you doofus somehow managed to fuck it up");
    }

    previouslyUsedIndexCount_ = lowestIndex_;
    maxDescriptorCount_ = descriptorCount;

    bindInfo_.heapRange.address = ((vk::Device)(*device_)).getBufferAddress(buffer);
    bindInfo_.heapRange.size = bufferCreateInfo.size;
    bindInfo_.reservedRangeOffset = 0;
    bindInfo_.reservedRangeSize = lowestIndex_ * descriptorSize_;
}

i32 DescriptorHeapBuffer::EnterDescriptor(vk::ResourceDescriptorInfoEXT *descriptor)
{
    i32 index = 0;
    if (freedIndices_.size() == 0)
        index = previouslyUsedIndexCount_;
    else
    {
        index = freedIndices_[freedIndices_.size() - 1];
        freedIndices_.pop_back();
    }

    vk::HostAddressRangeEXT dest;
    dest.address = (char *)map() + index * descriptorSize_;
    dest.size = descriptorSize_;

    ((vk::Device)(*device_)).writeResourceDescriptorsEXT(1, descriptor, &dest);

    unmap();
    return index;
}

vk::BindHeapInfoEXT *DescriptorHeapBuffer::GetBindInfo()
{
    return &bindInfo_;
}

i64 DescriptorHeapBuffer::calculateSize(const KE::VK::Device &device_in, i64 count, DescriptorHeapType type)
{
    if (type == ResourceHeap)
        return device_in.GetResourceHeapReservedRange() + count * device_in.GetResourceDescriptorSize();
    else
        return device_in.GetSamplerHeapReservedRange() + count * device_in.GetSamplerDescriptorSize();
}
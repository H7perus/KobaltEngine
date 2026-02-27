#pragma once
#include "vulkan/vulkan.hpp"
#include "Device.h"
#include "Buffer.h"
#include "BasicTypeAliases.h"



namespace KE::VK
{
	enum DescriptorHeapType
	{
		SamplerHeap,
		ResourceHeap
	};



	class DescriptorHeapBuffer : public Buffer
	{
		//lowest index: heap reserved range divided by descriptor size
		i32 lowestIndex_ = 0;
		//the limit. We can only have that many resources referenced by this heap
		i32 maxDescriptorCount_ = 0;

		i32 descriptorSize_ = 0;

		//counts up when a new slot in the heap is used. If an old one is reused, it stays the same.
		//value includes the reserved range
		i32 previouslyUsedIndexCount_ = 0;
		//We want to reuse free descriptor slots. Slot is freed = goes into the list, is reused and the element popped.
		std::vector<i32> freedIndices_;

		vk::BindHeapInfoEXT bindInfo_;

	public:
		DescriptorHeapBuffer( const KE::VK::Device&device_in, i32 descriptorCount, DescriptorHeapType heapType)
			: Buffer(device_in, calculateSize(device_in, descriptorCount, heapType), vk::BufferUsageFlagBits::eDescriptorHeapEXT
				| vk::BufferUsageFlagBits::eShaderDeviceAddress, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
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

		i32 EnterDescriptor(vk::ResourceDescriptorInfoEXT* descriptor)
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
			dest.address = (char*)map() + index * descriptorSize_;
			dest.size = descriptorSize_;

			((vk::Device)(*device_)).writeResourceDescriptorsEXT(1, descriptor, &dest);
			
			unmap();
			return index;
		}

		vk::BindHeapInfoEXT* GetBindInfo()
		{
			return &bindInfo_;
		}

	private:
		static i64 calculateSize(const KE::VK::Device& device_in, i64 count, DescriptorHeapType type)
		{
			if (type == ResourceHeap)
				return device_in.GetResourceHeapReservedRange() + count * device_in.GetResourceDescriptorSize();
			if (type == SamplerHeap)
				return device_in.GetSamplerHeapReservedRange() + count * device_in.GetSamplerDescriptorSize();
		}
		//static v
	};
}



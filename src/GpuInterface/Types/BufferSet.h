#pragma once

#include "Buffer.h"

#include "BasicTypeAliases.h"
// Purpose: Enable references to flipping buffers and frames-in-flight-aligned buffers (i.e. uniform buffers) in an abstract way. 
// Reflection info is then used to find the appropriate set and not a specific buffer.

namespace KE::VK
{
	enum BufferSetUsage
	{
		// A single buffer
		STATIC,
		// Two buffers, flip-flopping back and forth ( might not even need logic on the CPU side. We could just decide in shader(?) )
		FLIP,
		// Aligned to Frames-In-Flight. Used for things like uniform buffers.
		FRAMEALIGNED
	};


	//TODO: Is templatisation + pointers faster? We are deciding on behavior at runtime rn
	class BufferSet
	{
		std::vector<std::shared_ptr<Buffer>> buffers_;
		//This is a constant memory address, meaning that we can just point a vk::DescriptorWriteSet at it
		vk::Buffer currentBuffer_;

		BufferSetUsage usage_ = STATIC;
		u8 curr_index_ = 0;

	public:
		BufferSet(const vkb::Device& device_in, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
		{
			buffers_.emplace_back(std::make_shared<Buffer>(device_in, size, usage, properties));
		}
		BufferSet(const vkb::Device& device_in, BufferSetUsage setUsage, u32 bufferCount, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) : usage_(setUsage)
		{
			for(int i = 0; i < bufferCount; i++)
				buffers_.emplace_back(std::make_shared<Buffer>(device_in, size, usage, properties));
		}

		//we will just call this one on every frame in the renderer. A pure compute system might either just not touch this or have its own rules. Frame aligned doesn't apply then.
		void Advance(u8 frameIndex)
		{
			if (usage_ == FRAMEALIGNED)
				curr_index_ = frameIndex;
			else if (usage_ == FLIP)
				curr_index_ = (curr_index_ + 1) % 2;
			//crucial! This is some trickery, so we only have to advance the buffer and can use a static pointer pointing at currentBuffer_
			currentBuffer_ = buffers_[curr_index_]->buffer;
		}

		vk::Buffer* GetStaticPointerToCurrentBuffer()
		{
			return &currentBuffer_;
		}

		//we might not need this
		std::shared_ptr<Buffer> GetCurrentBuffer()
		{
			return buffers_[curr_index_];
		}
	};
}




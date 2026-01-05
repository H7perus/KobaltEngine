#pragma once
#include <vulkan/vulkan.hpp>
#include "memHelper.h"

#include "Device.h"


namespace KE::VK {
	struct Buffer
	{
		friend class Device;
		vk::Buffer buffer = VK_NULL_HANDLE;
		vk::DeviceMemory memory = VK_NULL_HANDLE;
		bool isMappable = false;
		vk::BufferCreateInfo bufferInfo{};

	private:
		KE::VK::Device* device;
	public:
		~Buffer()
		{
			if (buffer != VK_NULL_HANDLE)
				destroy();
		}
		Buffer() {}
		Buffer(KE::VK::Device* kedevice, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
			
			device = kedevice;

			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = vk::SharingMode::eExclusive;

			if (device->device_.createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to create buffer!");
			}

			vk::MemoryRequirements memRequirements;
			device->device_.getBufferMemoryRequirements(buffer, &memRequirements);

			vk::MemoryAllocateInfo allocInfo{};
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(*device, memRequirements.memoryTypeBits, vk::MemoryPropertyFlags(properties));

			if (device->device_.allocateMemory(&allocInfo, nullptr, &memory) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}
			if ((properties & vk::MemoryPropertyFlagBits::eHostVisible))
				isMappable = true;

			device->device_.bindBufferMemory(buffer, memory, 0);
		}

		Buffer(Buffer&& other) : buffer(other.buffer), memory(other.memory), isMappable(other.isMappable), bufferInfo(other.bufferInfo)
		{
			other.buffer = VK_NULL_HANDLE;
		}

		Buffer& operator= (Buffer&& other) noexcept
		{
			if (this != &other)
			{
				// Release any resources currently held by this object
				if (buffer)
				{
					destroy();
				}

				// Transfer ownership of the resources of the new object
				buffer = other.buffer;
				memory = other.memory;
				bufferInfo = other.bufferInfo;
				isMappable = other.isMappable;

				other.buffer = VK_NULL_HANDLE;
				other.memory = VK_NULL_HANDLE;
			}
			return *this;
		}
		void* map()
		{
			if (!isMappable)
			{
				std::cerr << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT was not requested for this piece of memory";
				std::terminate();
			}
			void* data;
			data = device->device_.mapMemory(memory, 0, bufferInfo.size, vk::MemoryMapFlags(0));

			return data;
		}
		void unmap()
		{
			device->device_.unmapMemory(memory);
		}
		void destroy()
		{
			if (buffer != VK_NULL_HANDLE)
			{
				device->device_.destroyBuffer(buffer, nullptr);
				if (memory == VK_NULL_HANDLE)
					throw("literally HOW?");

				device->device_.freeMemory(memory, nullptr);

				buffer = VK_NULL_HANDLE;
			}

		}
	};
}


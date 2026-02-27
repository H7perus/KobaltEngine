#pragma once

#include "Device.h"

#include <vulkan/vulkan.hpp>
#include "memHelper.h"
#include <iostream>
#include "VkBootstrap.h"

#include "BasicTypeAliases.h"



namespace KE::VK {
	struct Buffer
	{
		vk::Buffer buffer = VK_NULL_HANDLE;
		vk::DeviceMemory memory = VK_NULL_HANDLE;
		bool isMappable = false;
		vk::BufferCreateInfo bufferCreateInfo{};
	protected:
		const KE::VK::Device* device_ = nullptr;
		vk::DeviceAddressRangeEXT addressRange_;
	public:
		~Buffer()
		{
			if (buffer != VK_NULL_HANDLE)
				destroy();
		}
		Buffer() {}
		Buffer(KE::VK::Device const&  device_in, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) : device_(&device_in) {

			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = usage | vk::BufferUsageFlagBits::eShaderDeviceAddress;
			bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;




			if (((vk::Device)(*device_)).createBuffer(&bufferCreateInfo, nullptr, &buffer) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to create buffer!");
			}

			vk::MemoryRequirements memRequirements;
			((vk::Device)(*device_)).getBufferMemoryRequirements(buffer, &memRequirements);

			vk::MemoryAllocateInfo allocInfo{};

			vk::MemoryAllocateFlagsInfo flagsInfo;
			flagsInfo.flags = vk::MemoryAllocateFlagBits::eDeviceAddress;

			if (bufferCreateInfo.usage & vk::BufferUsageFlagBits::eShaderDeviceAddress)
			{

				allocInfo.pNext = &flagsInfo;
			}
				

			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = device_->FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlags(properties));

			if (((vk::Device)(*device_)).allocateMemory(&allocInfo, nullptr, &memory) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to allocate buffer memory!");
			}
			if ((properties & vk::MemoryPropertyFlagBits::eHostVisible))
				isMappable = true;

			((vk::Device)(*device_)).bindBufferMemory(buffer, memory, 0);


			addressRange_.address = GetBufferAddress();
			addressRange_.size = bufferCreateInfo.size;
		}

		Buffer(Buffer&& other) : buffer(other.buffer), memory(other.memory), isMappable(other.isMappable), bufferCreateInfo(other.bufferCreateInfo), device_(other.device_), addressRange_(other.addressRange_)
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
				device_ = other.device_;
				addressRange_ = other.addressRange_;
				buffer = other.buffer;
				memory = other.memory;
				bufferCreateInfo = other.bufferCreateInfo;
				isMappable = other.isMappable;
				other.buffer = VK_NULL_HANDLE;
				other.memory = VK_NULL_HANDLE;
			}
			return *this;
		}

		vk::DescriptorBufferInfo GetDescriptorBufferInfo(u32 offset, u64 range)
		{
			return vk::DescriptorBufferInfo(buffer, offset, range);
		}
	protected:
		vk::DeviceAddress GetBufferAddress()
		{
			return ((vk::Device)(*device_)).getBufferAddress(buffer);
		}
	public:
		vk::DeviceAddressRangeEXT* GetBufferAddressRangePtr()
		{
			return &addressRange_;
		}

		void* map()
		{
			if (!isMappable)
			{
				std::cerr << "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT was not requested for this piece of memory";
				std::terminate();
			}
			void* data;
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
}


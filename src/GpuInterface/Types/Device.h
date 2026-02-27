#pragma once


#include "vulkan/vulkan.hpp"
#include "VkBootstrap.h"

#include <map>
//TODO: Think about where to store named buffer references etc. Its a bit of an annoyance.


#include "BasicTypeAliases.h"

using std::shared_ptr;

namespace KE::VK
{
	

	class Device
	{
		friend class DeviceManager;
		//reserved ranges and element size for sampler and resource heaps
		vk::PhysicalDeviceDescriptorHeapPropertiesEXT heapProperties_;

		


	public:
		Device() {};

		Device(vkb::Device vkboot_device)
		{
			device_ = vk::Device(vkboot_device.device);
			physicalDevice_ = vk::PhysicalDevice(vkboot_device.physical_device);
			vkboot_device_ = vkboot_device;

			vk::PhysicalDeviceProperties2 props2;
			props2.pNext = &heapProperties_;

			physicalDevice_.getProperties2(&props2);
		}

		u32 FindMemoryType(u32 typeFilter, vk::MemoryPropertyFlags properties) const
		{

			vk::PhysicalDevice physicalDevice = vk::PhysicalDevice(physicalDevice_);
			vk::PhysicalDeviceMemoryProperties memProperties;

			physicalDevice.getMemoryProperties(&memProperties);

			for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
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

	protected:
		vk::Device device_;
		vk::PhysicalDevice physicalDevice_;
		vkb::Device vkboot_device_;
		
	};
	
}
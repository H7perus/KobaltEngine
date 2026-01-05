#pragma once


#include "vulkan/vulkan.hpp"
//#include "Buffer.h"


namespace KE::VK
{
	class Device
	{
		
	public:
		friend struct Buffer;

		Device() {};

		Device(vkb::Device vkboot_device)
		{
			device_ = vk::Device(vkboot_device.device);
			physical_device_ = vk::PhysicalDevice(vkboot_device.physical_device);
			vkboot_device_ = vkboot_device;
		}

		operator vk::Device() const
		{
			return device_;
		}

		operator vk::PhysicalDevice() const
		{
			return physical_device_;
		}

		operator vkb::Device() const
		{
			return vkboot_device_;
		}

	private:
		vk::Device device_;
		vk::PhysicalDevice physical_device_;
		vkb::Device vkboot_device_;
		
	};
	
}
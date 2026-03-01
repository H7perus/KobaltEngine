#pragma once
#include "../VkHelpers.h"
#include "Device.h"
#include "BufferSet.h"
#include "Buffer.h"
#include "DescriptorHeapBuffer.h"

#include "RendererDLL.h"

namespace KE { class VkGpuInterface; }

namespace KE::VK
{
	class GPUI_DLL_API DeviceManager
	{
		friend class KE::VkGpuInterface;
	protected:
		Device device_;
		std::map<std::string, shared_ptr<KE::VK::BufferSet>> namedBuffers_;

		DescriptorHeapBuffer ResourceHeap_ = DescriptorHeapBuffer();
		DescriptorHeapBuffer SamplerHeap_ = DescriptorHeapBuffer();
	public:

		DeviceManager() = default;


		DeviceManager(Device device);


		Device& GetDevice();


		vk::Queue GetGraphicsQueue() const;


		vk::Queue GetComputeQueue() const;


		vk::CommandPool CreateComputeCommandPool();

		
		vk::CommandPool CreateGraphicsCommandPool();


		shared_ptr<KE::VK::BufferSet> RequestNamedBufferSet(std::string name, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);


		shared_ptr<KE::VK::BufferSet> GetBufferSetByName(std::string name);

	};
}

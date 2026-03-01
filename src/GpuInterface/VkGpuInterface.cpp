#pragma once
#include "VkGpuInterface.h"
#include "SDL3/SDL_vulkan.h"
#include "Types/Swapchain.h"
#include "VkHelpers.h"

#include "glm/glm.hpp"

#include "Types/Buffer.h"
#include "Types/DescriptorHeapBuffer.h"
#include "Types/PipelineGraphics.h"
#include "Types/PipelineCompute.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;


#include "ShaderCompile/ShaderCompile.h"
#include "ShaderCompile/SlangCompileContext.h"
#include "ShaderCompile/SlangCompiledUnit.h"

#include "ShaderCompile/GlslShaderCompile.h"

#include <string>


f32 vertexInfo[] = {0.0, -1.0, 0.5,  1.0, 0.0, 0.0, 0.5, 0.0, -1.0, 1.0, 0.5, 0.0,
                    1.0, 0.0,  -1.0, 1.0, 1.0, 1.0, 0.5, 0.0, 0.0,  1.0, 1.0, 1.0};

int vertexIndices[] = { 0, 1, 2 };


void KE::VkGpuInterface::Init()
{
	vkboot_inst_ = createInstance();
	vk_inst_ = vkboot_inst_.instance;

	//VkSurfaceKHR surface_;
	bool test = SDL_Vulkan_CreateSurface(window_, vk_inst_, nullptr, (VkSurfaceKHR*)&(swapchain_.surface_));

	

	deviceManager_ = KE::VK::DeviceManager(createDevice(vkboot_inst_, swapchain_.surface_));

	swapchain_.Init(deviceManager_.GetDevice(), swapchain_.surface_, 800, 600);

	graphics_queue_ = deviceManager_.GetGraphicsQueue();
	compute_queue_ = deviceManager_.GetComputeQueue();

	computeCmdPool = deviceManager_.CreateComputeCommandPool();

	VK::Buffer buff = VK::Buffer(deviceManager_.device_, 1024, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	
	std::shared_ptr<VK::BufferSet> bufferset = deviceManager_.RequestNamedBufferSet("TestBuffer", 1024, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);


	KE::VK::SlangCompileContext compileContext;

	SlangCompiledUnit slangShader = compileContext.CompileShaderPath("../../../../../../Engine/src/GpuInterface/shaders/slangcompute.slang");

	SlangCompiledUnit slangGraphicsShader = compileContext.CompileShaderPath("../../../../../../Engine/src/GpuInterface/shaders/combinedVertFrag.slang");


	Slang::ComPtr<slang::IBlob> blob = slangShader.getTargetCode();

	auto glslComputeSpv = CompileGlslShader("../../../../../../Engine/src/GpuInterface/shaders/glslcompute.comp", shaderc_compute_shader);
	std::ofstream file("spvDescriptorHeapTEST2.spv", std::ios::binary);
	file.write((char*)blob.get()->getBufferPointer(), blob.get()->getBufferSize());
	file.close();
	


	KE::VK::PipelineCompute pipeline(deviceManager_.device_, slangShader);

	testPipeline = KE::VK::PipelineGraphics(deviceManager_.device_, slangGraphicsShader);

	vertexBuffer = KE::VK::Buffer(deviceManager_.device_, sizeof(vertexInfo), vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	f32* mappedVertBuffer = (float*)vertexBuffer.map();
	memcpy(mappedVertBuffer, vertexInfo, sizeof(vertexInfo));

	for (int i = 0; i < 10; i++)
		std::cout << "TEST VALUE: " << std::dec << mappedVertBuffer[i] << std::endl;

	vertexBuffer.unmap();
	vk::DescriptorPool descPool = createDescriptorPool(deviceManager_.device_);

	std::shared_ptr<VK::BufferSet> bufferAttach = deviceManager_.GetBufferSetByName("TestBuffer");
	

	vk::PhysicalDeviceDescriptorHeapPropertiesEXT heapProps;
	vk::PhysicalDeviceProperties2 props2;
	props2.pNext = &heapProps;
	((vk::PhysicalDevice)deviceManager_.device_).getProperties2(&props2);

	

	vk::DeviceSize bufDescSize = heapProps.bufferDescriptorSize;
	vk::DeviceSize reservedRange = heapProps.minResourceHeapReservedRange;


	KE::VK::DescriptorHeapBuffer descriptorHeap(deviceManager_.device_, 2048, KE::VK::DescriptorHeapType::ResourceHeap);
	
	vk::ResourceDescriptorInfoEXT resInfo;
	resInfo.type = vk::DescriptorType::eStorageBuffer;
	resInfo.data.pAddressRange = buff.GetBufferAddressRangePtr();

	descriptorHeap.EnterDescriptor(&resInfo);

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;


	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = computeCmdPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer = vk::Device(deviceManager_.device_).allocateCommandBuffers(allocInfo)[0];

	commandBuffer.begin(beginInfo);

	glm::uvec2 pushIndex = glm::uvec2(6048, 6048);

	vk::PushDataInfoEXT pushInfo{};
	pushInfo.offset = 0;
	pushInfo.data.address = &pushIndex;
	pushInfo.data.size = sizeof(pushIndex);


	commandBuffer.pushDataEXT(&pushInfo);

	commandBuffer.bindResourceHeapEXT(descriptorHeap.GetBindInfo());

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);

	commandBuffer.dispatch(4, 1, 1);

	// End recording
	commandBuffer.end();

	// Submit to compute queue
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	compute_queue_.submit(1, &submitInfo, nullptr);  // no fence for now
	compute_queue_.waitIdle();  // wait for completion (blocking)
	
	u32* numberPointer = (u32*)(buff.map());
	

	for (int i = 0; i < 1024 / 4; i++)
		std::cout << numberPointer[i] << std::endl;

}


u64 KE::VkGpuInterface::GetSDLWindowFlag() { return SDL_WINDOW_VULKAN; }
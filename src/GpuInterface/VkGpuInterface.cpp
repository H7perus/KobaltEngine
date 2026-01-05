#pragma once
#include "VkGpuInterface.h"
#include "VkHelpers.h"

#include "Types/Buffer.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;


#include "Functional/ShaderCompile.h"


#include <string>



void KE::VkGpuInterface::Init()
{
	vkboot_inst_ = createInstance();

	vk_inst_ = vkboot_inst_.instance;

	device_ = KE::VK::Device(createDevice(vkboot_inst_));


	graphics_queue_ = getGraphicsQueue(device_);
	compute_queue_ = getComputeQueue(device_);

	computeCmdPool = createComputeCommandPool(device_);

	VK::Buffer buff = VK::Buffer(&device_, 1024, vk::BufferUsageFlagBits::eStorageBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	vk::DescriptorSetLayout DSLayout = createDescriptorSetLayout(device_);

	vk::PipelineLayout PLayout = createPipelineLayout(device_, DSLayout);
	std::vector<char> shaderCode;

	try
	{
		shaderCode = readFile("../../../../Engine/src/GpuInterface/shaders/slangcompute.slang");
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << shaderCode.data() << std::endl;

	ShaderCompileSetup();

	CompileShader("../../../../Engine/src/GpuInterface/shaders/slangcompute.slang");

	createShaderModule(device_, (char*)spirvCode->getBufferPointer(), spirvCode->getBufferSize());

	std::vector<char> spirvCodeChar((char*)spirvCode->getBufferPointer(), (char*)spirvCode->getBufferPointer() + spirvCode->getBufferSize());

	vk::Pipeline cPipeline = createComputePipeline(PLayout, spirvCodeChar, device_);

	vk::DescriptorPool descPool = createDescriptorPool(device_);

	vk::DescriptorSet descSet = createDescriptorSet(device_, descPool, DSLayout);


	// Describe the buffer
	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buff.buffer;  // your vk::Buffer
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;  // or specific size like 1024

	// Write to descriptor set
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = descSet;
	descriptorWrite.dstBinding = 0;  // which binding in the layout
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vk::Device(device_).updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;


	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = computeCmdPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer = vk::Device(device_).allocateCommandBuffers(allocInfo)[0];

	commandBuffer.begin(beginInfo);

	// Bind pipeline
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, cPipeline);

	// Bind descriptor sets
	commandBuffer.bindDescriptorSets(
		vk::PipelineBindPoint::eCompute,
		PLayout,
		0,  // first set
		1,  // descriptor set count
		&descSet,
		0,  // dynamic offset count
		nullptr
	);

	// Dispatch compute work
	// vkCmdDispatch(groupCountX, groupCountY, groupCountZ)
	// If shader has [numthreads(8, 8, 1)] and you want 64x64 threads total:
	// groupCount = 64/8 = 8 in each dimension
	commandBuffer.dispatch(1, 16, 1);

	// End recording
	commandBuffer.end();

	// Submit to compute queue
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	compute_queue_.submit(1, &submitInfo, nullptr);  // no fence for now
	compute_queue_.waitIdle();  // wait for completion (blocking)
	
	u32* numberPointer = (u32*)buff.map();


	for (int i = 0; i < 1024 / 4; i++)
		std::cout << numberPointer[i] << std::endl;


	std::printf("seems to go right \n");
}


u64 KE::VkGpuInterface::GetSDLWindowFlag() { return SDL_WINDOW_VULKAN; }
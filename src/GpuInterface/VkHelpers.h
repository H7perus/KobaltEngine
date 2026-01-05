#pragma once

#include <iostream>
#include <optional>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include "VkBootstrap.h"

#include "SDL3/SDL_vulkan.h"
#include "BasicTypeAliases.h"




vkb::Instance createInstance()
{
	u32 count;
	SDL_Vulkan_GetInstanceExtensions(&count);

	vkb::InstanceBuilder builder;

	auto inst_ret = builder.set_app_name("Example Vulkan Application")
		.request_validation_layers()
		.require_api_version(1, 4)
		//.use_default_debug_messenger()
		.build();
	if (!inst_ret) { /* report */ }

	vkb::Instance vkb_inst_ = inst_ret.value();

	printf("API Version: %d.%d.%d\n",
		VK_VERSION_MAJOR(vkb_inst_.api_version),
		VK_VERSION_MINOR(vkb_inst_.api_version),
		VK_VERSION_PATCH(vkb_inst_.api_version));

	VULKAN_HPP_DEFAULT_DISPATCHER.init();
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::Instance(vkb_inst_.instance));

	return vkb_inst_;
}

void createSurface(vk::SurfaceKHR& surface, SDL_Window* window, vk::Instance instance)
{

	//SUS: weirded out by instance having to be created with the SDL extensions when it works fine without. Weird...
	bool err = SDL_Vulkan_CreateSurface(window, instance, NULL, (VkSurfaceKHR*)&surface);

	if (!err) { /* handle error */ }
}


//TODO: We need to be able to pass requirements for the device.
vkb::Device createDevice(vkb::Instance vkb_inst)
{
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	auto phys_ret = selector//.set_surface(NULL)
		.set_minimum_version(1, 4)
		.require_present(false)
		.select();
	if (!phys_ret) {
		std::cerr << "Error code: " << phys_ret.vk_result() << std::endl;
		std::cerr << "Error message: " << phys_ret.error().message() << std::endl;
	}

	vk::PhysicalDeviceVulkan13Features features;
	features.dynamicRendering = VK_TRUE;

	vk::PhysicalDeviceVulkan14Features feat2;

	//vk::PhysicalDeviceShaderObjectFeaturesEXT shaderObjectFeat;
	//shaderObjectFeat.shaderObject = VK_TRUE;

	//vk::PhysicalDeviceExtendedDynamicState3FeaturesEXT DynamicStateFeat;
	//DynamicStateFeat.extendedDynamicState3RasterizationSamples = VK_TRUE;


	vkb::DeviceBuilder device_builder{ phys_ret.value() };
	auto dev_ret = device_builder
		.add_pNext(&features)
		//.add_pNext(&shaderObjectFeat)
		//.add_pNext(&DynamicStateFeat)
		.build();
	if (!dev_ret) { /* report */ }
	vkb::Device vkb_device = dev_ret.value();

	if (!dev_ret) {
		std::cerr << "Failed to create logical device: " << dev_ret.error().message() << std::endl;
	}
	return vkb_device;
}

//THESE NEXT THREE NEED WORKOVER, THIS CAN NOT PROPERLY SUPPORT ASYNC/DIFFERING PRIORITIES ON DIFFERENT QUEUES

auto getGraphicsQueue(vkb::Device device)
{
	auto graphics_queue_ret = device.get_queue(vkb::QueueType::graphics);
	if (!graphics_queue_ret) { /* report */ }
	return vk::Queue(graphics_queue_ret.value());
}

auto getComputeQueue(vkb::Device device)
{
	auto compute_queue_ret = device.get_queue(vkb::QueueType::compute);
	if (!compute_queue_ret) { /* report */ }
	return vk::Queue(compute_queue_ret.value());
}

auto getPresentQueue(vkb::Device device)
{
	auto present_queue_ret = device.get_queue(vkb::QueueType::present);
	if (!present_queue_ret) { std::cout << present_queue_ret.error() << std::endl; }
	return vk::Queue(present_queue_ret.value());
}


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value() && graphicsFamily.has_value();
	}
};

QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {

		if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
			indices.computeFamily = i;
		}
		//VkBool32 presentSupport = false;
		//presentSupport = device.getSurfaceSupportKHR(i, VKS.surface);

		//if (presentSupport)
		//	indices.presentFamily = i;

		if (indices.isComplete())
			break;

		i++;
	}

	return indices;
}


vk::CommandPool createComputeCommandPool(vkb::Device device) {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vk::PhysicalDevice(device.physical_device));

	vk::CommandPoolCreateInfo poolInfo{};
	poolInfo.flags = vk::CommandPoolCreateFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();


	vk::CommandPool pool;

	try
	{
		pool = vk::Device(device.device).createCommandPool(poolInfo, nullptr);
	}
	catch (const vk::SystemError& err) {
		std::cerr << "Failed to create command pool: " << err.what() << std::endl;
	}

	return pool;
}

vk::DescriptorSetLayout createDescriptorSetLayout(vkb::Device vkboot_device)
{
	vk::DescriptorSetLayoutBinding binding;
	binding.binding = 0;  // matches binding = 0 in shader
	binding.descriptorType = vk::DescriptorType::eStorageBuffer;
	binding.descriptorCount = 1;  // number of descriptors (1 buffer)
	binding.stageFlags = vk::ShaderStageFlagBits::eCompute;  // used in compute shader
	binding.pImmutableSamplers = nullptr;  // only for samplers

	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &binding;

	return vk::Device(vkboot_device.device).createDescriptorSetLayout(layoutInfo);
}

vk::PipelineLayout createPipelineLayout(vkb::Device vkboot_device, vk::DescriptorSetLayout& DSL)
{
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DSL;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	return vk::Device(vkboot_device).createPipelineLayout(pipelineLayoutInfo);
}

vk::ShaderModule createShaderModule(KE::VK::Device device, const char* code, int size) {
	vk::ShaderModuleCreateInfo createInfo{};
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

	vk::ShaderModule shaderModule;

	shaderModule = vk::Device(device).createShaderModule(createInfo);
	return shaderModule;
}

vk::Pipeline createComputePipeline(vk::PipelineLayout pipelineLayout, std::vector<char> spirvCode, vkb::Device vkboot_device)
{

	vk::Device device = vk::Device(vkboot_device.device);

	vk::ShaderModuleCreateInfo shaderModuleInfo;
	shaderModuleInfo.codeSize = spirvCode.size();
	shaderModuleInfo.pCode = reinterpret_cast<const u32*>(spirvCode.data());
	std::cout << "Magic number: 0x" << std::hex << reinterpret_cast<const u32*>(spirvCode.data())[0] << std::endl;
	vk::ShaderModule shaderModule = device.createShaderModule(shaderModuleInfo);

	// Create the compute pipeline
	vk::PipelineShaderStageCreateInfo shaderStageInfo;
	shaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";  // entry point name in your shader

	vk::ComputePipelineCreateInfo pipelineInfo;
	pipelineInfo.stage = shaderStageInfo;
	pipelineInfo.layout = pipelineLayout;  // from previous step

	


	auto result = device.createComputePipeline(nullptr, pipelineInfo);
	

	// Clean up shader module (can be destroyed after pipeline creation)
	device.destroyShaderModule(shaderModule);

	return result.value;
}

vk::DescriptorPool createDescriptorPool(vkb::Device vkboot_device)
{
	vk::DescriptorPoolSize poolSize;
	poolSize.type = vk::DescriptorType::eStorageBuffer;
	poolSize.descriptorCount = 1;  // one storage buffer

	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;

	return vk::Device(vkboot_device.device).createDescriptorPool(poolInfo);
}

vk::DescriptorSet createDescriptorSet(vkb::Device vkboot_device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout descriptorSetLayout)
{
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;  // from earlier

	std::vector<vk::DescriptorSet> descriptorSets = vk::Device(vkboot_device.device).allocateDescriptorSets(allocInfo);
	return descriptorSets[0];
}

void updateDescriptorSet(vkb::Device vkboot_device, vk::Buffer buffer, vk::DescriptorSet descriptorSet)
{
	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buffer;  // the vk::Buffer you created
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;  // or specific size in bytes

	// Write the buffer to the descriptor set
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;  // matches binding = 0 in shader/layout
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = vk::DescriptorType::eStorageBuffer;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vk::Device(vkboot_device.device).updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
}

vk::CommandBuffer allocateCommandBuffer(vkb::Device vkboot_device, vk::CommandPool commandPool)
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = commandPool;  // compute command pool from earlier
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	std::vector<vk::CommandBuffer> commandBuffers = vk::Device(vkboot_device.device).allocateCommandBuffers(allocInfo);
	return commandBuffers[0];
}





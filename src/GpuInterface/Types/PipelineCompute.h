#pragma once

#include "vulkan/vulkan.hpp"

#include "../ShaderCompile/SlangCompiledUnit.h"

#include "Pipeline.h"
//#include "DescriptorSetLayoutInfo.h"


namespace KE::VK
{
	class PipelineCompute : Pipeline
	{
	public:

		operator vk::Pipeline()
		{
			return pipeline_;
		}

		PipelineCompute();

		//FOR DESCRIPTOR HEAP TESTING
		PipelineCompute(const vk::Device& device, std::vector<uint32_t> spirv)
		{
			vk::ShaderModuleCreateInfo shaderModuleInfo;
			shaderModuleInfo.codeSize = spirv.size() * 4;
			shaderModuleInfo.pCode = spirv.data();
			vk::ShaderModule shaderModule = device.createShaderModule(shaderModuleInfo);

			vk::PipelineShaderStageCreateInfo shaderStageInfo;
			shaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main";

			vk::PipelineCreateFlags2CreateInfo flags2Info;
			flags2Info.flags = vk::PipelineCreateFlagBits2::eDescriptorHeapEXT;

			vk::ComputePipelineCreateInfo pipelineInfo;
			pipelineInfo.pNext = &flags2Info;
			pipelineInfo.stage = shaderStageInfo;
			pipelineInfo.layout = VK_NULL_HANDLE; // pipelinelayout_;  // from previous step

			auto result = device.createComputePipeline(nullptr, pipelineInfo);

			// Clean up shader module (can be destroyed after pipeline creation)
			device.destroyShaderModule(shaderModule);

			pipeline_ = result.value;
		}
		PipelineCompute(const vk::Device& device, SlangCompiledUnit shader)
		{
			Slang::ComPtr<slang::IBlob> SpirV = shader.getTargetCode();

			vk::ShaderModuleCreateInfo shaderModuleInfo;
			shaderModuleInfo.codeSize = SpirV->getBufferSize();
			shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(SpirV->getBufferPointer());
			vk::ShaderModule shaderModule = device.createShaderModule(shaderModuleInfo);


			vk::PipelineShaderStageCreateInfo shaderStageInfo;
			shaderStageInfo.stage = vk::ShaderStageFlagBits::eCompute;
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main";  // entry point name in your shader

			vk::PipelineCreateFlags2CreateInfo flags2Info;
			flags2Info.flags = vk::PipelineCreateFlagBits2::eDescriptorHeapEXT;

			vk::ComputePipelineCreateInfo pipelineInfo;
			pipelineInfo.pNext = &flags2Info;
			pipelineInfo.stage = shaderStageInfo;
			pipelineInfo.layout = VK_NULL_HANDLE;  // from previous step

			auto result = device.createComputePipeline(nullptr, pipelineInfo);

			// Clean up shader module (can be destroyed after pipeline creation)
			device.destroyShaderModule(shaderModule);

			pipeline_ = result.value;

		}

	};
}
#include "PipelineGraphics.h"

using namespace KE::VK;
PipelineGraphics::PipelineGraphics(const vk::Device &device, SlangCompiledUnit shader)
{
    vk::ShaderModuleCreateInfo shaderModuleInfo;

    Slang::ComPtr<slang::IBlob> SpirV = shader.getTargetCode();
    shaderModuleInfo.codeSize = SpirV->getBufferSize();
    shaderModuleInfo.pCode = reinterpret_cast<const uint32_t *>(SpirV->getBufferPointer());

    vk::ShaderModule shaderModule = device.createShaderModule(shaderModuleInfo);

    vk::PipelineShaderStageCreateInfo vertexStageInfo;
    vertexStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
    vertexStageInfo.module = shaderModule;
    vertexStageInfo.pName = "vertMain";
    vk::PipelineShaderStageCreateInfo fragmentStageInfo;
    fragmentStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
    fragmentStageInfo.module = shaderModule;
    fragmentStageInfo.pName = "fragMain";

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vertexStageInfo, fragmentStageInfo};

    vk::Format format = vk::Format::eR8G8B8Unorm;

    vk::PipelineRenderingCreateInfo renderingCreateInfo;
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &format;
    vk::PipelineCreateFlags2CreateInfo flags2Info;
    flags2Info.pNext = &renderingCreateInfo;
    flags2Info.flags = vk::PipelineCreateFlagBits2::eDescriptorHeapEXT;

    vk::VertexInputBindingDescription bindings[] = {
        vk::VertexInputBindingDescription(0, 32, vk::VertexInputRate::eVertex)};

    vk::VertexInputAttributeDescription attrs[] = {
        vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0),  // pos
        vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, 12), // color
        vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, 24),    // uv
    };

    auto vertexInput = vk::PipelineVertexInputStateCreateInfo{}
                           .setVertexBindingDescriptions(bindings)
                           .setVertexAttributeDescriptions(attrs);

    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
    pipelineInputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;

    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
    pipelineRasterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    pipelineRasterizationStateCreateInfo.cullMode = vk::CullModeFlagBits::eBack;

    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
    pipelineViewportStateCreateInfo.scissorCount = 1;
    pipelineViewportStateCreateInfo.viewportCount = 1;

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.pNext = &flags2Info;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.layout = VK_NULL_HANDLE;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
    pipelineInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
    pipelineInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
    pipelineInfo.pViewportState = &pipelineViewportStateCreateInfo;
    pipelineInfo.pDynamicState = &pipelineDynamicStateCreateInfo;

    auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);
}
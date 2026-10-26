#pragma once
#include "RenderTriangle.h"

#include "Engine/General/View.h"
#include "GpuInterface/ShaderCompile/SlangCompileContext.h"

#include "GpuInterface/Types/DeviceManager.h"

#include "Base/Time/Clocks.h"
#include "GpuInterface/Types/PipelineGraphics.h"
#include "Renderer/Types/RenderContext.h"
#include "Engine/General/Perspective.h"
#include "ShaderCompile/ShaderReflection.h"
#include "glm/ext/quaternion_geometric.hpp"
#include "vulkan/vulkan.hpp"


#include <glm/ext/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>


f32 vertexAttribs[] = {1.0, 0.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};


KE::VK::RenderTriangle::RenderTriangle()
{
    Writes("SwapchainOutput");
    Writes("HDRFramebuffer");

    stageFlags_ = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                  vk::PipelineStageFlagBits2::eColorAttachmentOutput;

    f32 vertexPos[] = {0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f};

    f32* mappedPosBuffer = (float*)KE::R::RenderContext::GetInstance().vertexPosBuffer.map();
    memcpy(mappedPosBuffer, vertexPos, sizeof(vertexPos));
    KE::R::RenderContext::GetInstance().vertexPosBuffer.unmap();


    KE::VK::SlangCompileContext compileContext;
    SlangCompiledUnit           slangGraphicsShader =
        compileContext.CompileShaderPath("../../../../../Engine/src/Renderer/Shaders/ErrorModel.slang");


    std::vector<CompileTimeConstantValue> specConstValues;

    specConstValues.push_back({.name = "isRed", .value = 2});
    

    auto specialised = slangGraphicsShader.Specialise(compileContext.session, specConstValues);

    auto targetCode = slangGraphicsShader.getTargetCode();

    int testSize = targetCode->getBufferSize();
    
    R::RenderContext::GetInstance().pipelines.push_back(
        std::make_shared<VK::PipelineGraphics>(0, specialised));
};


KE::VK::RenderTriangle::RenderTriangle(u32 swapchainIndex, VkGpuInterface* gpuInterface)
    : swapchainIndex_(swapchainIndex), gpuInterface_(gpuInterface)
{
    Writes("SwapchainOutput");
    Writes("HDRFramebuffer");

    stageFlags_ = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                  vk::PipelineStageFlagBits2::eColorAttachmentOutput;

    f32 vertexPos[] = {0.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f};

    f32* mappedPosBuffer = (float*)KE::R::RenderContext::GetInstance().vertexPosBuffer.map();
    memcpy(mappedPosBuffer, vertexPos, sizeof(vertexPos));
    KE::R::RenderContext::GetInstance().vertexPosBuffer.unmap();
}

void KE::VK::RenderTriangle::Execute(vk::CommandBuffer cmd)
{

    auto& renderContext = R::RenderContext::GetInstance();

    mesh = &renderContext.meshes_["C:/Users/raket/source/repos/CobaltFPSTest/Engine/assets/error.gltf"]->submeshes_[0];

    SwapchainOutput& swapchainOutput = writes_["SwapchainOutput"].GetResourceRef<SwapchainOutput>();

    Texture& HDRFramebuffer = writes_["HDRFramebuffer"].GetResourceRef<Texture>();


    VK::Buffer& posBuffer = KE::R::RenderContext::GetInstance().vertexPosBuffer;

    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcStageMask  = vk::PipelineStageFlagBits2::eNone;
    barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
    barrier.dstStageMask  = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;

    barrier.oldLayout        = vk::ImageLayout::eGeneral;
    barrier.newLayout        = vk::ImageLayout::eColorAttachmentOptimal;
    barrier.image            = HDRFramebuffer.GetVkImage();
    barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo depInfo{};
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers    = &barrier;

    cmd.pipelineBarrier2(depInfo);

    vk::RenderingAttachmentInfo colorAttachment{};
    colorAttachment.imageView   = HDRFramebuffer.GetVkImageView();
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp      = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp     = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue  = vk::ClearColorValue{0.1f, 0.1f, 0.1f, 1.0f};

    vk::RenderingInfo renderingInfo{};
    renderingInfo.renderArea           = vk::Rect2D{{0, 0}, swapchainOutput.GetExtent()};
    renderingInfo.layerCount           = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments    = &colorAttachment;


    cmd.beginRendering(renderingInfo);

    vk::Viewport viewport{};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = swapchainOutput.GetExtent().width;
    viewport.height   = swapchainOutput.GetExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    cmd.setViewport(0, viewport);

    vk::Rect2D scissor{{0, 0}, swapchainOutput.GetExtent()};
    cmd.setScissor(0, scissor);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderContext.pipelines[0]->GetPipeline());
    std::array<vk::DeviceSize, 2> offsets = {0, 0};

    std::array<vk::Buffer, 2> buffers = {posBuffer.GetVkBuffer(), mesh->vertexAttributes_.GetVkBuffer()};

    std::array<vk::VertexInputBindingDescription2EXT, 2> bindings;

    bindings[0].binding = 0;
    bindings[0].stride  = 12;
    bindings[0].divisor = 1;


    bindings[1].binding = 1;
    bindings[1].stride  = 0;
    bindings[1].divisor = 1;
    // bindings[1] = mesh->binding_;


    cmd.setVertexInputEXT(bindings, mesh->attributeDescs_);
    cmd.bindVertexBuffers(0, 1, buffers.data(), offsets.data());

    cmd.bindIndexBuffer(mesh->indices_.GetVkBuffer(), 0, vk::IndexType::eUint32);

    cmd.bindResourceHeapEXT(ContextManager::GetDevice(0).GetResourceHeapBindInfoPtr());
    cmd.bindSamplerHeapEXT(ContextManager::GetDevice(0).GetSamplerHeapBindInfoPtr());

    // cmd.draw(3, 1, 0, 0);

    cmd.drawIndexed(mesh->count_, 1, 0, 0, 0);

    cmd.endRendering();

    vk::ImageMemoryBarrier2 toSampled{};
    toSampled.srcStageMask     = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    toSampled.srcAccessMask    = vk::AccessFlagBits2::eColorAttachmentWrite;
    toSampled.dstStageMask     = vk::PipelineStageFlagBits2::eFragmentShader;
    toSampled.dstAccessMask    = vk::AccessFlagBits2::eShaderRead;
    toSampled.oldLayout        = vk::ImageLayout::eColorAttachmentOptimal;
    toSampled.newLayout        = vk::ImageLayout::eGeneral;
    toSampled.image            = HDRFramebuffer.GetVkImage();
    toSampled.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo depInfo2{};
    depInfo2.imageMemoryBarrierCount = 1;
    depInfo2.pImageMemoryBarriers    = &toSampled;

    cmd.pipelineBarrier2(depInfo2);
}

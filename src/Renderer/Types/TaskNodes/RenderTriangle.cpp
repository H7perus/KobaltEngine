#pragma once
#include "RenderTriangle.h"

#include "GpuInterface/ShaderCompile/SlangCompileContext.h"

#include "GpuInterface/Types/DeviceManager.h"

#include "GpuInterface/Types/PipelineGraphics.h"
#include "Renderer/Types/RenderContext.h"


f32 vertexAttribs[] = {1.0, 0.0, 0.0, 0.5, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};


KE::VK::RenderTriangle::RenderTriangle()
{
    Writes("SwapchainOutput");
    stageFlags_          = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                           vk::PipelineStageFlagBits2::eColorAttachmentOutput;

    f32  vertexPos[]     = {0.0f, -1.0f, 0.5f, -1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f};
    f32* mappedPosBuffer = (float*)KE::R::RenderContext::GetInstance().vertexPosBuffer.map();
    memcpy(mappedPosBuffer, vertexPos, sizeof(vertexPos));
    KE::R::RenderContext::GetInstance().vertexPosBuffer.unmap();


    KE::VK::SlangCompileContext compileContext;
    SlangCompiledUnit slangGraphicsShader =
        compileContext.CompileShaderPath("../../../../../Engine/src/GpuInterface/shaders/combinedVertFrag.slang");

    R::RenderContext::GetInstance().pipelines_.push_back(std::make_shared<VK::PipelineGraphics>(0, slangGraphicsShader));
};


KE::VK::RenderTriangle::RenderTriangle(u32 swapchainIndex, VkGpuInterface* gpuInterface)
    : swapchainIndex_(swapchainIndex), gpuInterface_(gpuInterface)
{
    Writes("SwapchainOutput");

    stageFlags_ = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                  vk::PipelineStageFlagBits2::eColorAttachmentOutput;

    f32 vertexPos[] = {0.0f, -1.0f, 0.5f, -1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f};

    f32* mappedPosBuffer = (float*)KE::R::RenderContext::GetInstance().vertexPosBuffer.map();
    memcpy(mappedPosBuffer, vertexPos, sizeof(vertexPos));
    KE::R::RenderContext::GetInstance().vertexPosBuffer.unmap();
}

void KE::VK::RenderTriangle::Execute(vk::CommandBuffer cmd)
{

    auto& renderContext = R::RenderContext::GetInstance();

    mesh = &renderContext.meshes_["test"]->submeshes_[0];

    SwapchainOutput& swapchainOutput = writes_["SwapchainOutput"].GetResourceRef<SwapchainOutput>();

    VK::Buffer& posBuffer = KE::R::RenderContext::GetInstance().vertexPosBuffer;

    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcStageMask  = vk::PipelineStageFlagBits2::eNone;
    barrier.srcAccessMask = vk::AccessFlagBits2::eNone;
    barrier.dstStageMask  = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;

    barrier.oldLayout        = swapchainOutput.GetLayout();
    barrier.newLayout        = vk::ImageLayout::eColorAttachmentOptimal;
    barrier.image            = swapchainOutput.GetImage();
    barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    vk::DependencyInfo depInfo{};
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers    = &barrier;

    cmd.pipelineBarrier2(depInfo);

    vk::RenderingAttachmentInfo colorAttachment{};
    colorAttachment.imageView   = swapchainOutput.GetImageView();
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

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderContext.pipelines_[0]->GetPipeline());
    std::array<vk::DeviceSize, 2> offsets = {0, 0};

    std::array<vk::Buffer, 2> buffers = {posBuffer.GetVkBuffer(), mesh->vertexAttributes_.GetVkBuffer()};

    std::array<vk::VertexInputBindingDescription2EXT, 2> bindings;

    bindings[0].binding = 0;
    bindings[0].stride  = 12;

    bindings[1] = mesh->binding_;


    cmd.setVertexInputEXT(bindings, mesh->attributeDescs_);
    cmd.bindVertexBuffers(0, 2, buffers.data(), offsets.data());

    cmd.bindResourceHeapEXT(ContextManager::GetDevice(0).GetResourceHeapBindInfoPtr());
    cmd.bindSamplerHeapEXT(ContextManager::GetDevice(0).GetSamplerHeapBindInfoPtr());

    cmd.draw(3, 1, 0, 0);

    cmd.endRendering();
}

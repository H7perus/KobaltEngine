#include "RenderTriangle.h"
#include "../DeviceManager.h"

KE::VK::RenderTriangle::RenderTriangle(u32 swapchainIndex, VkGpuInterface* gpuInterface)
    : swapchainIndex_(swapchainIndex), gpuInterface_(gpuInterface)
{
    Writes("SwapchainOutput");
    stageFlags_ = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                  vk::PipelineStageFlagBits2::eColorAttachmentOutput;
}

void KE::VK::RenderTriangle::Execute(vk::CommandBuffer cmd)
{
    SwapchainOutput& swapchainOutput = writes_["SwapchainOutput"].GetResourceRef<SwapchainOutput>();

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

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, gpuInterface_->testPipeline.GetPipeline());
    vk::DeviceSize offset = 0;
    cmd.bindVertexBuffers(0, 1, &gpuInterface_->vertexBuffer.buffer, &offset);

    cmd.bindResourceHeapEXT(ContextManager::GetDevice(0).GetResourceHeapBindInfoPtr());
    cmd.bindSamplerHeapEXT(ContextManager::GetDevice(0).GetSamplerHeapBindInfoPtr());

    cmd.draw(3, 1, 0, 0);

    cmd.endRendering();
}

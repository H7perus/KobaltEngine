
#include "TonemapAndPresent.h"
#include "GpuInterface/ShaderCompile/SlangCompileContext.h"
#include "GpuInterface/Types/SwapchainOutput.h"
#include "GpuInterface/Types/PipelineGraphics.h"
#include "GpuInterface/Types/DeviceManager.h"
#include "Renderer/Types/RenderContext.h"

KE::R::TonemapAndPresent::TonemapAndPresent()
{
    Writes("SwapchainOutput");
    Reads("HDRFramebuffer");
    stageFlags_ = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader |
                  vk::PipelineStageFlagBits2::eColorAttachmentOutput;


    KE::VK::SlangCompileContext compileContext;
    SlangCompiledUnit           slangGraphicsShader = compileContext.CompileShaderPath(
        "../../../../../Engine/src/Renderer/Shaders/ScreenspaceTest.slang");

    auto specialisedShader  = slangGraphicsShader.Specialise();
    pipeline = VK::PipelineGraphics(0, specialisedShader);
};

void KE::R::TonemapAndPresent::Execute(vk::CommandBuffer cmd)
{
    VK::SwapchainOutput&    swapchainOutput = writes_["SwapchainOutput"].GetResourceRef<VK::SwapchainOutput>();
    VK::Texture& HDRFramebuffer = reads_["HDRFramebuffer"].GetResourceRef<VK::Texture>();

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

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.GetPipeline());

    


    u32 hdrBufferTexIndex = HDRFramebuffer.GetResourceHeapIndex();
    
    // vk::HostAddressRangeConstEXT range;
    // range.address = &hdrBufferTexIndex;
    // range.size = sizeof(hdrBufferTexIndex);
    // vk::PushDataInfoEXT pushInfo;
    // pushInfo.data = range;

    // cmd.pushDataEXT(pushInfo);

    cmd.draw(3, 1, 0, 0);
    cmd.endRendering();
    KE::VK::ContextManager::GetSwapchain(0).EndFrame();

};
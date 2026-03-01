#include "Swapchain.h"

void KE::VK::Swapchain::Init(KE::VK::Device &device, vk::SurfaceKHR surface, uint32_t width, uint32_t height)
{
    device_        = &device;
    graphicsQueue_ = device.GetGraphicsQueue();
    surface_       = surface;
    BuildSwapchain(width, height);
}

KE::VK::Frame &KE::VK::Swapchain::BeginNextFrame()
{
    Frame &frame = frames_[currentFrame_];

    device_->GetVkDevice().waitForFences(frame.inFlightFence_, vk::True, UINT64_MAX);
    device_->GetVkDevice().resetFences(frame.inFlightFence_);

    try
    {
        auto [result, currentImage] =
        device_->GetVkDevice().acquireNextImageKHR(swapchain_, UINT64_MAX, frame.imageAvailableSemaphore_, nullptr);
        currentImage_ = currentImage;
    }
    catch (vk::OutOfDateKHRError &)
    {
        Recreate(800, 600);
        return BeginNextFrame();
    };

    return frame;
}

void KE::VK::Swapchain::EndFrame()
{

    vk::SubmitInfo submitInfo{};
    submitInfo.waitSemaphoreCount = 1;
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores    = &frames_[currentFrame_].imageAvailableSemaphore_;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores  = &frames_[currentFrame_].renderFinishedSemaphore_;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &frames_[currentFrame_].commandBuffer_;

    graphicsQueue_.submit(submitInfo, frames_[currentFrame_].inFlightFence_);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &frames_[currentFrame_].renderFinishedSemaphore_;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain_;
    presentInfo.pImageIndices      = &currentImage_;

    try
    {
        graphicsQueue_.presentKHR(presentInfo);
    }
    catch (vk::OutOfDateKHRError &)
    {
    }
    currentFrame_ = (currentFrame_ + 1) % frames_.size();
}

void KE::VK::Swapchain::BuildSwapchain(uint32_t width, uint32_t height)
{
    vkb::SwapchainBuilder builder{(vkb::Device)(*device_)};

    auto result = builder.set_desired_extent(width, height)
                      .set_desired_format({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
                      .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
                      .set_old_swapchain(vkbSwapchain_)
                      .build();

    if (!result)
    {
        // handle error
        printf("Swapchain build failed: %s\n", result.error().message().c_str());
        return;
    }

    vkbSwapchain_ = result.value();

    swapchain_ = (vk::SwapchainKHR)(vkbSwapchain_.swapchain);
    format_    = vk::Format(vkbSwapchain_.image_format);
    extent_    = vk::Extent2D{vkbSwapchain_.extent.width, vkbSwapchain_.extent.height};

    // grab images
    auto images = vkbSwapchain_.get_images().value();
    images_.assign(images.begin(), images.end());


    // grab image views
    auto imageViews = vkbSwapchain_.get_image_views().value();
    imageViews_.assign(imageViews.begin(), imageViews.end());

    // create frames
    frames_.resize(imageViews_.size());
    for (size_t i = 0; i < frames_.size(); i++)
    {
        frames_[i].Init(*device_);
    }
}

void KE::VK::Swapchain::Recreate(uint32_t width, uint32_t height)
{
    // destroy old image views before rebuilding
    Destroy();
    BuildSwapchain(width, height);
}

void KE::VK::Swapchain::Destroy()
{
    vk::Device device = (vk::Device)(*device_);

    for (auto &iv : imageViews_)
        device.destroyImageView(iv);

    imageViews_.clear();
    images_.clear();

    device.destroySwapchainKHR(swapchain_);
}

#pragma once
#include "Client.h"
#include "Frame/Frame.h"

#include "Base/Time/LoopRateLimiter.h"

#include "../GpuInterface/Types/Swapchain.h"

namespace KE
{
	int Client::Initialise()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("SDL initialization failed: %s\n", SDL_GetError());
			return 1;
		}

		window = SDL_CreateWindow(
			"My SDL Window",              // Window title
			800,                          // Width
			600,                          // Height
			SDL_WINDOW_VULKAN              // Flags
		);

		if (!window) {
			printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		//SDL_SetWindowFullscreen(window, true);

		

		GpuInterface->Init();

		// VkSurfaceKHR surface;
		// SDL_Vulkan_CreateSurface(window, ((VkGpuInterface*)GpuInterface)->vk_inst_, nullptr, &surface);
		// KE::VK::Device dev = ((VkGpuInterface*)GpuInterface)->deviceManager_.GetDevice();
		// ((VkGpuInterface*)GpuInterface)->swapchain_.Init(dev, surface, 800, 600);

		// commandPool = ((VkGpuInterface*)GpuInterface)->deviceManager_.CreateGraphicsCommandPool();

		// vk::CommandBufferAllocateInfo allocInfo;
		// allocInfo.commandPool = commandPool;
		// allocInfo.level = vk::CommandBufferLevel::ePrimary;
		// allocInfo.commandBufferCount = 1;

		// commandBuffer = vk::Device(((VkGpuInterface*)GpuInterface)->deviceManager_.GetDevice()).allocateCommandBuffers(allocInfo)[0];

		return 0;
	}
	void Client::Render()
	{
		// // acquire next image
		// auto [result, imageIndex] = vk::Device(((VkGpuInterface*)GpuInterface)->deviceManager_.GetDevice()).acquireNextImageKHR(((VkGpuInterface*)GpuInterface)->swapchain_.swapchain_, UINT64_MAX, imageAvailableSemaphore, nullptr);
		// if (result == vk::Result::eErrorOutOfDateKHR)
		// {
		// 	((VkGpuInterface*)GpuInterface)->swapchain_.Recreate(800, 600);
		// 	return;
		// }

		// commandBuffer.reset();
		// commandBuffer.begin(vk::CommandBufferBeginInfo{});

		// // transition image to color attachment
		// vk::ImageMemoryBarrier2 barrier{};
		// barrier.image = swapchainImage; // current swapchain image
		// barrier.oldLayout = vk::ImageLayout::eUndefined;
		// barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
		// barrier.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe;
		// barrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
		// barrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
		// barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

		// vk::DependencyInfo depInfo{};
		// depInfo.imageMemoryBarrierCount = 1;
		// depInfo.pImageMemoryBarriers = &barrier;
		// commandBuffer.pipelineBarrier2(depInfo);

		// // begin dynamic rendering
		// vk::RenderingAttachmentInfo colorAttachment{};
		// colorAttachment.imageView = currentImageView;
		// colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
		// colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		// colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		// colorAttachment.clearValue = vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f};

		// vk::RenderingInfo renderingInfo{};
		// renderingInfo.renderArea = vk::Rect2D{{0, 0}, extent};
		// renderingInfo.layerCount = 1;
		// renderingInfo.colorAttachmentCount = 1;
		// renderingInfo.pColorAttachments = &colorAttachment;

		// commandBuffer.beginRendering(renderingInfo);

		// // draw
		// commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		// commandBuffer.setViewport(0, vk::Viewport{0, 0, (float)extent.width, (float)extent.height, 0.0f, 1.0f});
		// commandBuffer.setScissor(0, vk::Rect2D{{0, 0}, extent});
		// commandBuffer.draw(3, 1, 0, 0); // 3 vertices, hardcoded in shader

		// commandBuffer.endRendering();

		// // transition image to present
		// barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
		// barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
		// barrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
		// barrier.dstStageMask = vk::PipelineStageFlagBits2::eBottomOfPipe;
		// barrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
		// barrier.dstAccessMask = vk::AccessFlagBits2::eNone;
		// commandBuffer.pipelineBarrier2(depInfo);

		// commandBuffer.end();

		// // submit
		// vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		// vk::SubmitInfo submitInfo{};
		// submitInfo.waitSemaphoreCount = 1;
		// submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
		// submitInfo.pWaitDstStageMask = &waitStage;
		// submitInfo.commandBufferCount = 1;
		// submitInfo.pCommandBuffers = &commandBuffer;
		// submitInfo.signalSemaphoreCount = 1;
		// submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

		// graphicsQueue.submit(submitInfo, inFlightFence);

		// // present
		// vk::PresentInfoKHR presentInfo{};
		// presentInfo.waitSemaphoreCount = 1;
		// presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
		// presentInfo.swapchainCount = 1;
		// presentInfo.pSwapchains = &swapchain;
		// presentInfo.pImageIndices = &imageIndex;

		// graphicsQueue.presentKHR(presentInfo);

		// // wait for frame to finish
		// device.waitForFences(inFlightFence, VK_TRUE, UINT64_MAX);
		// device.resetFences(inFlightFence);
	}

	void Client::Main()
	{
		LoopRateLimiter fps_limiter(10.f);
		bool isRunning = true;
		while (isRunning)
		{
			fps_limiter.Sleep();
			Frame::RunClientFrame();
		}
	}
}
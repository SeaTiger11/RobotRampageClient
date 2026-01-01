#include "Rendering.h";

void drawFrame(vk::raii::Device& device, vk::raii::CommandBuffer& commandBuffer, vk::raii::Pipeline& graphicsPipeline, vk::raii::Queue& queue, SyncObjects& syncObjects, SwapChainData& swapChainData) {
    queue.waitIdle();

	auto [result, imageIndex] = swapChainData.swapChain.acquireNextImage(UINT64_MAX, *syncObjects.presentCompleteSemaphore, nullptr);

	recordCommandBuffer(imageIndex, commandBuffer, swapChainData, graphicsPipeline);

	device.resetFences(*syncObjects.drawFence);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(&*syncObjects.presentCompleteSemaphore);
    submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*commandBuffer);
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&*syncObjects.renderFinishedSemaphore);

    queue.submit(submitInfo, syncObjects.drawFence);

    result = device.waitForFences(*syncObjects.drawFence, vk::True, UINT64_MAX);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence");
    }

    vk::PresentInfoKHR presentInfoKHR;
    presentInfoKHR.setWaitSemaphoreCount(1);
    presentInfoKHR.setPWaitSemaphores(&*syncObjects.renderFinishedSemaphore);
    presentInfoKHR.setSwapchainCount(1);
    presentInfoKHR.setPSwapchains(&*swapChainData.swapChain);
    presentInfoKHR.setPImageIndices(&imageIndex);

    result = queue.presentKHR(presentInfoKHR);
}
#include "Rendering.h";

void drawFrame(vk::raii::Device& device, std::vector<vk::raii::CommandBuffer>& commandBuffers, vk::raii::Pipeline& graphicsPipeline, vk::raii::Queue& queue, SyncObjects& syncObjects, SwapChainData& swapChainData, uint32_t& frameIndex) {
    auto fenceResult = device.waitForFences(*syncObjects.inFlightFences[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence");
    }
    device.resetFences(*syncObjects.inFlightFences[frameIndex]);

	auto [result, imageIndex] = swapChainData.swapChain.acquireNextImage(UINT64_MAX, *syncObjects.presentCompleteSemaphores[frameIndex], nullptr);

    commandBuffers[frameIndex].reset();
	recordCommandBuffer(imageIndex, commandBuffers[frameIndex], swapChainData, graphicsPipeline);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(&*syncObjects.presentCompleteSemaphores[frameIndex]);
    submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*commandBuffers[frameIndex]);
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&*syncObjects.renderFinishedSemaphores[imageIndex]);

    queue.submit(submitInfo, syncObjects.inFlightFences[frameIndex]);

    vk::PresentInfoKHR presentInfoKHR;
    presentInfoKHR.setWaitSemaphoreCount(1);
    presentInfoKHR.setPWaitSemaphores(&*syncObjects.renderFinishedSemaphores[imageIndex]);
    presentInfoKHR.setSwapchainCount(1);
    presentInfoKHR.setPSwapchains(&*swapChainData.swapChain);
    presentInfoKHR.setPImageIndices(&imageIndex);

    result = queue.presentKHR(presentInfoKHR);
    switch (result)
    {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
            std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR\n";
            break;
        default:
            break;        // an unexpected result is returned!
    }
    frameIndex = (frameIndex + 1) % Constants::MAX_FRAMES_IN_FLIGHT;
}
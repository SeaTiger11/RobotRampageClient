#include "CommandBufferHelper.h";

void transition_image_layout(
	uint32_t imageIndex,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	vk::AccessFlags2 srcAccessMask,
	vk::AccessFlags2 dstAccessMask,
	vk::PipelineStageFlags2 srcStageMask,
	vk::PipelineStageFlags2 dstStageMask,

    vk::raii::CommandBuffer& commandBuffer,
    SwapChainData& swapChainData
) {
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    subresourceRange.setBaseMipLevel(0);
    subresourceRange.setLevelCount(1);
    subresourceRange.setBaseArrayLayer(0);
    subresourceRange.setLayerCount(1);

    vk::ImageMemoryBarrier2 barrier;
    barrier.setSrcStageMask(srcStageMask);
    barrier.setSrcAccessMask(srcAccessMask);
    barrier.setDstStageMask(dstStageMask);
    barrier.setDstAccessMask(dstAccessMask);
    barrier.setOldLayout(oldLayout);
    barrier.setNewLayout(newLayout);
    barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setImage(swapChainData.swapChainImages[imageIndex]);
    barrier.setSubresourceRange(subresourceRange);

    vk::DependencyInfo dependencyInfo;
    dependencyInfo.setDependencyFlags({});
    dependencyInfo.setImageMemoryBarrierCount(1);
    dependencyInfo.setPImageMemoryBarriers(&barrier);

    commandBuffer.pipelineBarrier2(dependencyInfo);
}

void createCommandBuffer(std::vector<vk::raii::CommandBuffer>& commandBuffers, vk::raii::CommandPool& commandPool, vk::raii::Device& device) {
    commandBuffers.clear();

	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(Constants::MAX_FRAMES_IN_FLIGHT);

    commandBuffers = vk::raii::CommandBuffers(device, allocInfo);
}

void recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& commandBuffer, SwapChainData& swapChainData, vk::raii::Pipeline& graphicsPipeline) {
	commandBuffer.begin({});

    // Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
    transition_image_layout(
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},                                                         // srcAccessMask (no need to wait for previous operations)
        vk::AccessFlagBits2::eColorAttachmentWrite,                 // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // srcStage
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,         // dstStage

        commandBuffer,
        swapChainData
    );

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo;
    attachmentInfo.setImageView(swapChainData.swapChainImageViews[imageIndex]);
    attachmentInfo.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
    attachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
    attachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
    attachmentInfo.setClearValue(clearColor);

    vk::Rect2D renderArea;
    renderArea.setOffset({ 0, 0 });
    renderArea.setExtent(swapChainData.swapChainExtent);

    vk::RenderingInfo renderingInfo;
    renderingInfo.setRenderArea(renderArea);
    renderingInfo.setLayerCount(1);
    renderingInfo.setColorAttachmentCount(1);
    renderingInfo.setPColorAttachments(&attachmentInfo);

    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainData.swapChainExtent.width), static_cast<float>(swapChainData.swapChainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainData.swapChainExtent));

    commandBuffer.draw(3, 1, 0, 0);

    commandBuffer.endRendering();

    // After rendering, transition the swapchain image to PRESENT_SRC
    transition_image_layout(
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,             // srcAccessMask
        {},                                                     // dstAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,     // srcStage
        vk::PipelineStageFlagBits2::eBottomOfPipe,               // dstStage

        commandBuffer,
        swapChainData
    );

    commandBuffer.end();
}
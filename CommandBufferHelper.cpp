#include "CommandBufferHelper.h";

void transition_image_layout(
    RobotRampageClient& app,

    vk::Image image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask,
    vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStageMask,
    vk::PipelineStageFlags2 dstStageMask,
    vk::ImageAspectFlags imageAspectFlag
) {
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setAspectMask(imageAspectFlag);
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
    barrier.setImage(image);
    barrier.setSubresourceRange(subresourceRange);

    vk::DependencyInfo dependencyInfo;
    dependencyInfo.setDependencyFlags({});
    dependencyInfo.setImageMemoryBarrierCount(1);
    dependencyInfo.setPImageMemoryBarriers(&barrier);

    app.commandBuffers[app.frameIndex].pipelineBarrier2(dependencyInfo);
}

void createCommandBuffers(RobotRampageClient& app) {
    app.commandBuffers.clear();

	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(app.commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(Constants::MAX_FRAMES_IN_FLIGHT);

    app.commandBuffers = vk::raii::CommandBuffers(app.device, allocInfo);
}

void recordCommandBuffer(RobotRampageClient& app, uint32_t imageIndex) {
    auto& commandBuffer = app.commandBuffers[app.frameIndex];
    commandBuffer.begin({});

    // Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
    transition_image_layout(
        app,
        app.swapChainImages[imageIndex],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    // Transition depth image to depth attachment optimal layout
    transition_image_layout(
        app,
        app.depthImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
        vk::ImageAspectFlagBits::eDepth
    );

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo;
    attachmentInfo.setImageView(app.swapChainImageViews[imageIndex]);
    attachmentInfo.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
    attachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
    attachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
    attachmentInfo.setClearValue(clearColor);

    vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0.0f);
    vk::RenderingAttachmentInfo depthAttachmentInfo;
    depthAttachmentInfo.setImageView(app.depthImageView);
    depthAttachmentInfo.setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal);
    depthAttachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthAttachmentInfo.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachmentInfo.clearValue = clearDepth;

    vk::Rect2D renderArea;
    renderArea.setOffset({ 0, 0 });
    renderArea.setExtent(app.swapChainExtent);

    vk::RenderingInfo renderingInfo;
    renderingInfo.setRenderArea(renderArea);
    renderingInfo.setLayerCount(1);
    renderingInfo.setColorAttachmentCount(1);
    renderingInfo.setPColorAttachments(&attachmentInfo);
    renderingInfo.setPDepthAttachment(&depthAttachmentInfo);

    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, app.graphicsPipeline);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(app.swapChainExtent.width), static_cast<float>(app.swapChainExtent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), app.swapChainExtent));
    commandBuffer.bindVertexBuffers(0, *app.vertexBuffer, { 0 });
    commandBuffer.bindIndexBuffer(*app.indexBuffer, 0, vk::IndexType::eUint32);

    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, app.pipelineLayout, 0, *app.descriptorSets[app.frameIndex], nullptr);
    commandBuffer.drawIndexed(app.indices.size(), 1, 0, 0, 0);

    commandBuffer.endRendering();

    // After rendering, transition the swapchain image to PRESENT_SRC
    transition_image_layout(
        app,
        app.swapChainImages[imageIndex],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor
    );

    commandBuffer.end();
}

vk::raii::CommandBuffer beginSingleTimeCommands(RobotRampageClient& app) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandPool(app.commandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandBufferCount(1);

    vk::raii::CommandBuffer commandBuffer = std::move(app.device.allocateCommandBuffers(allocInfo).front());

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    
    commandBuffer.begin(beginInfo);

    return commandBuffer;
}

void endSingleTimeCommands(RobotRampageClient& app, vk::raii::CommandBuffer& commandBuffer) {
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*commandBuffer);

    app.queue.submit(submitInfo, nullptr);
    app.queue.waitIdle();
}
#include "ImageViewHelper.h";

void createImageViews(SwapChainData& swapChainData, vk::raii::Device& device) {
	swapChainData.swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	imageViewCreateInfo.setFormat(swapChainData.swapChainSurfaceFormat.format);
	imageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	for (auto& image : swapChainData.swapChainImages) {
		imageViewCreateInfo.image = image;
		swapChainData.swapChainImageViews.emplace_back(device, imageViewCreateInfo);
	}
}
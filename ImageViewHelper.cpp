#include "ImageViewHelper.h";

void createImageViews(std::vector<vk::raii::ImageView>& swapChainImageViews, SwapChainData& swapChainData, vk::raii::Device& device) {
	swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	imageViewCreateInfo.setFormat(swapChainData.swapChainSurfaceFormat.format);
	imageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	for (auto& image : swapChainData.swapChainImages) {
		imageViewCreateInfo.image = image;
		swapChainImageViews.emplace_back(device, imageViewCreateInfo);
	}
}
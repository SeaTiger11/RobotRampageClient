#include "SwapChainHelper.h";

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return availableFormat;
	}

	return availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	return {
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

static uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

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

void createSwapChain(SwapChainData& swapChainData, vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, GLFWwindow* window) {
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	swapChainData.swapChainSurfaceFormat = chooseSwapSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));
	swapChainData.swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo;
	swapChainCreateInfo.setSurface(*surface);
	swapChainCreateInfo.setMinImageCount(chooseSwapMinImageCount(surfaceCapabilities));
	swapChainCreateInfo.setImageFormat(swapChainData.swapChainSurfaceFormat.format);
	swapChainCreateInfo.setImageColorSpace(swapChainData.swapChainSurfaceFormat.colorSpace);
	swapChainCreateInfo.setImageExtent(swapChainData.swapChainExtent);
	swapChainCreateInfo.setImageArrayLayers(1);
	swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	swapChainCreateInfo.setPreTransform(surfaceCapabilities.currentTransform);
	swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapChainCreateInfo.setPresentMode(chooseSwapPresentMode(physicalDevice.getSurfacePresentModesKHR(*surface)));
	swapChainCreateInfo.setClipped(true);

	swapChainData.swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
	swapChainData.swapChainImages = swapChainData.swapChain.getImages();
}
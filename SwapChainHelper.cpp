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

void createSwapChain(RobotRampageClient& app) {
	auto surfaceCapabilities = app.physicalDevice.getSurfaceCapabilitiesKHR(app.surface);
	app.swapChainSurfaceFormat = chooseSwapSurfaceFormat(app.physicalDevice.getSurfaceFormatsKHR(app.surface));
	app.swapChainExtent = chooseSwapExtent(surfaceCapabilities, app.window);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo;
	swapChainCreateInfo.setSurface(*app.surface);
	swapChainCreateInfo.setMinImageCount(chooseSwapMinImageCount(surfaceCapabilities));
	swapChainCreateInfo.setImageFormat(app.swapChainSurfaceFormat.format);
	swapChainCreateInfo.setImageColorSpace(app.swapChainSurfaceFormat.colorSpace);
	swapChainCreateInfo.setImageExtent(app.swapChainExtent);
	swapChainCreateInfo.setImageArrayLayers(1);
	swapChainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
	swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	swapChainCreateInfo.setPreTransform(surfaceCapabilities.currentTransform);
	swapChainCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	swapChainCreateInfo.setPresentMode(chooseSwapPresentMode(app.physicalDevice.getSurfacePresentModesKHR(*app.surface)));
	swapChainCreateInfo.setClipped(true);

	app.swapChain = vk::raii::SwapchainKHR(app.device, swapChainCreateInfo);
	app.swapChainImages = app.swapChain.getImages();
}

void cleanupSwapChain(RobotRampageClient& app) {
	app.swapChainImageViews.clear();
	app.swapChain = nullptr;
}

void recreateSwapChain(RobotRampageClient& app) {
	int width = 0, height = 0;
	glfwGetFramebufferSize(app.window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(app.window, &width, &height);
		glfwWaitEvents();
	}

	app.device.waitIdle();

	cleanupSwapChain(app);
	createSwapChain(app);
	createImageViews(app);
	createDepthResources(app);
}
#include "ImageViewHelper.h";

vk::raii::ImageView createImageView(RobotRampageClient& app, vk::raii::Image& image, vk::Format format) {
	vk::ImageViewCreateInfo viewInfo;
	viewInfo.setImage(image);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	return vk::raii::ImageView(app.device, viewInfo);
}

void createImageViews(RobotRampageClient& app) {
	app.swapChainImageViews.clear();

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
	imageViewCreateInfo.setFormat(app.swapChainSurfaceFormat.format);
	imageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	for (auto& image : app.swapChainImages) {
		imageViewCreateInfo.image = image;
		app.swapChainImageViews.emplace_back(app.device, imageViewCreateInfo);
	}
}

void createTextureImageView(RobotRampageClient& app) {
	app.textureImageView = createImageView(app, app.textureImage, vk::Format::eR8G8B8A8Srgb);
}
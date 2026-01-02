#include "ImageHelper.h";

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

vk::raii::ImageView createImageView(RobotRampageClient& app, vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
	vk::ImageViewCreateInfo viewInfo;
	viewInfo.setImage(image);
	viewInfo.setViewType(vk::ImageViewType::e2D);
	viewInfo.setFormat(format);
	viewInfo.setSubresourceRange({ aspectFlags, 0, 1, 0, 1 });

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
	app.textureImageView = createImageView(app, app.textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void copyBufferToImage(RobotRampageClient& app, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height) {
	vk::raii::CommandBuffer commandBuffer = beginSingleTimeCommands(app);

	vk::BufferImageCopy region;
	region.setBufferOffset(0);
	region.setBufferRowLength(0);
	region.setBufferImageHeight(0);
	region.setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 });
	region.setImageOffset({ 0, 0, 0 });
	region.setImageExtent({ width, height, 1 });

	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });

	endSingleTimeCommands(app, commandBuffer);
}

void transitionImageLayout(RobotRampageClient& app, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
	auto commandBuffer = beginSingleTimeCommands(app);

	vk::ImageMemoryBarrier barrier;
	barrier.setOldLayout(oldLayout);
	barrier.setNewLayout(newLayout);
	barrier.setImage(image);
	barrier.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else {
		throw std::invalid_argument("Unsupported layout transition");
	}

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);

	endSingleTimeCommands(app, commandBuffer);
}

void createImage(RobotRampageClient& app, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image, vk::raii::DeviceMemory& imageMemory) {
	vk::ImageCreateInfo imageInfo;
	imageInfo.setImageType(vk::ImageType::e2D);
	imageInfo.setFormat(format);
	imageInfo.setExtent({ width, height, 1 });
	imageInfo.setMipLevels(1);
	imageInfo.setArrayLayers(1);
	imageInfo.setSamples(vk::SampleCountFlagBits::e1);
	imageInfo.setTiling(tiling);
	imageInfo.setUsage(usage);
	imageInfo.setSharingMode(vk::SharingMode::eExclusive);

	image = vk::raii::Image(app.device, imageInfo);

	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo(memRequirements.size, findMemoryType(app, memRequirements.memoryTypeBits, properties));
	imageMemory = vk::raii::DeviceMemory(app.device, allocInfo);
	image.bindMemory(*imageMemory, 0);
}

void createTextureImage(RobotRampageClient& app) {
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/quack.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image");
	}

	vk::raii::Buffer stagingBuffer({});
	vk::raii::DeviceMemory stagingBufferMemory({});

	createBuffer(app, imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
	void* data = stagingBufferMemory.mapMemory(0, imageSize);
	memcpy(data, pixels, imageSize);
	stagingBufferMemory.unmapMemory();

	stbi_image_free(pixels);

	createImage(app, texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, app.textureImage, app.textureImageMemory);

	transitionImageLayout(app, app.textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(app, stagingBuffer, app.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(app, app.textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void createTextureSampler(RobotRampageClient& app) {
	vk::PhysicalDeviceProperties properties = app.physicalDevice.getProperties();

	vk::SamplerCreateInfo samplerInfo;
	samplerInfo.setMagFilter(vk::Filter::eLinear);
	samplerInfo.setMinFilter(vk::Filter::eLinear);
	samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
	samplerInfo.setMipLodBias(0.0f);
	samplerInfo.setMinLod(0.0f);
	samplerInfo.setMaxLod(0.0f);
	samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeV(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAddressModeW(vk::SamplerAddressMode::eRepeat);
	samplerInfo.setAnisotropyEnable(vk::True);
	samplerInfo.setMaxAnisotropy(properties.limits.maxSamplerAnisotropy);
	samplerInfo.setCompareEnable(vk::False);
	samplerInfo.setCompareOp(vk::CompareOp::eAlways);
	samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
	samplerInfo.setUnnormalizedCoordinates(vk::False);

	app.textureSampler = vk::raii::Sampler(app.device, samplerInfo);
}

vk::Format findSupportedFormat(RobotRampageClient& app, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
	for (const auto format : candidates) {
		vk::FormatProperties props = app.physicalDevice.getFormatProperties(format);

		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format");
}

vk::Format findDepthFormat(RobotRampageClient& app) {
	return findSupportedFormat( 
		app,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

bool hasStencilComponent(vk::Format format) {
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void createDepthResources(RobotRampageClient& app) {
	vk::Format depthFormat = findDepthFormat(app);

	createImage(app, app.swapChainExtent.width, app.swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, app.depthImage, app.depthImageMemory);
	app.depthImageView = createImageView(app, app.depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}
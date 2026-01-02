#include "DescriptorSetHelper.h";

void createDescriptorSetLayout(RobotRampageClient& app) {
	std::array bindings = {
		vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
		vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)
	};
	vk::DescriptorSetLayoutCreateInfo layoutInfo({}, static_cast<uint32_t>(bindings.size()), bindings.data());
	app.descriptorSetLayout = vk::raii::DescriptorSetLayout(app.device, layoutInfo);
}

void createDescriptorPool(RobotRampageClient& app) {
	std::array poolSizes = {
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, Constants::MAX_FRAMES_IN_FLIGHT),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, Constants::MAX_FRAMES_IN_FLIGHT)
	};
	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
	poolInfo.setMaxSets(Constants::MAX_FRAMES_IN_FLIGHT);
	poolInfo.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()));
	poolInfo.setPPoolSizes(poolSizes.data());

	app.descriptorPool = vk::raii::DescriptorPool(app.device, poolInfo);
}

void createDescriptorSets(RobotRampageClient& app) {
	std::vector<vk::DescriptorSetLayout> layouts(Constants::MAX_FRAMES_IN_FLIGHT, *app.descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.setDescriptorPool(app.descriptorPool);
	allocInfo.setDescriptorSetCount(static_cast<uint32_t>(layouts.size()));
	allocInfo.setPSetLayouts(layouts.data());

	app.descriptorSets.clear();
	app.descriptorSets = app.device.allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < Constants::MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DescriptorBufferInfo bufferInfo;
		bufferInfo.setBuffer(app.uniformBuffers[i]);
		bufferInfo.setOffset(0);
		bufferInfo.setRange(sizeof(UniformBufferObject));

		vk::DescriptorImageInfo imageInfo;
		imageInfo.setSampler(app.textureSampler);
		imageInfo.setImageView(app.textureImageView);
		imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

		vk::WriteDescriptorSet bufferDescriptorWrite;
		bufferDescriptorWrite.setDstSet(app.descriptorSets[i]);
		bufferDescriptorWrite.setDstBinding(0);
		bufferDescriptorWrite.setDstArrayElement(0);
		bufferDescriptorWrite.setDescriptorCount(1);
		bufferDescriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
		bufferDescriptorWrite.setPBufferInfo(&bufferInfo);

		vk::WriteDescriptorSet imageDescriptorWrite;
		imageDescriptorWrite.setDstSet(app.descriptorSets[i]);
		imageDescriptorWrite.setDstBinding(1);
		imageDescriptorWrite.setDstArrayElement(0);
		imageDescriptorWrite.setDescriptorCount(1);
		imageDescriptorWrite.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
		imageDescriptorWrite.setPImageInfo(&imageInfo);

		std::array descriptorWrites{ bufferDescriptorWrite, imageDescriptorWrite };

		app.device.updateDescriptorSets(descriptorWrites, {});
	}
}
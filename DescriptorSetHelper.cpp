#include "DescriptorSetHelper.h";

void createDescriptorSetLayout(RobotRampageClient& app) {
	vk::DescriptorSetLayoutBinding uboLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
	vk::DescriptorSetLayoutCreateInfo layoutInfo({}, 1, &uboLayoutBinding);
	app.descriptorSetLayout = vk::raii::DescriptorSetLayout(app.device, layoutInfo);
}

void createDescriptorPool(RobotRampageClient& app) {
	vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, Constants::MAX_FRAMES_IN_FLIGHT);
	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
	poolInfo.setMaxSets(Constants::MAX_FRAMES_IN_FLIGHT);
	poolInfo.setPoolSizeCount(1);
	poolInfo.setPPoolSizes(&poolSize);

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

		vk::WriteDescriptorSet descriptorWrite;
		descriptorWrite.setDstSet(app.descriptorSets[i]);
		descriptorWrite.setDstBinding(0);
		descriptorWrite.setDstArrayElement(0);
		descriptorWrite.setDescriptorCount(1);
		descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
		descriptorWrite.setPBufferInfo(&bufferInfo);

		app.device.updateDescriptorSets(descriptorWrite, {});
	}
}
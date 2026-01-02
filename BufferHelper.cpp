#include "BufferHelper.h";

uint32_t findMemoryType(RobotRampageClient& app, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memProperties = app.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

void copyBuffer(RobotRampageClient& app, vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size) {
	vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands(app);
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	endSingleTimeCommands(app, commandCopyBuffer);
}

void createBuffer(RobotRampageClient& app, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory) {
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.setSize(size);
	bufferInfo.setUsage(usage);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	buffer = vk::raii::Buffer(app.device, bufferInfo);

	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocationInfo;
	memoryAllocationInfo.setAllocationSize(memRequirements.size);
	memoryAllocationInfo.setMemoryTypeIndex(findMemoryType(app, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

	bufferMemory = vk::raii::DeviceMemory(app.device, memoryAllocationInfo);
	buffer.bindMemory(*bufferMemory, 0);
}

void createVertexBuffer(RobotRampageClient& app) {
	vk::DeviceSize bufferSize = sizeof(app.vertices[0]) * app.vertices.size();
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, app.vertices.data(), bufferSize);
	stagingBufferMemory.unmapMemory();

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, app.vertexBuffer, app.vertexBufferMemory);

	copyBuffer(app, stagingBuffer, app.vertexBuffer, bufferSize);
}

void createIndexBuffer(RobotRampageClient& app) {
	vk::DeviceSize bufferSize = sizeof(app.indices[0]) * app.indices.size();
	vk::raii::Buffer stagingBuffer = nullptr;
	vk::raii::DeviceMemory stagingBufferMemory = nullptr;

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);
	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, app.indices.data(), bufferSize);
	stagingBufferMemory.unmapMemory();

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, app.indexBuffer, app.indexBufferMemory);

	copyBuffer(app, stagingBuffer, app.indexBuffer, bufferSize);
}

void createUniformBuffers(RobotRampageClient& app) {
	app.uniformBuffers.clear();
	app.uniformBuffersMemory.clear();
	app.uniformBuffersMapped.clear();

	for (size_t i = 0; i < Constants::MAX_FRAMES_IN_FLIGHT; i++) {
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		vk::raii::Buffer buffer({});
		vk::raii::DeviceMemory bufferMem({});

		createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, buffer, bufferMem);

		app.uniformBuffers.emplace_back(std::move(buffer));
		app.uniformBuffersMemory.emplace_back(std::move(bufferMem));
		app.uniformBuffersMapped.emplace_back(app.uniformBuffersMemory[i].mapMemory(0, bufferSize));
	}
}

void updateUniformBuffer(RobotRampageClient& app, uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(app.swapChainExtent.width) / static_cast<float>(app.swapChainExtent.height), 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	memcpy(app.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
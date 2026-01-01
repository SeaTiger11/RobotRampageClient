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
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(app.commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(1);

	vk::raii::CommandBuffer commandCopyBuffer = std::move(app.device.allocateCommandBuffers(allocInfo).front());

	commandCopyBuffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	commandCopyBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&*commandCopyBuffer);

	app.queue.submit(submitInfo, nullptr);
	app.queue.waitIdle();
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

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, app.stagingBuffer, app.stagingBufferMemory);
	void* dataStaging = app.stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, app.vertices.data(), bufferSize);
	app.stagingBufferMemory.unmapMemory();

	createBuffer(app, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, app.vertexBuffer, app.vertexBufferMemory);

	copyBuffer(app, app.stagingBuffer, app.vertexBuffer, bufferSize);
}
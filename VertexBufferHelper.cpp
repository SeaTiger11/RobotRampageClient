#include "VertexBufferHelper.h";

uint32_t findMemoryType(RobotRampageClient& app, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memProperties = app.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type");
}

void createVertexBuffer(RobotRampageClient& app) {
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.setSize(sizeof(app.vertices[0]) * app.vertices.size());
	bufferInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
	bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

	app.vertexBuffer = vk::raii::Buffer(app.device, bufferInfo);

	vk::MemoryRequirements memRequirements = app.vertexBuffer.getMemoryRequirements();
	vk::MemoryAllocateInfo memoryAllocationInfo;
	memoryAllocationInfo.setAllocationSize(memRequirements.size);
	memoryAllocationInfo.setMemoryTypeIndex(findMemoryType(app, memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

	app.vertexBufferMemory = vk::raii::DeviceMemory(app.device, memoryAllocationInfo);
	app.vertexBuffer.bindMemory(*app.vertexBufferMemory, 0);

	void* data = app.vertexBufferMemory.mapMemory(0, bufferInfo.size);
	memcpy(data, app.vertices.data(), bufferInfo.size);
	app.vertexBufferMemory.unmapMemory();
}
#include "CommandPoolHelper.h";

void createCommandPool(vk::raii::CommandPool& commandPool, uint32_t& queueIndex, vk::raii::Device& device) {
	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(queueIndex);

	commandPool = vk::raii::CommandPool(device, poolInfo);
}
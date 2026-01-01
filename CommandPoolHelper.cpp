#include "CommandPoolHelper.h";

void createCommandPool(RobotRampageClient& app) {
	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	poolInfo.setQueueFamilyIndex(app.queueIndex);

	app.commandPool = vk::raii::CommandPool(app.device, poolInfo);
}
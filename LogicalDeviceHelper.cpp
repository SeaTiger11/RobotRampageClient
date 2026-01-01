#include "LogicalDeviceHelper.h";

void createLogicalDevice(RobotRampageClient& app) {
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = app.physicalDevice.getQueueFamilyProperties();

	// Get the first index into queueFamilyProperties that supports both graphics and present
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++) {
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && app.physicalDevice.getSurfaceSupportKHR(qfpIndex, *app.surface)) {
			app.queueIndex = qfpIndex;
			break;
		}
	}

	if (app.queueIndex == ~0) {
		throw std::runtime_error("Could not find a queue for graphics and present");
	}

	// Query for the vulkan 1.3 features (features are organised in feature structures based on which version they were released in, so Vulkan13Features is correct to use even with Vulkan 1.4)
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
	featureChain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = vk::True;
	featureChain.get<vk::PhysicalDeviceVulkan13Features>().synchronization2 = vk::True;
	featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = vk::True;
	featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = vk::True;

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.setQueueFamilyIndex(app.queueIndex)
		.setQueueCount(1)
		.setPQueuePriorities(&queuePriority);

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>())
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&deviceQueueCreateInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(Constants::deviceExtensions.size()))
		.setPpEnabledExtensionNames(Constants::deviceExtensions.data());

	app.device = vk::raii::Device(app.physicalDevice, deviceCreateInfo);
	app.queue = vk::raii::Queue(app.device, app.queueIndex, 0);
}
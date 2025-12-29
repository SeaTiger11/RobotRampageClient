#include "LogicalDeviceHelper.h";

void createLogicalDevice(vk::raii::Device& device, vk::raii::Queue& graphicsQueue, vk::raii::PhysicalDevice& physicalDevice) {
	// Find the index of the first queue family that supports graphics
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	
	auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); });
	if (graphicsQueueFamilyProperty == queueFamilyProperties.end()) throw std::runtime_error("No graphics queue family found");

	auto graphicsIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

	// Query for the vulkan 1.3 features (features are organised in feature structures based on which version they were released in, so Vulkan13Features is correct to use even with Vulkan 1.4)
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
	featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
	featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.setQueueFamilyIndex(graphicsIndex)
		.setQueueCount(1)
		.setPQueuePriorities(&queuePriority);

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>())
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&deviceQueueCreateInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(Constants::deviceExtensions.size()))
		.setPpEnabledExtensionNames(Constants::deviceExtensions.data());

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	graphicsQueue = vk::raii::Queue(device, graphicsIndex, 0);
}
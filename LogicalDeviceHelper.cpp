#include "LogicalDeviceHelper.h";

void createLogicalDevice(vk::raii::Device& device, vk::raii::Queue& queue, uint32_t& queueIndex, vk::raii::PhysicalDevice& physicalDevice, vk::raii::SurfaceKHR& surface) {
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

	// Get the first index into queueFamilyProperties that supports both graphics and present
	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++) {
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface)) {
			queueIndex = qfpIndex;
			break;
		}
	}

	if (queueIndex == ~0) {
		throw std::runtime_error("Could not find a queue for graphics and present");
	}

	// Query for the vulkan 1.3 features (features are organised in feature structures based on which version they were released in, so Vulkan13Features is correct to use even with Vulkan 1.4)
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
	featureChain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = vk::True;
	featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = vk::True;

	float queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
	deviceQueueCreateInfo.setQueueFamilyIndex(queueIndex)
		.setQueueCount(1)
		.setPQueuePriorities(&queuePriority);

	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>())
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&deviceQueueCreateInfo)
		.setEnabledExtensionCount(static_cast<uint32_t>(Constants::deviceExtensions.size()))
		.setPpEnabledExtensionNames(Constants::deviceExtensions.data());

	device = vk::raii::Device(physicalDevice, deviceCreateInfo);
	queue = vk::raii::Queue(device, queueIndex, 0);
}
#include "PhysicalDeviceHelper.h";

void pickPhysicalDevice(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Instance& instance) {
	auto devices = instance.enumeratePhysicalDevices();

	if (devices.empty()) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}

	// Use an ordered map to automatically sort candidates by increasing score
	std::multimap<int, vk::raii::PhysicalDevice> candidates;

	for (const auto& device : devices) {
		auto deviceProperties = device.getProperties();
		auto deviceFeatures = device.getFeatures();
		auto queueFamilies = device.getQueueFamilyProperties();
		uint32_t score = 0;

		// Currently application only uses Vulkan 1.3 features and the docs.vulkan.org tutoril recommended checking for this version of support. This may need to be changed to 1_4 in the future
		if (!(deviceProperties.apiVersion >= VK_API_VERSION_1_3))
			continue;

		// Application needs a queue that supports graphics commands
		const auto qfpIter = std::ranges::find_if(queueFamilies, [](vk::QueueFamilyProperties const& qfp) {
				return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
			});

		if (qfpIter == queueFamilies.end())
			continue;

		// Device needs to support all extensions listed in Constants
		auto extensions = device.enumerateDeviceExtensionProperties();
		bool found = true;
		for (auto const& extension : Constants::deviceExtensions) {
			auto extensionIter = std::ranges::find_if(extensions, [extension](auto const& ext) {return strcmp(ext.extensionName, extension) == 0; });
			found = found && extensionIter != extensions.end();
		}

		if (!found)
			continue;

		// Application requires gemotery shaders
		if (!deviceFeatures.geometryShader)
			continue;

		// Prioritise Discrete GPUs as they have a significant performance advantage
		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			score += 1000;
		}

		// Consider the maximum possible size of textures as it affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		candidates.insert(std::make_pair(score, device));
	}

	// Check if the best candidate is actually suitable
	if (candidates.rbegin()->first > 0) {
		physicalDevice = candidates.rbegin()->second;
	}
	else {
		throw std::runtime_error("Failed to find a suitable GPU");
	}
}
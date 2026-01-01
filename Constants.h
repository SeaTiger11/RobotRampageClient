#pragma once

namespace Constants {
	// Debug controls

	#ifndef NDEBUG
		constexpr bool enableValidationLayers = true;
	#else
		constexpr bool enableValidationLayers = false;
	#endif

	constexpr bool debugExtensions = false;

	// General Constants

	constexpr const char* AppName = "Robot Rampage";
	constexpr uint32_t AppVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr const char* EngineName = "Striatum";
	constexpr uint32_t EngineVersion = VK_MAKE_VERSION(2, 0, 0);

	constexpr uint32_t WIDTH = 800;
	constexpr uint32_t HEIGHT = 600;

	constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	constexpr std::array<char const*, 1> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	// The flags that should be debugged (all except for VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	constexpr vk::DebugUtilsMessageTypeFlagsEXT    messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	constexpr std::array<const char*, 4> deviceExtensions = {
		vk::KHRSwapchainExtensionName,
		vk::KHRSpirv14ExtensionName,
		vk::KHRSynchronization2ExtensionName,
		vk::KHRCreateRenderpass2ExtensionName
	};
}
#pragma once

namespace Constants {
	constexpr const char* AppName = "Robot Rampage";
	constexpr const uint32_t AppVersion = VK_MAKE_VERSION(1, 0, 0);
	constexpr const char* EngineName = "Striatum";
	constexpr const uint32_t EngineVersion = VK_MAKE_VERSION(2, 0, 0);

	constexpr const uint32_t WIDTH = 800;
	constexpr const uint32_t HEIGHT = 600;

	const std::vector<char const*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	#ifndef NDEBUG
		constexpr bool enableValidationLayers = false;
	#else
		constexpr bool enableValidationLayers = true;
	#endif

	// The flags that should be debugged (all except for VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	constexpr vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	constexpr vk::DebugUtilsMessageTypeFlagsEXT    messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
}
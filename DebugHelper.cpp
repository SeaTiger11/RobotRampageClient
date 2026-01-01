#include "DebugHelper.h";

void setupDebugMessenger(RobotRampageClient& app) {
	if (!Constants::enableValidationLayers) return;

	vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
	debugUtilsMessengerCreateInfoEXT.setMessageSeverity(Constants::severityFlags)
		.setMessageType(Constants::messageTypeFlags)
		.setPfnUserCallback(&debugCallback);

	app.debugMessenger = app.instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
	std::cerr << "Validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

	return vk::False;
}
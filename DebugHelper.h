#pragma once

#include <iostream>
#include "Constants.h"
#include "RobotRampageClient.h"

void setupDebugMessenger(RobotRampageClient& app);

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
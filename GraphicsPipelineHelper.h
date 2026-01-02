#pragma once

#include <fstream>
#include "SwapChainHelper.h";
#include "RobotRampageClient.h";

void createGraphicsPipeline(RobotRampageClient& app);

// Forward declaration, function is actually in ImageHelper.cpp
vk::Format findDepthFormat(RobotRampageClient& app);
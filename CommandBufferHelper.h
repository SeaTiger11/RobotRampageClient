#pragma once

#include "RobotRampageClient.h";
#include "Constants.h";

void createCommandBuffers(RobotRampageClient& app);

void recordCommandBuffer(RobotRampageClient& app, uint32_t imageIndex);
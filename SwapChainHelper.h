#pragma once

#include "RobotRampageClient.h";

void createSwapChain(RobotRampageClient& app);

void cleanupSwapChain(RobotRampageClient& app);

void recreateSwapChain(RobotRampageClient& app);

// Forward declaration, function is actually in ImageViewHelper.cpp
void createImageViews(RobotRampageClient& app);
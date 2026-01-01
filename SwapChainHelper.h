#pragma once

#include "RobotRampageClient.h";

void createImageViews(RobotRampageClient& app);

void createSwapChain(RobotRampageClient& app);

void cleanupSwapChain(RobotRampageClient& app);

void recreateSwapChain(RobotRampageClient& app);
#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "RobotRampageClient.h";
#include "Constants.h";

void createVertexBuffer(RobotRampageClient& app);

void createIndexBuffer(RobotRampageClient& app);

void createUniformBuffers(RobotRampageClient& app);

void updateUniformBuffer(RobotRampageClient& app, uint32_t currentImage);
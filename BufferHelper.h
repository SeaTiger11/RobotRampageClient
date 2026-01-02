#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "RobotRampageClient.h";
#include "Constants.h";
#include "CommandBufferHelper.h";

uint32_t findMemoryType(RobotRampageClient& app, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

void createBuffer(RobotRampageClient& app, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory);

void createVertexBuffer(RobotRampageClient& app);

void createIndexBuffer(RobotRampageClient& app);

void createUniformBuffers(RobotRampageClient& app);

void updateUniformBuffer(RobotRampageClient& app, uint32_t currentImage);
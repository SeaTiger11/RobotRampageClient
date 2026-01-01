#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include "SwapChainHelper.h";

void createCommandBuffer(vk::raii::CommandBuffer& commandBuffer, vk::raii::CommandPool& commandPool, vk::raii::Device& device);
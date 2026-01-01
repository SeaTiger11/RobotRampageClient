#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include "SwapChainHelper.h";
#include "Constants.h";

void createCommandBuffer(std::vector<vk::raii::CommandBuffer>& commandBuffers, vk::raii::CommandPool& commandPool, vk::raii::Device& device);

void recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& commandBuffer, SwapChainData& swapChainData, vk::raii::Pipeline& graphicsPipeline);
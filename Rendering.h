#pragma once

#include <iostream>
#include "CommandBufferHelper.h";
#include "Constants.h";

// Used for the structs
#include "SyncObjectsHelper.h";
#include "SwapChainHelper.h";

void drawFrame(vk::raii::Device& device, std::vector<vk::raii::CommandBuffer>& commandBuffers, vk::raii::Pipeline& graphicsPipeline, vk::raii::Queue& queue, SyncObjects& syncObjects, SwapChainData& swapChainData, uint32_t& frameIndex);
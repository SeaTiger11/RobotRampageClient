#pragma once

#include "CommandBufferHelper.h"

// Used for the structs
#include "SyncObjectsHelper.h";
#include "SwapChainHelper.h";

void drawFrame(vk::raii::Device& device, vk::raii::CommandBuffer& commandBuffer, vk::raii::Pipeline& graphicsPipeline, vk::raii::Queue& queue, SyncObjects& syncObjects, SwapChainData& swapChainData);
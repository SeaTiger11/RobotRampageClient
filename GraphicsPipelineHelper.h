#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <fstream>
#include "SwapChainHelper.h";

void createGraphicsPipeline(vk::raii::Pipeline& graphicsPipeline, vk::raii::PipelineLayout& pipelineLayout, vk::raii::Device& device, SwapChainData& swapChainData);
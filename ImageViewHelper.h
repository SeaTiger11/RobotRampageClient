#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include "SwapChainHelper.h";

void createImageViews(std::vector<vk::raii::ImageView>& swapChainImageViews, SwapChainData& swapChainData, vk::raii::Device& device);
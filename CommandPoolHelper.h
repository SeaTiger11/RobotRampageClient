#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

void createCommandPool(vk::raii::CommandPool& commandPool, uint32_t& queueIndex, vk::raii::Device& device);
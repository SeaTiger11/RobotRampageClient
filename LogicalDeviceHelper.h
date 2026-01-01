#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <iostream>;
#include "Constants.h"

void createLogicalDevice(vk::raii::Device& device, vk::raii::Queue& queue, uint32_t& queueIndex, vk::raii::PhysicalDevice& physicalDevice, vk::raii::SurfaceKHR& surface);
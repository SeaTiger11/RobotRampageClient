#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include <fstream>

void createGraphicsPipeline(vk::raii::PipelineLayout& pipelineLayout, vk::raii::Device& device);
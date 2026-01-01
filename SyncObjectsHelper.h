#pragma once

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#include "Constants.h";
#include "SwapChainHelper.h";

struct SyncObjects {
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;
};

void createSyncObjects(SyncObjects& syncObjects, vk::raii::Device& device, SwapChainData& swapChainData);
#include "SyncObjectsHelper.h";

void createSyncObjects(SyncObjects& syncObjects, vk::raii::Device& device, SwapChainData& swapChainData) {
    assert(syncObjects.presentCompleteSemaphores.empty() && syncObjects.renderFinishedSemaphores.empty() && syncObjects.inFlightFences.empty());

    for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
        syncObjects.renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
    }

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < Constants::MAX_FRAMES_IN_FLIGHT; i++) {
        syncObjects.presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(device, vk::SemaphoreCreateInfo()));
        syncObjects.inFlightFences.emplace_back(vk::raii::Fence(device, fenceInfo));
    }
}
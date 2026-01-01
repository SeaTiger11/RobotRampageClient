#include "SyncObjectsHelper.h";

void createSyncObjects(SyncObjects& syncObjects, vk::raii::Device& device) {
    syncObjects.presentCompleteSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
    syncObjects.renderFinishedSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    syncObjects.drawFence = vk::raii::Fence(device, fenceInfo);
}
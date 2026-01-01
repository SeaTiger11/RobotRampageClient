#include "SyncObjectsHelper.h";

void createSyncObjects(RobotRampageClient& app) {
    assert(app.presentCompleteSemaphores.empty() && app.renderFinishedSemaphores.empty() && app.inFlightFences.empty());

    for (size_t i = 0; i < app.swapChainImages.size(); i++) {
        app.renderFinishedSemaphores.emplace_back(app.device, vk::SemaphoreCreateInfo());
    }

    vk::FenceCreateInfo fenceInfo;
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (size_t i = 0; i < Constants::MAX_FRAMES_IN_FLIGHT; i++) {
        app.presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(app.device, vk::SemaphoreCreateInfo()));
        app.inFlightFences.emplace_back(vk::raii::Fence(app.device, fenceInfo));
    }
}
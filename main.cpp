#include "RobotRampageClient.h";

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "Constants.h";

#include "InstanceHelper.h";
#include "DebugHelper.h";
#include "SurfaceHelper.h";
#include "PhysicalDeviceHelper.h";
#include "LogicalDeviceHelper.h";
#include "SwapChainHelper.h";
#include "GraphicsPipelineHelper.h";
#include "CommandPoolHelper.h";
#include "CommandBufferHelper.h";
#include "SyncObjectsHelper.h";
#include "BufferHelper.h";
#include "DescriptorSetHelper.h";
#include "TextureHelper.h";

void RobotRampageClient::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void RobotRampageClient::framebufferResizeCallback (GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<RobotRampageClient*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void RobotRampageClient::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(Constants::WIDTH, Constants::HEIGHT, Constants::AppName, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void RobotRampageClient::initVulkan() {
    createInstance(*this);
    setupDebugMessenger(*this);
    createSurface(*this);
    pickPhysicalDevice(*this);
    createLogicalDevice(*this);
    createSwapChain(*this);
    createImageViews(*this);
    createDescriptorSetLayout(*this);
    createGraphicsPipeline(*this);
    createCommandPool(*this);
    createTextureImage(*this);
    createVertexBuffer(*this);
    createIndexBuffer(*this);
    createUniformBuffers(*this);
    createDescriptorPool(*this);
    createDescriptorSets(*this);
    createCommandBuffers(*this);
    createSyncObjects(*this);
}

void RobotRampageClient::drawFrame() {
    auto fenceResult = device.waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence");
    }

    auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[frameIndex], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain(*this);
        return;
    }
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire swap chain images");
    }

    device.resetFences(*inFlightFences[frameIndex]);

    updateUniformBuffer(*this, frameIndex);

    commandBuffers[frameIndex].reset();
    recordCommandBuffer(*this, imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(&*presentCompleteSemaphores[frameIndex]);
    submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*commandBuffers[frameIndex]);
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&*renderFinishedSemaphores[imageIndex]);

    queue.submit(submitInfo, inFlightFences[frameIndex]);

    try {
        vk::PresentInfoKHR presentInfoKHR;
        presentInfoKHR.setWaitSemaphoreCount(1);
        presentInfoKHR.setPWaitSemaphores(&*renderFinishedSemaphores[imageIndex]);
        presentInfoKHR.setSwapchainCount(1);
        presentInfoKHR.setPSwapchains(&*swapChain);
        presentInfoKHR.setPImageIndices(&imageIndex);

        result = queue.presentKHR(presentInfoKHR);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain(*this);
        }
        else if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
    catch (const vk::SystemError& e) {
        if (e.code().value() == static_cast<int>(vk::Result::eErrorOutOfDateKHR)) {
            recreateSwapChain(*this);
            return;
        }
        else {
            throw;
        }
    }

    frameIndex = (frameIndex + 1) % Constants::MAX_FRAMES_IN_FLIGHT;
}

void RobotRampageClient::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    device.waitIdle();
}

void RobotRampageClient::cleanup() {
    cleanupSwapChain(*this);

    glfwDestroyWindow(window);

    glfwTerminate();
}

int main() {
    RobotRampageClient app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
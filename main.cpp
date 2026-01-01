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

class RobotRampageClient {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window = nullptr;

    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;

    vk::raii::Queue queue = nullptr;
    uint32_t queueIndex = ~0;

    SwapChainData swapChainData;

    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline graphicsPipeline = nullptr;

    vk::raii::CommandPool commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> commandBuffers;

    SyncObjects syncObjects;

    uint32_t frameIndex = 0;

    bool framebufferResized = false;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<RobotRampageClient*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(Constants::WIDTH, Constants::HEIGHT, Constants::AppName, nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void initVulkan() {
        createInstance(context, instance);
        setupDebugMessenger(debugMessenger, instance);
        createSurface(surface, instance, window);
        pickPhysicalDevice(physicalDevice, instance);
        createLogicalDevice(device, queue, queueIndex, physicalDevice, surface);
        createSwapChain(swapChainData, physicalDevice, device, surface, window);
        createImageViews(swapChainData, device);
        createGraphicsPipeline(graphicsPipeline, pipelineLayout, device, swapChainData);
        createCommandPool(commandPool, queueIndex, device);
        createCommandBuffer(commandBuffers, commandPool, device);
        createSyncObjects(syncObjects, device, swapChainData);
    }

    void drawFrame() {
        auto fenceResult = device.waitForFences(*syncObjects.inFlightFences[frameIndex], vk::True, UINT64_MAX);
        if (fenceResult != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to wait for fence");
        }

        auto [result, imageIndex] = swapChainData.swapChain.acquireNextImage(UINT64_MAX, *syncObjects.presentCompleteSemaphores[frameIndex], nullptr);

        if (result == vk::Result::eErrorOutOfDateKHR) {
            recreateSwapChain(swapChainData, physicalDevice, device, surface, window);
            return;
        }
        if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
            throw std::runtime_error("Failed to acquire swap chain images");
        }

        device.resetFences(*syncObjects.inFlightFences[frameIndex]);

        commandBuffers[frameIndex].reset();
        recordCommandBuffer(imageIndex, commandBuffers[frameIndex], swapChainData, graphicsPipeline);

        vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vk::SubmitInfo submitInfo;
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(&*syncObjects.presentCompleteSemaphores[frameIndex]);
        submitInfo.setPWaitDstStageMask(&waitDestinationStageMask);
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&*commandBuffers[frameIndex]);
        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(&*syncObjects.renderFinishedSemaphores[imageIndex]);

        queue.submit(submitInfo, syncObjects.inFlightFences[frameIndex]);

        try {
            vk::PresentInfoKHR presentInfoKHR;
            presentInfoKHR.setWaitSemaphoreCount(1);
            presentInfoKHR.setPWaitSemaphores(&*syncObjects.renderFinishedSemaphores[imageIndex]);
            presentInfoKHR.setSwapchainCount(1);
            presentInfoKHR.setPSwapchains(&*swapChainData.swapChain);
            presentInfoKHR.setPImageIndices(&imageIndex);

            result = queue.presentKHR(presentInfoKHR);
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
                framebufferResized = false;
                recreateSwapChain(swapChainData, physicalDevice, device, surface, window);
            }
            else if (result != vk::Result::eSuccess) {
                throw std::runtime_error("failed to present swap chain image!");
            }
        }
        catch (const vk::SystemError& e) {
            if (e.code().value() == static_cast<int>(vk::Result::eErrorOutOfDateKHR)) {
                recreateSwapChain(swapChainData, physicalDevice, device, surface, window);
                return;
            }
            else {
                throw;
            }
        }

        frameIndex = (frameIndex + 1) % Constants::MAX_FRAMES_IN_FLIGHT;
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }

        device.waitIdle();
    }

    void cleanup() {
        cleanupSwapChain(swapChainData);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

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
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
#include "ImageViewHelper.h";
#include "GraphicsPipelineHelper.h";

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

    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::Queue presentQueue = nullptr;

    SwapChainData swapChainData;
    std::vector<vk::raii::ImageView> swapChainImageViews;

    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline graphicsPipeline = nullptr;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, false);

        window = glfwCreateWindow(Constants::WIDTH, Constants::HEIGHT, Constants::AppName, nullptr, nullptr);
    }

    void initVulkan() {
        createInstance(context, instance);
        setupDebugMessenger(debugMessenger, instance);
        createSurface(surface, instance, window);
        pickPhysicalDevice(physicalDevice, instance);
        createLogicalDevice(device, graphicsQueue, presentQueue, physicalDevice, surface);
        createSwapChain(swapChainData, physicalDevice, device, surface, window);
        createImageViews(swapChainImageViews, swapChainData, device);
        createGraphicsPipeline(graphicsPipeline, pipelineLayout, device, swapChainData);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
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
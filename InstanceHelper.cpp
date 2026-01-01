#include "InstanceHelper.h";

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (Constants::enableValidationLayers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}

void createInstance(RobotRampageClient& app) {
    constexpr vk::ApplicationInfo appInfo{ 
        Constants::AppName, 
        Constants::AppVersion, 
        Constants::EngineName, 
        Constants::EngineVersion, 
        vk::ApiVersion14
    };

    // Get the required layers
    std::vector<char const*> requiredLayers;
    if (Constants::enableValidationLayers) {
        requiredLayers.assign(Constants::validationLayers.begin(), Constants::validationLayers.end());
    }

    // Check if the required layers are supported by the vulkan implementation
    auto layerProperties = app.context.enumerateInstanceLayerProperties();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) {
                return strcmp(layerProperty.layerName, requiredLayer) == 0;
            });
        })) {
        throw std::runtime_error("One or more required layers are not supported");
    }

    // Get the required instance extensions from GLFW.
    auto requiredExtensions = getRequiredExtensions();

    // Check if the required GLFW extensions are supported by the Vulkan implementation.
    auto extensionProperties = app.context.enumerateInstanceExtensionProperties();
    if (Constants::enableValidationLayers && Constants::debugExtensions) {
        std::cout << "Available extensions:" << std::endl;

        for (const auto& extension : extensionProperties) {
            std::cout << '\t' << extension.extensionName << std::endl;
        }

        std::cout << '\n' << "Required Extensions:" << std::endl;

        for (const auto& requiredExtension : requiredExtensions) {
            std::cout << '\t' << requiredExtension << std::endl;
        }
    }

    for (auto const &requiredExtension : requiredExtensions) {
        if (std::ranges::none_of(extensionProperties, [requiredExtension](auto const& extensionProperty) {
                return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
            })) {
            throw std::runtime_error("Required extension not supported: " + std::string(requiredExtension));
        }
    }

    vk::InstanceCreateInfo createInfo;
    createInfo.setPApplicationInfo(&appInfo)
        .setEnabledLayerCount(static_cast<uint32_t>(requiredLayers.size()))
        .setPpEnabledLayerNames(requiredLayers.data())
        .setEnabledExtensionCount(requiredExtensions.size())
        .setPpEnabledExtensionNames(requiredExtensions.data());

    app.instance = vk::raii::Instance(app.context, createInfo);
}


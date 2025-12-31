#include "SurfaceHelper.h";

void createSurface(vk::raii::SurfaceKHR& surface, vk::raii::Instance& instance, GLFWwindow* window) {
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
		throw std::runtime_error("Failed to create window surface");
	}

	surface = vk::raii::SurfaceKHR(instance, _surface);
}
#include "SurfaceHelper.h";

void createSurface(RobotRampageClient& app) {
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*app.instance, app.window, nullptr, &_surface) != 0) {
		throw std::runtime_error("Failed to create window surface");
	}

	app.surface = vk::raii::SurfaceKHR(app.instance, _surface);
}
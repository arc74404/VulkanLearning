#include "window.hpp"
#include <iostream>
#include <fstream>

#include "sources/renderer/vk_renderer.hpp"

Window::Window(uint32_t x, uint32_t y, const std::string &title)
{
	if (!glfwInit())
	{
		std::cerr << "GLFW initialization failed!" << std::endl;
		throw std::exception("glfw init failed");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_window = glfwCreateWindow(x, y, title.data(), nullptr, nullptr);

	if (m_window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		throw std::exception("Window did not create");
	}
}

int32_t Window::run(VkContext *vk_context)
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		if (!vk_render(vk_context))
			return -1;
	}
	return 0;
}

GLFWwindow *Window::getGlfwWindowPtr()
{
	return m_window;
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
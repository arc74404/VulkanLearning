#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>
#include "vulkan/vulkan.h"

class Renderer;

class VkContext;

class Window
{
public:
	Window(uint32_t x, uint32_t y, const std::string &title);

	VkRect2D getScreenRect();

	GLFWwindow *getGlfwWindowPtr();

	int32_t run(VkContext *vk_context);

	int32_t run(Renderer *renderer);

	~Window();

private:
	GLFWwindow *m_window;

	VkRect2D m_screen_rect;
};

#endif // WINDOW_HPP
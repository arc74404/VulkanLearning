#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>

class VkContext;

class Window
{
public:
	Window(uint32_t x, uint32_t y, const std::string &title);

	GLFWwindow *getGlfwWindowPtr();

	int32_t run(VkContext *vk_context);

	~Window();

private:
	GLFWwindow *m_window;
};

#endif // WINDOW_HPP
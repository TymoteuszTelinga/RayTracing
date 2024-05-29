#include "Input.h"

#include "Application.h"
#include <GLFW/glfw3.h>
#include <iostream>

bool Input::IsKeyDown(uint32_t keycode)
{
	GLFWwindow* window = Application::Get().GetWindow();
	int state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonDown(uint32_t button)
{
	GLFWwindow* window = Application::Get().GetWindow();
	int state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
	GLFWwindow* window = Application::Get().GetWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return {(float)xpos, (float)ypos};
}

void Input::SetScrollCalback()
{
	GLFWwindow* window = Application::Get().GetWindow();
	auto callback = [](GLFWwindow* window, double xoffset, double yoffset)
	{
		std::cout << yoffset <<'\n';
	};
	glfwSetScrollCallback(window, callback);
}

void Input::SetCursorMode(uint32_t mode)
{
	GLFWwindow* window = Application::Get().GetWindow();
	glfwSetInputMode(window, GLFW_CURSOR, mode);
}

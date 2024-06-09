#pragma once

#include <string>
#include <vector>
#include "Base.h"
#include "Layer.h"

struct GLFWwindow;

struct ApplicationSpecification
{
	std::string Name = "Application";
	uint32_t Width = 1280;
	uint32_t Height = 720;
};

class Application
{
public:
	Application(const ApplicationSpecification& spec);
	~Application();
	static Application& Get();
	GLFWwindow* GetWindow() const { return m_Window; };

	void Run();

	template<typename T>
	void PushLayer()
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
	}

private:
	void Init();
	void Shutdown();

private:
	ApplicationSpecification m_Specification;
	GLFWwindow* m_Window = nullptr;

	bool m_Runing = false;
	float m_DeltaTime = 0.0f;
	float m_LastTime = 0.0f;
	std::vector<Ref<Layer>> m_LayerStack;
};


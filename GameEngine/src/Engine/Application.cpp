#include "enginepch.h"
#include "Application.h"

#include "Engine/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Engine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application()
	{
		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(ENGINE_BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		float r = 1.0f, g = 0.0f, b = 0.0f;
		const float increment = 0.005f;
		while (m_Running)
		{
			if (r <= 1.0f && g <= 0.0f && b <= 0.0f)
				r += increment;
			else if (r >= 1.0f && g <= 1.0f && b <= 0.0f)
				g += increment;
			else if (g >= 1.0f && r >= 0.0f && b <= 0.0f)
				r -= increment;
			else if (g >= 1.0f && b <= 1.0f && r <= 0.0f)
				b += increment;
			else if (b >= 1.0f && g >= 0.0f && r <= 0.0f)
				g -= increment;
			else if (b >= 1.0f && r <= 1.0f && g <= 0.0f)
				r += increment;
			else if (r >= 1.0f && b >= 0.0f && g <= 0.0f)
				b -= increment;

			ENGINE_CORE_INFO("R: {0}, G: {1}, B: {2} ", r, g, b);

			glClearColor(r, g, b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			auto[x, y] = Input::GetMousePosition();
			ENGINE_CORE_TRACE("{0}, {1}", x, y);

			auto state = Input::IsMouseButtonReleased(0);
			ENGINE_CORE_TRACE("{0}", state);
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}

#include "enginepch.h"
#include "Engine/Core/Application.h"

#include "Engine/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application(const std::string& name)
	{
		ENGINE_PROFILE_FUNCTION();
		
		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = Scope<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallBack(ENGINE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		ENGINE_PROFILE_FUNCTION();
		
		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{	
		ENGINE_PROFILE_FUNCTION();
		
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");
		
			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");
					
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}

			m_ImGuiLayer->Begin();
			{
				ENGINE_PROFILE_SCOPE("LayerStack OnImGuiRender");
				
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		
		return false;
	}
}

#include "enginepch.h"
#include "Engine/Core/Application.h"

#include "Engine/Asset/AssetManager.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/UI/UIEngine.h"

#include "Engine/Utils/PlatformUtils.h"

namespace Engine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application(const ApplicationSpecification& specification)
		:m_Specification(specification)
	{
		ENGINE_PROFILE_FUNCTION();
		
		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
		{
			std::filesystem::current_path(m_Specification.WorkingDirectory);
		}

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallBack(ENGINE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		ENGINE_PROFILE_FUNCTION();

		ScriptEngine::Shutdown();
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
		if (m_Specification.Runtime)
			m_Running = false;
		else
			ENGINE_CORE_TRACE("\"Application Close\"");
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
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
		
			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

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

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}

}
